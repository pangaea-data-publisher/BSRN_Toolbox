// -------------------------------------------------------------------------------
//  Project:		Qt Webbased File Handling
//
//  File:           webfile.cpp
//
// (c) 2012 by Oblivion Software/Norbert Schlia
// All rights reserved.
// -------------------------------------------------------------------------------
//
#include "Webfile.h"

#include <QTimer>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QThread>
#include <QCoreApplication>

webfile::webfile(QObject *parent /*= 0*/) :
    QObject(parent),
    m_pNetworkProxy(NULL)
{
    clear();

    // QThread is required, otherwise QEventLoop will block
    m_pSocketThread = new QThread(this);
    moveToThread(m_pSocketThread);
    m_pSocketThread->start(QThread::HighestPriority);
}

webfile::~webfile()
{
    // Ensure we exit all loops
    m_loop.exit(1);
    // Close any connection
    close();
    // End socket thread
    if (m_pSocketThread != NULL)
    {
        m_pSocketThread->quit();
        m_pSocketThread->wait();

        delete m_pSocketThread;
    }
}

// Reset all variables (e.g for open())
void webfile::clear()
{
    m_pReply            = NULL;
    m_offset            = 0;
    m_nSize             = 0;
    m_nPos              = 0;
    m_nReceivedPos      = 0;
    m_nOverHead         = 0;
    m_nReadFails        = 0;
    m_nResponse         = 0;
    m_nSize             = 0;
    m_nOpenTimeOutms    = 5000; /* ms */
    m_nReadTimeOutms    = 5000; /* ms */
    m_nBufferSize       = (1024*1204); /* = 1 MB */

    m_bReadTimeOut      = false;
    m_bResetMonitor     = false;
    m_bHaveSize         = false;

    m_NetworkError      = QNetworkReply::NoError;

    m_strContentType.clear();
    m_sslerrors.clear();
}

// The url for open() (similar to file for QFile)
QUrl webfile::url() const
{
    return m_url;
}

void webfile::setUrl(const QString & strUrl)
{
    m_url = QUrl::fromEncoded(strUrl.toUtf8());
}

void webfile::setUrl(const QUrl & Url)
{
    m_url = Url;
}

// Username and password for site, if required
void webfile::setUser(const QString & strUser)
{
    m_strUser = strUser;
}

void webfile::setPassword(const QString & strPassword)
{
    m_strPassword = strPassword;
}

// Actually "open" the file - connect to remote site
bool webfile::open(qint64 offset /*= 0*/)
{
    bool bSuccess = false;

    if (isGuiThread())
    {
        // For GUI threads, we use the non-blocking call and use QEventLoop to wait and yet keep the GUI alive
        QMetaObject::invokeMethod(this, "slotOpen", Qt::QueuedConnection,
                                  Q_ARG(void *, &bSuccess),
                                  Q_ARG(void *, &m_loop),
                                  Q_ARG(qint64, offset));
        m_loop.exec();
    }
    else
    {
        // For non-GUI threads, QEventLoop would cause a deadlock, so we simply use a blocking call.
        // (Does not hurt as no messages need to be processed either during the open operation).
        QMetaObject::invokeMethod(this, "slotOpen", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, &bSuccess),
                                  Q_ARG(void *, NULL),
                                  Q_ARG(qint64, offset));
    }

    return bSuccess;    // Please note that it's perfectly safe to wait on the return Q_ARG, as we wait for the invokeMethod call to complete.
}

void webfile::slotOpen(void *pReturnSuccess, void *pLoop, qint64 offset /*= 0*/)
{
    *(bool*)pReturnSuccess = workerOpen(offset);

    if (pLoop != NULL)
    {
        QMetaObject::invokeMethod((QEventLoop*)pLoop, "quit", Qt::QueuedConnection);
    }
}

bool webfile::workerOpen(qint64 offset /*= 0*/)
{
    clear();

    resetReadFails();

    m_bResetMonitor = true; // Schedule reset

    close();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    if (m_pNetworkProxy != NULL)
        manager->setProxy(*m_pNetworkProxy);

//  connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
    connect(manager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    QNetworkRequest request;

    request.setUrl(m_url);
    request.setRawHeader("User-Agent", "Qt NetworkAccess 1.2");

    m_nPos = offset;

    if (m_nPos)
    {
        QByteArray data;
        QString strData("bytes=" + QString::number(m_nPos) + "-");

        data = strData.toLatin1();
        request.setRawHeader("Range", data);

        // Range: bytes=20000000-\r\n
        // Range: bytes=0-100\r\n
        // etc.
    }

    m_pReply = manager->get(request);

    if (m_pReply == NULL)
    {
        m_NetworkError = QNetworkReply::UnknownNetworkError;
        return false;
    }

    // Set the read buffer size
    m_pReply->setReadBufferSize(m_nBufferSize);

    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));
    connect(m_pReply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(slotSslErrors(QList<QSslError>)));

    if (!waitForConnect(m_nOpenTimeOutms, manager))
    {
        m_NetworkError = QNetworkReply::TimeoutError;
        return false;
    }

    if (m_pReply == NULL)
    {
        m_NetworkError = QNetworkReply::OperationCanceledError;
        return false;
    }

    if (m_pReply->error() != QNetworkReply::NoError)
    {
        m_NetworkError = m_pReply->error();
        return false;
    }

    m_NetworkError      = m_pReply->error();

    m_strContentType    = m_pReply->header(QNetworkRequest::ContentTypeHeader).toString();
    m_LastModified      = m_pReply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
    m_nSize             = m_pReply->header(QNetworkRequest::ContentLengthHeader).toULongLong();

    m_nResponse         = m_pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_strResponse       = m_pReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    m_bHaveSize =  (m_nSize ? true : false);

    m_nSize += m_nPos;

    if (error() != QNetworkReply::NoError)
    {
        m_NetworkError = m_pReply->error();
        return false;
    }

    m_NetworkError = response2error(m_nResponse);

    return (response() == 200 || response() == 206);
}

void webfile::close()
{
    if (m_pReply != NULL)
    {
        m_pReply->close();
        m_pReply->deleteLater();
        m_pReply = NULL;
    }
}

// File size
qint64 webfile::size() const
{
    if (m_bHaveSize)
    {
        return m_nSize;                     // size as reported by content-length header
    }
    else
    {
        return m_nReceivedPos + m_nSize;    // content-length header was missing, fake size
    }
}

// Current read position
qint64 webfile::pos() const
{
    return m_nPos;
}

// "seek" to position - actually reopen and skip to desired byte offset
bool webfile::seek(qint64 offset)
{
    return open(offset);
}

// Return "true" if file was completely read
bool webfile::atEnd() const
{
    return (pos() >= size() && isFinished());
}

// Read block from file
qint64 webfile::read(char *data, qint64 maxlen)
{
    qint64 nBytesRead = 0;

    if (isGuiThread())
    {
        // For GUI threads, we use the non-blocking call and use QEventLoop to wait and yet keep the GUI alive
        //QEventLoop loop;
        QMetaObject::invokeMethod(this, "slotRead", Qt::QueuedConnection,
                                  Q_ARG(void *, &nBytesRead),
                                  Q_ARG(void *, &m_loop),
                                  Q_ARG(void *, data),
                                  Q_ARG(qint64, maxlen));
        m_loop.exec();
    }
    else
    {
        // For non-GUI threads, QEventLoop would cause a deadlock, so we simply use a blocking call.
        // (Does not hurt as no messages need to be processed either during the read operation).
        QMetaObject::invokeMethod(this, "slotRead", Qt::BlockingQueuedConnection,
                                  Q_ARG(void *, &nBytesRead),
                                  Q_ARG(void *, NULL),
                                  Q_ARG(void *, data),
                                  Q_ARG(qint64, maxlen));
    }

    return nBytesRead;  // Please note that it's perfectly safe to wait on the return Q_ARG, as we wait for the invokeMethod call to complete.
}

void webfile::slotRead(void *pResultBytesRead, void *pLoop, void * data, qint64 maxlen)
{
    *(qint64*)pResultBytesRead = workerRead((char*)data, maxlen);

    if (pLoop != NULL)
    {
        ((QEventLoop*)pLoop)->wakeUp();
        QMetaObject::invokeMethod((QEventLoop*)pLoop, "quit", Qt::QueuedConnection);
    }
}

qint64 webfile::workerRead(char *data, qint64 maxlen)
{
    qint64 nBytesRead = 0;

    if (m_NetworkError != QNetworkReply::NoError)
    {
        return -1;
    }

    m_bReadTimeOut = false;

    try
    {
        qint64 nSize = 0;

        if (m_pReply == NULL)
        {
            m_NetworkError = QNetworkReply::OperationCanceledError;
            throw (qint64)-1;
        }

        while ((nSize = m_pReply->size()) < maxlen && !isFinished())
        {
            m_nReadFails++; // Wait for more data to arrive

            if (!waitForData(m_nReadTimeOutms))
            {
                if (m_pReply != NULL)
                {
                    m_pReply->close();
                }
                m_NetworkError = QNetworkReply::TimeoutError;
                throw (qint64)-1;
            }
        }

        m_nReceivedPos += nSize - m_nOverHead;

        if (atEnd())
            throw (qint64)0;

        if (m_pReply == NULL)
        {
            m_NetworkError = QNetworkReply::OperationCanceledError;
            throw (qint64)-1;
        }

        nBytesRead = m_pReply->read(data, maxlen);

        m_nOverHead = nSize - nBytesRead;
        m_nPos += nBytesRead;
    }
    catch (qint64 _nBytesRead)
    {
        nBytesRead = _nBytesRead;
    }

    return nBytesRead;
}

// Return NetworkError code
QNetworkReply::NetworkError webfile::error() const
{
    return m_NetworkError;
}

// Return localised error string for network error (default: current code)
QString webfile::errorString(QNetworkReply::NetworkError networkerror /*= QNetworkReply::NoError*/) const
{
    if (networkerror == QNetworkReply::NoError)
    {
        networkerror = error();
    }

    weberror we;

    we.setError(networkerror);

    return we.errorString();
}

// Response code (200, 404, etc.)
int webfile::response() const
{
    return m_nResponse;
}

// Corresponding text to response code
QString webfile::responseString() const
{
    return m_strResponse;
}

// true if download finished
bool webfile::isFinished() const
{
    if (m_pReply != NULL)
    {
        return m_pReply->isFinished();
    }
    else
    {
        return true;
    }
}

// true if download still running
bool webfile::isRunning() const
{
    if (m_pReply != NULL)
    {
        return m_pReply->isRunning();
    }
    else
    {
        return false;
    }
}

bool webfile::isThreadRunning() const
{
    if (m_pSocketThread != NULL)
    {
        return m_pSocketThread->isRunning();
    }
    else
    {
        return false;
    }
}

// Number of "read fails", e.g., not enough data available as requested
int webfile::getReadFails() const
{
    return m_nReadFails;
}

void webfile::resetReadFails()
{
    m_nReadFails = 0;
}

// Set the current proxy. Must be done before open(), will not have effect otherwise
void webfile::setProxy(const QNetworkProxy * networkProxy)
{
    m_pNetworkProxy = networkProxy;
}

// Receive network error
void webfile::slotError(QNetworkReply::NetworkError error)
{
    m_NetworkError = error;
}

// Receive SSL errors
void webfile::slotSslErrors(QList<QSslError> sslerrors)
{
    m_sslerrors.append(sslerrors);
}

// Do proxy authentication
void webfile::slotProxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
{

    //    if (!proxy.user().isEmpty() && !proxy.password().isEmpty() &&
    //            (authenticator->user() != proxy.user() ||
    //            authenticator->password() != proxy.password()))
    {
        // Send credentials if changed and not empty
        authenticator->setUser(proxy.user());
        authenticator->setPassword(proxy.password());
    }

    // QNetworkProxy p(proxy);
    // p.setUser(strUser);
    // p.setPassword(strPassword);
    // QNetworkProxy::setApplicationProxy(p);
}

// Do remote server authentication
void webfile::slotAuthenticationRequired(QNetworkReply * /*reply*/, QAuthenticator * authenticator)
{
    if (!m_strUser.isEmpty() && !m_strPassword.isEmpty() &&
            (authenticator->user() != m_strUser ||
             authenticator->password() != m_strPassword))
    {
        // Send credentials if changed and not empty
        authenticator->setUser(m_strUser);
        authenticator->setPassword(m_strPassword);
    }
}

// Wait timeout
void webfile::slotWaitTimeout()
{
    m_bReadTimeOut = true;  // Report timeout
    emit signalReadTimeout();
}

// Open timeout
int webfile::getOpenTimeout() const
{
    return m_nOpenTimeOutms;
}

void webfile::setOpenTimeout(int nOpenTimeOutms)
{
    m_nOpenTimeOutms = nOpenTimeOutms;
}

// Read timeout
int webfile::getReadTimeout() const
{
    return m_nReadTimeOutms;
}

void webfile::setReadTimeout(int nReadTimeOutms)
{
    m_nReadTimeOutms = nReadTimeOutms;
}

// Internal: wait for connection to establish
bool webfile::waitForConnect(int nTimeOutms, QNetworkAccessManager *manager)
{
    QTimer *timer = NULL;
    QEventLoop eventLoop;
    bool bReadTimeOut = false;

    m_bReadTimeOut = false;

    if (nTimeOutms > 0)
    {
        timer = new QTimer(this);

        connect(timer, SIGNAL(timeout()), this, SLOT(slotWaitTimeout()));
        timer->setSingleShot(true);
        timer->start(nTimeOutms);

        connect(this, SIGNAL(signalReadTimeout()), &eventLoop, SLOT(quit()));
    }

    // Wait on QNetworkManager reply here
    connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));

    if (m_pReply != NULL)
    {
        // Preferrably we wait for the first reply which comes faster than the finished signal
        connect(m_pReply, SIGNAL(readyRead()), &eventLoop, SLOT(quit()));
    }
    eventLoop.exec();

    if (timer != NULL)
    {
        timer->stop();
        delete timer;
        timer = NULL;
    }

    bReadTimeOut = m_bReadTimeOut;

    m_bReadTimeOut = false;

    return !bReadTimeOut;
}

// Internal: wait for new data to arrive
bool webfile::waitForData(int nTimeOutms)
{
    QEventLoop  eventLoop;
    QTimer *    timer = NULL;
    bool        bReadTimeOut = false;

    m_bReadTimeOut = false;

    if (nTimeOutms > 0)
    {
        timer = new QTimer(this);	// ??? owner??

        connect(timer, SIGNAL(timeout()), this, SLOT(slotWaitTimeout()));
        timer->setSingleShot(true);
        timer->start(nTimeOutms);

        connect(this, SIGNAL(signalReadTimeout()), &eventLoop, SLOT(quit()));
    }

    if (m_pReply != NULL)
    {
        connect(m_pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        connect(m_pReply, SIGNAL(readyRead()), &eventLoop, SLOT(quit()));
    }

    eventLoop.exec();

    if (timer != NULL)
    {
        timer->stop();
        delete timer;
        timer = NULL;
    }

    bReadTimeOut = m_bReadTimeOut;

    m_bReadTimeOut = false;

    return !bReadTimeOut;
}

// Return content type of document, if available
QString webfile::getContentType() const
{
    return m_strContentType;
}

// Return last modified time of document, if available
QDateTime webfile::getLastModified() const
{
    return m_LastModified;
}

// Internal: detect if running as GUI thread
bool webfile::isGuiThread() const
{
    QCoreApplication *pCoreApp = QCoreApplication::instance();

    if (pCoreApp == NULL)
    {
        return false;       // No QApp object, should never happen, but no Qt GUI anyway.
    }
    return (QThread::currentThread() == pCoreApp->thread());
}

QNetworkReply * webfile::getReply()
{
    return m_pReply;
}

QNetworkReply::NetworkError webfile::response2error(int nResponse) const
{
    QNetworkReply::NetworkError error = QNetworkReply::NoError;

    switch (nResponse)
    {
    case 200:
    case 206:
        break;
    case 401:
        error = QNetworkReply::AuthenticationRequiredError;
        break;
    case 403:
        error = QNetworkReply::ContentOperationNotPermittedError;
        break;
    case 404:
        error = QNetworkReply::ContentNotFoundError;
        break;
    default:
        error = QNetworkReply::UnknownNetworkError;
        break;
    }

    return error;
}

void  webfile::setReadBufferSize(qint64 size)
{
    if (m_pReply != NULL)
    {
        m_pReply->setReadBufferSize(size);
    }
}

qint64 webfile::readBufferSize() const
{
    if (m_pReply != NULL)
    {
        return m_pReply->readBufferSize();
    }
    else
    {
        return -1;
    }
}

// -------------------------------------------------------------------------------
//  Project:        Qt Webbased File Handling
//
//  File:           weberror.cpp
//
// (c) 2012 by Oblivion Software/Norbert Schlia
// All rights reserved.
// -------------------------------------------------------------------------------
//
// #include "weberror.h"

weberror::weberror(QObject *parent) :
    QObject(parent)
{
}

void weberror::setError(QNetworkReply::NetworkError error)
{
    m_error = error;
}

QNetworkReply::NetworkError weberror::error() const
{
    return m_error;
}

void weberror::setSslError(QSslError::SslError error)
{
    m_sslerror = error;
}

QSslError::SslError weberror::errorSsl() const
{
    return m_sslerror;
}

QString weberror::errorString() const
{
    QString strErrorString;

    switch (m_error)
    {
    case QNetworkReply::NoError:
        strErrorString = tr("No error");
        break;
    case QNetworkReply::ConnectionRefusedError:
        strErrorString = tr("The remote server refused the connection (the server is not accepting requests).");
        break;
    case QNetworkReply::RemoteHostClosedError:
        strErrorString = tr("The remote server closed the connection prematurely, before the entire reply was received and processed.");
        break;
    case QNetworkReply::HostNotFoundError:
        strErrorString = tr("The remote host name was not found (invalid hostname).");
        break;
    case QNetworkReply::TimeoutError:
        strErrorString = tr("The connection to the remote server timed out.");
        break;
    case QNetworkReply::OperationCanceledError:
        strErrorString = tr("The operation was canceled via calls to abort() or close() before it was finished.");
        break;
    case QNetworkReply::SslHandshakeFailedError:
        strErrorString = tr("The SSL/TLS handshake failed and the encrypted channel could not be established.");
        break;
    case QNetworkReply::TemporaryNetworkFailureError:
        strErrorString = tr("The connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established.");
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        strErrorString = tr("The connection to the proxy server was refused (the proxy server is not accepting requests).");
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        strErrorString = tr("The proxy server closed the connection prematurely, before the entire reply was received and processed.");
        break;
    case QNetworkReply::ProxyNotFoundError:
        strErrorString = tr("The proxy host name was not found (invalid proxy hostname).");
        break;
    case QNetworkReply::ProxyTimeoutError:
        strErrorString = tr("The connection to the proxy timed out or the proxy did not reply in time to the request sent.");
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        strErrorString = tr("The proxy requires authentication in order to honour the request but did not accept any credentials offered (if any).");
        break;
    case QNetworkReply::ContentAccessDenied:
        strErrorString = tr("The access to the remote content was denied (similar to HTTP error 401).");
        break;
    case QNetworkReply::ContentOperationNotPermittedError:
        strErrorString = tr("The operation requested on the remote content is not permitted.");
        break;
    case QNetworkReply::ContentNotFoundError:
        strErrorString = tr("The remote content was not found at the server (similar to HTTP error 404).");
        break;
    case QNetworkReply::AuthenticationRequiredError:
        strErrorString = tr("The remote server requires authentication to serve the content but the credentials provided were not accepted (if any).");
        break;
    case QNetworkReply::ContentReSendError:
        strErrorString = tr("The request needed to be sent again, but this failed for example because the upload data could not be read a second time.");
        break;
    case QNetworkReply::ProtocolUnknownError:
        strErrorString = tr("The Network Access API cannot honor the request because the protocol is not known.");
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        strErrorString = tr("The requested operation is invalid for this protocol.");
        break;
    case QNetworkReply::UnknownNetworkError:
        strErrorString = tr("An unknown network-related error was detected.");
        break;
    case QNetworkReply::UnknownProxyError:
        strErrorString = tr("An unknown proxy-related error was detected.");
        break;
    case QNetworkReply::UnknownContentError:
        strErrorString = tr("An unknown error related to the remote content was detected.");
        break;
    case QNetworkReply::ProtocolFailure:
        strErrorString = tr("A breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.).");
        break;
    default:
        strErrorString = QString("Unknown error %1").arg(m_error);
    }

    return strErrorString;
}

// Return localised error string for SSL error
QString weberror::sslErrorString() const
{
    QString strErrorString;

    switch (m_sslerror)
    {
    case QSslError::NoError:
        strErrorString = tr("No error");
        break;
    case QSslError::UnableToGetIssuerCertificate:
        strErrorString = tr("The issuer certificate could not be found.");
        break;
    case QSslError::UnableToDecryptCertificateSignature:
        strErrorString = tr("The certificate signature could not be decrypted.");
        break;
    case QSslError::UnableToDecodeIssuerPublicKey:
        strErrorString = tr("The public key in the certificate could not be read.");
        break;
    case QSslError::CertificateSignatureFailed:
        strErrorString = tr("The signature of the certificate is invalid.");
        break;
    case QSslError::CertificateNotYetValid:
        strErrorString = tr("The certificate is not yet valid.");
        break;
    case QSslError::CertificateExpired:
        strErrorString = tr("The certificate has expired.");
        break;
    case QSslError::InvalidNotBeforeField:
        strErrorString = tr("The certificate's notBefore field contains an invalid time.");
        break;
    case QSslError::InvalidNotAfterField:
        strErrorString = tr("The certificate's notAfter field contains an invalid time.");
        break;
    case QSslError::SelfSignedCertificate:
        strErrorString = tr("The certificate is self-signed, and untrusted.");
        break;
    case QSslError::SelfSignedCertificateInChain:
        strErrorString = tr("The root certificate of the certificate chain is self-signed, and untrusted.");
        break;
    case QSslError::UnableToGetLocalIssuerCertificate:
        strErrorString = tr("The issuer certificate of a locally looked up certificate could not be found.");
        break;
    case QSslError::UnableToVerifyFirstCertificate:
        strErrorString = tr("No certificates could be verified.");
        break;
    case QSslError::InvalidCaCertificate:
        strErrorString = tr("One of the CA certificates is invalid.");
        break;
    case QSslError::PathLengthExceeded:
        strErrorString = tr("The basicConstraints path length parameter has been exceeded");
        break;
    case QSslError::InvalidPurpose:
        strErrorString = tr("The supplied certificate is unsuitable for this purpose.");
        break;
    case QSslError::CertificateUntrusted:
        strErrorString = tr("The root CA certificate is not trusted for this purpose.");
        break;
    case QSslError::CertificateRejected:
        strErrorString = tr("The root CA certificate is marked to reject the specified purpose.");
        break;
    case QSslError::SubjectIssuerMismatch: // hostname mismatch
        strErrorString = tr("The current candidate issuer certificate was rejected because its"
                            " subject name did not match the issuer name of the current certificate.");
        break;
    case QSslError::AuthorityIssuerSerialNumberMismatch:
        strErrorString = tr("The current candidate issuer certificate was rejected because"
                            " its issuer name and serial number was present and did not match the"
                            " authority key identifier of the current certificate.");
        break;
    case QSslError::NoPeerCertificate:
        strErrorString = tr("The peer did not present any certificate.");
        break;
    case QSslError::HostNameMismatch:
        strErrorString = tr("The host name did not match any of the valid hosts"
                            " for this certificate.");
        break;
    case QSslError::UnspecifiedError:
        strErrorString = tr("Unspecified error.");
        break;
    case QSslError::CertificateRevoked:
        strErrorString = tr("The peer presented a certificate that was revoked.");
        break;
    case QSslError::NoSslSupport:
        strErrorString = tr("No SSL support.");
        break;
    default:
        strErrorString = QString("Unknown SSL error %1").arg(m_sslerror);
    }

    return strErrorString;
}
