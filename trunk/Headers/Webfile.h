// -------------------------------------------------------------------------------
//  Project:		Qt Webbased File Handling
//
//  File:           webfile.h
//
// (c) 2012 by Oblivion Software/Norbert Schlia
// All rights reserved.
// -------------------------------------------------------------------------------
//
#pragma once

#include <QUrl>
#include <QList>
#include <QSslError>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDateTime>
#include <QTime>
#include <QObject>

#ifndef WEBFILE_H
#define WEBFILE_H

class webfile : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "author", "Norbert Schlia" )
    Q_CLASSINFO( "url", "http://www.oblivion-software.de" )

public:
    explicit webfile(QObject *parent = 0);
    virtual ~webfile();

    QUrl                        url() const;
    void                        setUrl(const QString &strUrl);
    void                        setUrl(const QUrl & Url);

    void                        setUser(const QString &strUser);
    void                        setPassword(const QString &strPassword);

    bool                        open(qint64 offset = 0);
    virtual void                close();

    virtual qint64              size() const;
    virtual qint64              pos() const;
    bool                        seek(qint64 offset);
    bool                        atEnd() const;

    qint64                      read(char *data, qint64 maxlen);

    QNetworkReply::NetworkError error() const;

    QString                     errorString(QNetworkReply::NetworkError networkerror = QNetworkReply::NoError) const;
    int                         response() const;
    QString                     responseString() const;

    bool                        isFinished() const;
    bool                        isRunning() const;
    bool                        isThreadRunning() const;

    int                         getReadFails() const;
    void                        resetReadFails();

    void                        setProxy(const QNetworkProxy * networkProxy);

    int                         getOpenTimeout() const;
    void                        setOpenTimeout(int nOpenTimeOutms);
    int                         getReadTimeout() const;
    void                        setReadTimeout(int nReadTimeOutms);

    bool                        waitForConnect(int nTimeOutms, QNetworkAccessManager *manager);
    bool                        waitForData(int nTimeOutms);

    QString                     getContentType() const;
    QDateTime                   getLastModified() const;

    QNetworkReply *             getReply();

    void                        setReadBufferSize(qint64 size);
    qint64                      readBufferSize() const;

protected:
    void                        clear();

    bool                        workerOpen(qint64 offset = 0);
    qint64                      workerRead(char *data, qint64 maxlen);

    bool                        isGuiThread() const;

    QNetworkReply::NetworkError response2error(int nResponse) const;

signals:
    void                        signalReadTimeout();

public slots:
    void                        slotOpen(void *pReturnSuccess, void *pLoop, qint64 offset = 0);
    void                        slotRead(void *pResultBytesRead, void *pLoop, void *data, qint64 maxlen);

private slots:
    void                        slotError(QNetworkReply::NetworkError);
    void                        slotSslErrors(QList<QSslError>);
    void                        slotProxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator);
    void                        slotAuthenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);
    void                        slotWaitTimeout();

protected:
    QEventLoop                  m_loop;

    QNetworkReply *             m_pReply;

    qint64                      m_offset;
    QUrl                        m_url;

    qint64                      m_nSize;
    bool                        m_bHaveSize;
    qint64                      m_nPos;
    qint64                      m_nReceivedPos;
    qint64                      m_nOverHead;

    QString                     m_strContentType;
    QString                     m_strLastModified;
    QDateTime                   m_LastModified;

    int                         m_nReadFails;

    QString                     m_strUser;
    QString                     m_strPassword;

    QThread *                   m_pSocketThread;
    const QNetworkProxy *       m_pNetworkProxy;

    QNetworkReply::NetworkError m_NetworkError;
    QList<QSslError>            m_sslerrors;
    int                         m_nResponse;
    QString                     m_strResponse;

    int                         m_nOpenTimeOutms;
    int                         m_nReadTimeOutms;

    bool                        m_bReadTimeOut;

    qint64                      m_nBufferSize;

    volatile bool               m_bResetMonitor;
};

#endif // WEBFILE_H

// -------------------------------------------------------------------------------
//  Project:        Qt Webbased File Handling
//
//  File:           weberror.h
//
// (c) 2012 by Oblivion Software/Norbert Schlia
// All rights reserved.
// -------------------------------------------------------------------------------
//
// #pragma once

#ifndef WEBERROR_H
#define WEBERROR_H

class weberror : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("author", "Norbert Schlia")
    Q_CLASSINFO("url", "http://www.oblivion-software.de/")

public:
    explicit weberror(QObject *parent = 0);

    void                        setError(QNetworkReply::NetworkError error);
    QNetworkReply::NetworkError error() const;

    void                        setSslError(QSslError::SslError sslerror);
    QSslError::SslError         errorSsl() const;

    QString                     errorString() const;
    QString                     sslErrorString() const;

signals:

public slots:

protected:
    QNetworkReply::NetworkError m_error;
    QSslError::SslError         m_sslerror;
};

#endif // WEBERROR_H
