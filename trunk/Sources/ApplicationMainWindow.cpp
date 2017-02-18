/* ApplicationMainWindow.cpp  */
/* 2009-01-12                 */
/* Dr. Rainer Sieger          */

#include <QLoggingCategory>

#include "Application.h"
#include "../Forms/AboutDialog/AboutDialog.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Hauptfunktion der Application.
*   @return app.exec()
*/

int main( int argc, char *argv[] )
{
    QLoggingCategory::setFilterRules( "qt.network.ssl.warning=false" ); // disables the QSslSocket warning

    QApplication app( argc, argv );

    #if defined(Q_OS_MAC)
        app.setQuitOnLastWindowClosed( false );
    #endif

    QCoreApplication::setOrganizationName("PANGAEA");
    QCoreApplication::setOrganizationDomain( "pangaea.de" );
    QCoreApplication::setApplicationName( "BSRN_Toolbox" );
    QCoreApplication::setApplicationVersion( _VERSION_ );

    MainWindow mainWin;
    mainWin.show();

    return app.exec();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Beendigung des Programms.
*/

void MainWindow::exitApplication()
{
    savePreferences();

    QCoreApplication::quit();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ausgabe eines Hilfstextes.
*
*   Wenn eine readme.html im Programmverzeichnis exsitiert wird diese geoeffnet. Andernfalls wird
*   ein Dialog ausgegeben der auf eine Internet-Adresse verweist.
*/

void MainWindow::displayHelp()
{
    textViewer = new QTextEdit;
    textViewer->setReadOnly(true);

    QFile file("readme.html");

    if ( file.open( QIODevice::ReadOnly ) == true )
    {
        textViewer->setHtml(file.readAll());
        textViewer->resize(750, 700);
        textViewer->show();
    }
    else
    {
        QString s_ApplicationName = "BSRN_Toolbox";

        QDesktopServices::openUrl( QUrl( tr( "https://wiki.pangaea.de/wiki/%1" ).arg( s_ApplicationName ) ) );
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief aktuelle Version ermitteln.
*
*   Die Nummer der aktuellen Version befindet sich auf dem Server in
*   http://www.panngaea.de/software/ProgramName/ProgramName_Version.txt
*
*   @return Nummer der aktuellen Version.
*/

QString MainWindow::getVersion()
{
    int             n                   = 0;
    int             err                 = _NOERROR_;

    QString         s_Version           = QCoreApplication::applicationName() + "\t" + tr( "unknown" );

    QString         s_Url               = "";
    QString         s_Version_Filename  = "";

    QStringList     sl_Input;

// **********************************************************************************************

    s_Url              = QLatin1String( "https://store.pangaea.de/software" ) + "/" + QCoreApplication::applicationName() + "/" + QCoreApplication::applicationName() + QLatin1String( "_version.txt" );
    s_Version_Filename = getDataLocation() + "/" + QCoreApplication::applicationName() + QLatin1String( "_version.txt" );

    err = downloadFile( findCurl(), s_Url, s_Version_Filename );

    if ( err == _NOERROR_ )
    {
        n = readFile( s_Version_Filename, sl_Input, _SYSTEM_ ); // System encoding

        if ( ( n >= 2) && ( sl_Input.at( 0 ).startsWith( "<!DOCTYPE") == false ) )
            s_Version = sl_Input.at( 1 ).section( " ", 1, 1 );
    }

    return( s_Version );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-28

/*! @brief Download einer Datei mit Curl von einem beliebigen Webserver. Ablage in eine lokale Datei.
*
*   @param s_Curl, Pfad zum Programm curl wird mit findCurl() ermittelt
*   @param s_Url, Link zur Datei auf Webserver
*   @param s_Filename, Pfad und Name der lokalen Datei
*/

int MainWindow::downloadFile( const QString &s_Curl, const QString &s_Url, const QString &s_Filename )
{
    QProcess process;

    removeFile( s_Filename );

    process.start( "\"" + QDir::toNativeSeparators( s_Curl ) + "\" -o \"" + QDir::toNativeSeparators( s_Filename ) + "\" \"" + s_Url + "\"" );
    process.waitForFinished( -1 );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Auswerten der Argumente der Commandline.
*
*   Das Programm kann durch per drag and drop einer oder mehrere Dateien oder eines oder mehrere
*   Ordner auf das Pogrammicon gestartet werden. Die Dateinamen werden vom Betriebssystem in der
*   Kommandozeile uebergeben. Die Methode analysiert jedes Argument. Ist das Argument ein Filename,
*   so wird dieser zur Dateiliste (sl_FilenameList) hinzugefuegt. Ist es ein Ordner, so wird dieser
*   geoeffnet und die darin enthaltenen Dateien werden der Dateiliste hinzugefuegt. Dieser Vorgang
*   ist rekursiv. Ist die Datei eine Zip- oder GZ-Datei, so wird diese entpackt und die darin
*   enthaltenen Dateien werden der Dateiliste hinzugefuegt.
*/

QStringList MainWindow::expandCommandline()
{
    QFileInfo	fi;

    QStringList sl_Filename;

// **********************************************************************************************

    if ( QApplication::arguments().count() > 1 )
    {
        setStatusBar( tr( "Building filelist - Please wait." ) );

        for ( int i=1; i<QApplication::arguments().count(); ++i )
        {
            QApplication::processEvents();

            fi.setFile( QApplication::arguments().at( i ) );

            if ( fi.isFile() == true )
            {
                addToFilenameList( sl_Filename, QApplication::arguments().at( i ) );
            }
            else
            {
                if ( fi.isDir() == true )
                    listDir( sl_Filename, QApplication::arguments().at( i ) );
            }
        }

        sl_Filename.sort();

        if ( sl_Filename.count() > 0 )
            showFilenameList( 0, "", _TXT_, sl_Filename );

        if ( QApplication::arguments().at( 1 ) == "/auto" )
        {
            sl_Filename.clear();
            sl_Filename.append( QApplication::arguments().at( 1 ) );
        }
    }

    return( sl_Filename );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief DocumentDir ermitteln.
*
*   Das DocumentDir ist nicht notwendigerweise das Homeverzeichnis des Benutzers. Es enthaelt in der
*   Regel die Dokumente des Benutzers. Unter Windows ist dies das Verzeichnis "Eigene Dateien".
*
*   @return Pfad des Verzeichnises.
*/

QString MainWindow::getDocumentDir()
{
#if defined(Q_OS_LINUX)
    return( QDir::homePath() );
#endif

#if defined(Q_OS_MAC)
    return( QDir::homePath() );
#endif

#if defined(Q_OS_WIN)
    return( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) );
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermitteln des Pfades fuer die Zwischenablage fuer das Program.
*   @return Name des Pfades. Default Name ist gleich "_ERROR_",
*   wenn Betriebssystem ungleich Windows, Mac OS oder Unix ist.
*/

QString MainWindow::getDataLocation()
{
    QString s_DataLocation = "";

    #if defined(Q_OS_LINUX)
        s_DataLocation = QDir::homePath() + QLatin1String( "/.config/" ) + QCoreApplication::organizationName() + "/" + getApplicationName( true );
    #endif

    #if defined(Q_OS_MAC)
        s_DataLocation = QStandardPaths::writableLocation( QStandardPaths::DataLocation );
    #endif

    #if defined(Q_OS_WIN)
        if ( QCoreApplication::applicationName().toLower().endsWith( "portable" ) )
            s_DataLocation = QCoreApplication::applicationDirPath();
        else
            s_DataLocation = QStandardPaths::writableLocation( QStandardPaths::DataLocation );
    #endif

    QFileInfo fi( s_DataLocation );
    QDir().mkdir( QDir::toNativeSeparators( fi.absolutePath() ) );
    QDir().mkdir( QDir::toNativeSeparators( fi.absoluteFilePath() ) );

    return( fi.absoluteFilePath() );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Eine Datei auswaehlen.
*
*   Mit Hilfe des Dateidialogs des Betriebssystems kann eine Datei zur Bearbeitung
*   ausgewaehlt werden. Die Suche nach Dateien startet im zuletzt geoeffneten Ordner.
*   Wird der Dateidialog abgebrochen, wird _ERROR_ zuruekgegeben. Die ausgwaehlte
*   Datei wird der Dateiliste hinzugefuegt.
*
*   @retval _NOERROR_ Es wurde eine oder mehrere Dateien ausgewaehlt.
*   @retval _ERROR_ Auswahl von Dateien wurde abgebrochen.
*/

int MainWindow::chooseFile()
{
    int			err				= _NOERROR_;

    QString		s_FilenameIn	= "";
    QString	    s_FilenameOut	= "";

    QFileInfo	fi;

// **********************************************************************************************

    setStatusBar( tr( "Select one file to process" ) );

// **********************************************************************************************

    fi.setFile( gs_Path );

    if ( fi.exists() == false )
        gs_Path = getDocumentDir();

// **********************************************************************************************
// Startverzeichnis ermitteln

    switch ( gsl_FilenameList.count() )
    {
    case 1:
        fi.setFile( gsl_FilenameList.at( 0 ) );

        if ( fi.exists() == true )
            s_FilenameIn = gsl_FilenameList.at( 0 );
        else
            s_FilenameIn = gs_Path;
        break;

    default:
            s_FilenameIn = gs_Path;
        break;
    }

// **********************************************************************************************
// Dateiauswahl-Dialog anzeigen

    #if defined(Q_OS_LINUX)
        s_FilenameOut = QFileDialog::getOpenFileName(
            this,
            tr( "Select one file to process" ),
            s_FilenameIn,
            tr( "All files (*.*)\nText (*.txt *.csv *.html *.htm)" ),
            0,
            QFileDialog::DontUseNativeDialog );
    #endif

    #if defined(Q_OS_WIN)
        s_FilenameOut = QFileDialog::getOpenFileName(
            this,
            tr( "Select one file to process" ),
            s_FilenameIn,
            tr( "All files (*.*)\nText (*.txt *.csv *.html *.htm)" ),
            0,
            QFileDialog::DontUseNativeDialog );
    #endif

    #if defined(Q_OS_MAC)
        s_FilenameOut = QFileDialog::getOpenFileName(
            this,
            tr( "Select one file to process" ),
            s_FilenameIn + "/*",
            tr( "All files (*)\nText (*.txt *.csv *.html *.htm)" ),
            0,
            QFileDialog::DontUseNativeDialog );
    #endif

// **********************************************************************************************
// Dateiliste erzeugen

    if ( s_FilenameOut.isEmpty() == false )
    {
        clearFilenameList( gi_ActionNumber, gsl_FilenameList );

        fi.setFile( s_FilenameOut );

        if ( ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        {
            err = _ERROR_;

            setStatusBar( tr( "Wrong format" ), 2 );
        }
        else
        {
            if ( ( fi.exists() == true ) && ( fi.isFile() == true ) )
            {
                gsl_FilenameList.append( s_FilenameOut );

                gs_Path	= fi.absolutePath();
                err		= _NOERROR_;

                setStatusBar( tr( "Ready" ), 2 );
            }
            else
            {
                err = _ERROR_;

                setStatusBar( tr( "File not exists" ), 2 );
            }
        }
    }
    else
    {
        err = _ERROR_;

        setStatusBar( tr( "Choose aborted" ), 2 );
    }

// **********************************************************************************************

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    showFilenameList( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    setNormalCursor();

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Eine oder mehrere Dateien auswaehlen.
*
*   Mit Hilfe des Dateidialogs des Betriebssystems koennen eine oder mehrere Dateien zur Bearbeitung
*   ausgewaehlt werden. Die Suche nach Dateien startet im zuletzt geoeffneten Ordner.
*   Wird der Dateidialog abgebrochen, wird _ERROR_ zuruekgegeben. Die ausgwaehlten
*   Dateien werden der Dateiliste hinzugefuegt. Wurde eine Zip- oder GZ-Datei ausgewaehlt, so wird
*   diese entpackt und die darin enthaltenen Dateien und Ordner werden der Dateiliste hinzugefuegt.
*
*   @retval _NOERROR_ Es wurde eine oder mehrere Dateien ausgewaehlt.
*   @retval _ERROR_ Auswahl von Dateien wurde abgebrochen.
*/

int MainWindow::chooseFiles()
{
    int			i			= 0;
    int			err			= _NOERROR_;

    QString		s_Filename	= "";

    QStringList	sl_d_Filename;

    QFileInfo	fi;

// **********************************************************************************************

    setStatusBar( tr( "Select one or more files to process" ) );

    sl_d_Filename.clear();

// **********************************************************************************************

    fi.setFile( gs_Path );

    if ( fi.exists() == false )
        gs_Path = getDocumentDir();

// **********************************************************************************************
// Startverzeichnis ermitteln

    switch ( gsl_FilenameList.count() )
    {
    case 1:
        fi.setFile( gsl_FilenameList.at( 0 ) );

        if ( fi.exists() == true )
            s_Filename = gsl_FilenameList.at( 0 );
        else
            s_Filename = gs_Path;
        break;

    default:
            s_Filename = gs_Path;
        break;
    }

// **********************************************************************************************
// Dateiauswahl-Dialog anzeigen

#if defined(Q_OS_LINUX)
    sl_d_Filename = QFileDialog::getOpenFileNames(
        this,
        tr( "Select one or more files to process" ),
        s_Filename,
        tr( "All files (*.*)\nText (*.txt *.csv *.html *.htm)" ),
        0,
        QFileDialog::DontUseNativeDialog );
#endif

#if defined(Q_OS_WIN)
    sl_d_Filename = QFileDialog::getOpenFileNames(
        this,
        tr( "Select one or more files to process" ),
        s_Filename,
        tr( "All files (*.*)\nText (*.txt *.csv *.html *.htm)" ),
        0,
        QFileDialog::DontUseNativeDialog );
#endif

#if defined(Q_OS_MAC)
    sl_d_Filename = QFileDialog::getOpenFileNames(
        this,
        tr( "Select one or more files to process" ),
        s_Filename + "/*",
        tr( "All files (*)\nText (*.txt *.csv *.html *.htm)" ),
        0,
        QFileDialog::DontUseNativeDialog );
#endif

// **********************************************************************************************
// Dateiliste erzeugen

    setWaitCursor();

    if ( sl_d_Filename.isEmpty() == false )
    {
        setStatusBar( tr( "Building filelist - Please wait." ) );

        clearFilenameList( gi_ActionNumber, gsl_FilenameList );

        while ( i<sl_d_Filename.count() )
        {
            QApplication::processEvents();

            fi.setFile( sl_d_Filename.at( i ) );

            if ( fi.isFile() == true )
                addToFilenameList( gsl_FilenameList, sl_d_Filename.at( i ) );

            ++i;
        }

        gsl_FilenameList.sort();

        fi.setFile( sl_d_Filename.at( 0 ) );

        gs_Path = fi.absolutePath();
        err	    = _NOERROR_;

        setStatusBar( tr( "Ready" ), 2 );
    }
    else
    {
        err = _ERROR_;

        setStatusBar( tr( "Choose aborted" ), 2 );
    }

// **********************************************************************************************

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    showFilenameList( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    setNormalCursor();

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Verzeichnis auswaehlen.
*
*   Mit Hilfe des Ordnerdialogs des Betriebssystems kann ein Ordner zur Bearbeitung
*   ausgewaehlt werden. Die Suche nach Ordnern startet im zuletzt geoeffneten Ordner.
*   Wird der Ordnerdialog abgebrochen, wird _ERROR_ zuruekgegeben. Die im Ordner enthaltenen
*   Dateien werden der Dateiliste hinzugefuegt. Wurde eine Zip- oder GZ-Datei ausgewaehlt, so wird
*   diese entpackt und die darin enthaltenen Dateien und Ordner werden der Dateiliste hinzugefuegt.
*
*   @retval _NOERROR_ Es wurde ein Folder ausgewaehlt.
*   @retval _ERROR_ Auswahl wurde abgebrochen.
*/

int MainWindow::chooseFolder( const QString &s_Folder )
{
    int		err		= _NOERROR_;

    QString sd_Path = s_Folder;

// **********************************************************************************************

    if ( s_Folder.isEmpty() == true )
    {
        setStatusBar( tr( "Choose a folder to process all files whitin this folder" ) );

        QFileInfo fi( gs_Path );

        if ( fi.exists() == false )
            gs_Path = getDocumentDir();

        #if defined(Q_OS_LINUX)
            sd_Path = QFileDialog::getExistingDirectory( this, tr( "Choose a folder to process all files whitin this folder" ), gs_Path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog );
        #endif

        #if defined(Q_OS_WIN)
            sd_Path = QFileDialog::getExistingDirectory( this, tr( "Choose a folder to process all files whitin this folder" ), gs_Path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog );
        #endif

        #if defined(Q_OS_MAC)
            sd_Path = QFileDialog::getExistingDirectory( this, tr( "Choose a folder to process all files whitin this folder" ), gs_Path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog );
        #endif
    }

// **********************************************************************************************
// Dateiliste erzeugen

    setWaitCursor();

    if ( sd_Path.isEmpty() == false )
    {
        setStatusBar( tr( "Building filelist - Please wait." ) );

        clearFilenameList( gi_ActionNumber, gsl_FilenameList );

        gs_Path = sd_Path;

        listDir( gsl_FilenameList, gs_Path );

        err				= _NOERROR_;

        setStatusBar( tr( "Ready" ), 2 );
    }
    else
    {
        err = _ERROR_;

        setStatusBar( tr( "Choose aborted" ), 2 );
    }

// **********************************************************************************************

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    showFilenameList( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    setNormalCursor();

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Datei sichern.
    Wird zur Zeit nicht benoetigt.
*/

int MainWindow::saveFile()
{
    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Datei sichern als.
    Wird zur Zeit nicht benoetigt.
*/

int MainWindow::saveFileAs()
{
    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Sichern der Einstellungen wenn Programm beendet wird. */

void MainWindow::closeEvent( QCloseEvent *event )
{
    savePreferences();
    event->accept();

#if defined(Q_OS_MAC) // Clicking the dock icon to show the app window on Mac (http://qt-project.org/forums/viewthread/720/)
    if ( event->spontaneous() )
    {
        QStringList args;

        args << "-e";
        args << "tell application \"System Events\"";
        args << "-e";
        args << "set visible of process \"" + QFileInfo( QApplication::applicationFilePath() ).baseName() + "\" to false";
        args << "-e";
        args << "end tell";

        QProcess::execute( "osascript", args);

        event->ignore();
        return;
    }
    else
    {
        QApplication::quit();
    }
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Erstellen der Dateiliste.
*
*   Der uebergebene Dateiname wird der Dateiliste hinzugefuegt. Ist die Datei eine Zip- oder
*   GZ-Datei, so wird diese Ausgepackt und die darin enthaltenen Dateien und Ordner werden der
*   Dateiliste hinzugefuegt.
*
*   @param  s_Filename Name der Datei, die der Liste hinzugefuegt werden soll.
*   @param  b_decompressFile wenn b_decompressFile gleich true ist, werden Zip-Dateien entpackt.
*
*   @retval _NOERROR_ Evtl. vorhandene Zip-Dateie wurde erfolgreich geloescht.
*   @retval _ERROR_ Das Loeschen einer Zip-Datei hat nicht funktioniert.
*/

int MainWindow::addToFilenameList( QStringList &sl_FilenameList, const QString &s_Filename, const bool b_showAllFiles, const bool b_decompressFile )
{
    int	err	= _NOERROR_;

// **********************************************************************************************

    QFileInfo fi( s_Filename );

    if ( ( b_showAllFiles == true ) || ( ( fi.suffix().toLower() != "zip" ) && ( fi.suffix().toLower() != "gz" ) ) )
    {
        sl_FilenameList.append( QDir::toNativeSeparators( s_Filename ) );
    }
    else
    {
        if ( b_decompressFile == true )
        {
            QString s_Program = "Unzip not found";

            if ( fi.suffix().toLower() == "zip" )
            {
                s_Program = findUnzip( _ZIP_ );

                if ( s_Program != "Unzip not found" )
                    err = decompressFile( s_Filename, false, _ZIP_, s_Program );
            }

            if ( fi.suffix().toLower() == "gz" )
            {
                s_Program = findUnzip( _GZIP_ );

                if ( s_Program != "Unzip not found" )
                    err = decompressFile( s_Filename, false, _GZIP_, s_Program );
            }

            if ( err == _NOERROR_ )
            {
                listDir( sl_FilenameList, fi.absolutePath() + "/" + fi.baseName(), b_showAllFiles, b_decompressFile );
            }
            else
            {
                setNormalCursor();

                QString s_Message = tr( "Cannot delete zip file." );
                QMessageBox::warning( this, getApplicationName( true ), s_Message );

                setWaitCursor();
            }
        }
    }

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
/*! @brief Loescht alle Dateien in einem Verzeichnis.
*
*   @param s_Dir Name des Verzeichnises, das geleert werden soll.
*/

int MainWindow::emptyDir( const QString &s_Dir )
{
    QStringList	sl_FilenameList;

// **********************************************************************************************

    QFileInfo di( s_Dir );

    if ( di.isDir() == true )
    {
        QDir d( di.filePath() );
        sl_FilenameList = d.entryList();
        QFile fi( sl_FilenameList.at( 0 ) );

        for ( int i=0; i<sl_FilenameList.count(); ++i )
        {
            QApplication::processEvents();

            if ( sl_FilenameList.at( i ).startsWith( "." ) == false )
            {
                fi.setFileName( di.filePath() + "/" + sl_FilenameList.at( i ) );

                if ( fi.exists() == true )
                    fi.remove();
            }
        }
    }

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die Dateien in einem Verzeichnis.
*
*   @param s_Dir Name des Verzeichnises, das analysiert werden soll.
*   @param  b_decompressFile wenn b_decompressFile gleich true ist, werden Zip-Dateien entpackt.
*/

void MainWindow::listDir( QStringList &sl_FilenameList, const QString &s_Dir, const bool b_showAllFiles, const bool b_decompressFile )
{
    int			i				= 0;

    QString		s_Filename		= "";
    QString		s_Directory		= "";

    QStringList	sl_d_Filename;

// **********************************************************************************************

    QFileInfo fi( s_Dir );

    if ( fi.isDir() == true )
    {
        s_Directory = fi.filePath();

        QDir d( s_Directory );

        if ( s_Directory.endsWith( "/" ) == false )
            s_Directory.append( "/" );

        sl_d_Filename = d.entryList();

        while ( i<sl_d_Filename.count() )
        {
            QApplication::processEvents();

            if ( sl_d_Filename.at( i ).startsWith( "." ) == false )
            {
                s_Filename = s_Directory + sl_d_Filename.at( i );

                fi.setFile( s_Filename );

                if ( fi.isFile() == true )
                {
                    addToFilenameList( sl_FilenameList, s_Filename, b_showAllFiles, b_decompressFile );
                }
                else
                {
                    if ( fi.isDir() == true )
                        listDir( sl_FilenameList, s_Filename, b_showAllFiles, b_decompressFile );
                }
            }

            ++i;
        }
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Liest Datei.
*
*   @param  s_FilenameIn Dateiname.
*   @param  sl_Input enthaelt die gelesenen Zeilen der Datei.
*   @param  i_Codec bestimmt den Zeichensatz.
*   @param  i_NumOfFiles Anzahl der Dateien. Bestimmt das Verhalten des StatusBars.
*   @param  i_Bytes Anzahl der zulesenden Bytes (= Zeichen).

*   @retval Anzahl der gelesenen Zeilen.
*/

int MainWindow::readFile( const QString &s_FilenameIn, QStringList &sl_Input, const int i_CodecInput, const int i_NumOfFiles, const qint64 i_Bytes )
{
    QByteArray ba;

    sl_Input.clear();

// **********************************************************************************************
// read data

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Read data..." ), 4 );

    QFile fpdb( s_FilenameIn );

    if ( fpdb.open( QIODevice::ReadOnly ) == false )
        return( -10 );

    if ( i_Bytes == 0 )
        ba = fpdb.readAll();

    if ( i_Bytes > 0 )
        ba = fpdb.read( i_Bytes );

    if ( i_Bytes < 0 )
        ba = qUncompress( fpdb.readAll() );

    fpdb.close();

    if ( incProgress( i_NumOfFiles, 1 ) == _APPBREAK_ )
        return( 0 );

// **********************************************************************************************
// replace End-Of-Line character

    ba.replace( "\r\r\n", "\n" ); // Windows -> Unix
    ba.replace( "\r\n", "\n" ); // Windows -> Unix
    ba.replace( "\r", "\n" ); // MacOS -> Unix

    if ( incProgress( i_NumOfFiles, 2 ) == _APPBREAK_ )
        return( 0 );

// **********************************************************************************************
// split

    if ( ba.size() < 500000000 ) // 500 MB; not tested
    {
        switch ( i_CodecInput )
        {
        case _SYSTEM_: // nothing
            sl_Input = QString( ba ).split( "\n" );
            break;

        case _APPLEROMAN_: // Apple Roman
            sl_Input = QTextCodec::codecForName( "Apple Roman" )->toUnicode( ba ).split( "\n" );
            break;

        case _LATIN1_: // Latin-1 = ISO 8859-1
            sl_Input = QTextCodec::codecForName( "ISO 8859-1" )->toUnicode( ba ).split( "\n" );
            break;

        default: // UTF-8
            sl_Input = QTextCodec::codecForName( "UTF-8" )->toUnicode( ba ).split( "\n" );
            break;
        }
    }
    else
    {
        sl_Input.append( tr( "File to big" ) );
    }

    if ( incProgress( i_NumOfFiles, 3 ) == _APPBREAK_ )
        return( 0 );

// **********************************************************************************************
// remove empty lines at the end of the file

    while ( ( sl_Input.count() > 0 ) && ( sl_Input.at( sl_Input.count()-1 ).isEmpty() == true ) )
        sl_Input.removeLast();

// **********************************************************************************************

    resetProgress( i_NumOfFiles );

    return( sl_Input.count() );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-05-28

/*! @brief berechnet den Hash-Wert einer Datei.
*
*   @param  s_FilenameIn Dateiname.
*
*   @retval Hash.
*/

QString MainWindow::getFileChecksum( const QString &s_FilenameIn )
{
    QFile              f_File( s_FilenameIn );
    QCryptographicHash hash( QCryptographicHash::Sha256 );

// **********************************************************************************************

    if ( f_File.open( QFile::ReadOnly ) == true )
    {
        hash.addData( &f_File );
        f_File.close();
        return( hash.result().toHex().trimmed() );
    }

    return( "" );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2009-12-03

/*! @brief bestimmt die Groesse einer Datei und gibt eine Groessenklasse zurueck.
*
*   @param  s_FilenameIn Dateiname.
*   @param  i_NumOfFiles Anzahl der Dateien.
*   @param  lowerLimit untere Schranke in KB.
*   @param  upperLimit obere Schranke in KB.

*   @retval Groessenklasse.
*/

int MainWindow::calcFileSizeClass( const QString &s_Filename, const int i_NumOfFiles, const int lowerLimit, const int upperLimit )
{
    if ( i_NumOfFiles > 1 )
        return( i_NumOfFiles );

    QFileInfo fi( s_Filename );

    if ( fi.size() < lowerLimit*1024 )
        return( 0 );

    if ( fi.size() > upperLimit*1024 )
        return( 1 );

    return( 2 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-10-25

/*! @brief Komprimieren eines Verzeichnisses mit zip, tar+gz oder 7-Zip.
*
*   @param  s_Folder Name des zu zippenden Verzeichnisses.
*   @param  mode Zip Mode ( _ZIP_ oder _GZIP_ )
*   @param  s_Program Zip-Programm (zip, gz oder 7-Zip)
*/

void MainWindow::compressFolder( const QString &s_Folder, const int mode, const QString &s_Program )
{
    QString     s_arg = "";

    QStringList sl_Message;

    QProcess    process;

// **********************************************************************************************

    sl_Message.clear();

    QFileInfo fdir( s_Folder );

    if ( fdir.exists() == true )
    {
        if ( mode == _ZIP_ )
        {
            QFileInfo farchive( fdir.fileName() + ".zip" );

            QDir::setCurrent( fdir.absolutePath() );

            removeFile( farchive.fileName() );

            showMessage( tr( "Compressing " ) + QDir::toNativeSeparators( farchive.fileName() ) + tr( " ..." ), sl_Message );

            #if defined(Q_OS_LINUX)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -r \"" + farchive.fileName() + "\"" + " \"" + fdir.fileName() + "\" -x *.DS_Store";
            #endif

            #if defined(Q_OS_MAC)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -r \"" + farchive.fileName() + "\"" + " \"" + fdir.fileName() + "\" -x *.DS_Store";
            #endif

            #if defined(Q_OS_WIN)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" a \"" +  QDir::toNativeSeparators( farchive.fileName() ) + "\"" + " \"" +  QDir::toNativeSeparators( fdir.fileName() + "/" ) + "\"";
            #endif

            process.start( s_arg );
            process.waitForFinished( -1 );
        }

        if ( mode == _GZIP_ )
        {
            QFileInfo farchive( fdir.fileName() );

            QDir::setCurrent( fdir.absolutePath() );

            removeFile( farchive.fileName() + ".tar.gz" );

            showMessage( tr( "Compressing " ) + QDir::toNativeSeparators( farchive.fileName() ) + tr( " ..." ), sl_Message );

            #if defined(Q_OS_LINUX)
                s_arg.append( "tar --exclude=.DS_Store -czf " );
                s_arg.append( "\"" + farchive.fileName() + ".tar.gz" + "\"" + " " );
                s_arg.append( "\"" + fdir.fileName() + "\"" );
                process.start( s_arg );
                process.waitForFinished( -1 );
            #endif

            #if defined(Q_OS_MAC)
                s_arg.append( "tar --disable-copyfile --exclude=.DS_Store -czf " );
                s_arg.append( "\"" + farchive.fileName() + ".tar.gz" + "\"" + " " );
                s_arg.append( "\"" + fdir.fileName() + "\"" );
                process.start( s_arg );
                process.waitForFinished( -1 );
            #endif

            #if defined(Q_OS_WIN)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" a \"" + farchive.fileName() + ".tar" + "\"" + " \"" + fdir.fileName() + "\\" + "\"";
                process.start( s_arg );
                process.waitForFinished( -1 );

                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" a \"" + farchive.fileName() + ".tar.gz" + "\"" + " \"" + farchive.fileName() + ".tar" + "\"";
                process.start( s_arg );
                process.waitForFinished( -1 );

                removeFile( farchive.fileName() + ".tar" );
            #endif
        }
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-10-25

/*! @brief Komprimieren von Dateien mit zip, gz oder 7-Zip.
*
*   @param  s_Filename Dateiname der zu zippenden Datei.
*   @param  mode Zip Mode ( _ZIP_ oder _GZIP_ )
*   @param  s_Program Zip-Programm (zip, gz oder 7-Zip)
*/

void MainWindow::compressFile( const QString &s_Filename, const int mode, const QString &s_Program )
{
    QString     s_arg = "";

    QStringList sl_Message;

    QProcess    process;

// **********************************************************************************************

    sl_Message.clear();

    QFileInfo fi( s_Filename );

    if ( fi.exists() == true )
    {
        showMessage( tr( "Compressing " ) + QDir::toNativeSeparators( s_Filename ) + tr( " ..." ), sl_Message );

// **********************************************************************************************
// zip -j archive.zip file.txt , -j = junk path

        if ( mode == _ZIP_ )
        {
            #if defined(Q_OS_LINUX)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -j \"" + QDir::toNativeSeparators( fi.absolutePath() + "/" + fi.completeBaseName() + ".zip" ) + "\"" + " \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif

            #if defined(Q_OS_MAC)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -j \"" + QDir::toNativeSeparators( fi.absolutePath() + "/" + fi.completeBaseName() + ".zip" ) + "\"" + " \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif

            #if defined(Q_OS_WIN)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" a \"" + QDir::toNativeSeparators( fi.absolutePath() + "/" + fi.completeBaseName() + ".zip" ) + "\"" + " \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif
        }

// **********************************************************************************************
// gz file.txt

        if ( mode == _GZIP_ )
        {
            #if defined(Q_OS_LINUX)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif

            #if defined(Q_OS_MAC)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif

            #if defined(Q_OS_WIN)
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" a \"" + QDir::toNativeSeparators( fi.absolutePath() + "/" + fi.completeBaseName() + ".gz" ) + "\"" + " \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
            #endif
        }

// **********************************************************************************************

        process.start( s_arg );
        process.waitForFinished( -1 );
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-10-25

/*! @brief Auspacken von Zip und GZip Dateien.
*
*   @param  s_Filename Dateiname der Zip-Datei.
*   @param  b_delZipFile wenn b_delZipFile gleich true ist, wird die Zip-Datei nach dem Auspacken geloescht.

*   @retval _NOERROR_ Loeschen der Zip-Datei war erfolgreich.
*   @retval _ERROR_ Zip-Datei konnte nicht geloescht werden.
*/

int MainWindow::decompressFile( const QString &s_Filename, const bool b_delZipFile, const int mode, const QString &s_Program )
{
    int         err   = _NOERROR_;

    QString     s_arg = "";

    QStringList sl_Message;

    QProcess    process;

// **********************************************************************************************

    if ( mode < 1 ) return( -999 );

    sl_Message.clear();

    QFileInfo fi( s_Filename );

    if ( fi.exists() == true )
    {
        showMessage( tr( "Decompressing " ) + QDir::toNativeSeparators( s_Filename ) + tr( " ..." ), sl_Message );

        #if defined(Q_OS_LINUX)
            switch ( mode )
            {
            case _ZIP_:
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -o \"" + QDir::toNativeSeparators( s_Filename ) + "\" -d \"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";
                break;

            case _GZIP_:
                if ( fi.completeSuffix().toLower() == "tar.gz" )
                    s_arg = "tar -xzf \"" + QDir::toNativeSeparators( s_Filename ) + "\" -C \"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";

                if ( ( fi.completeSuffix().toLower() != "tar.gz" ) && ( fi.suffix().toLower() == "gz" ) )
                    s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -d \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
                break;

            default:
                break;
            }

            process.start( s_arg );
            process.waitForFinished( -1 );
        #endif

        #if defined(Q_OS_MAC)
            switch ( mode )
            {
            case _ZIP_:
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -o \"" + QDir::toNativeSeparators( s_Filename ) + "\" -d \"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";
                break;

            case _GZIP_:
                if ( fi.completeSuffix().toLower() == "tar.gz" )
                    s_arg = "tar -xzf \"" + QDir::toNativeSeparators( s_Filename ) + "\" -C \"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";

                if ( ( fi.completeSuffix().toLower() != "tar.gz" ) && ( fi.suffix().toLower() == "gz" ) )
                    s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" -d \"" + QDir::toNativeSeparators( s_Filename ) + "\"";
                break;

            default:
                break;
            }

            process.start( s_arg );
            process.waitForFinished( -1 );
        #endif

        #if defined(Q_OS_WIN)
            s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" x \"" + QDir::toNativeSeparators( s_Filename ) + "\" -o\"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";
            process.start( s_arg );
            process.waitForFinished( -1 );

            if ( fi.completeSuffix().toLower() == "tar.gz" )
            {
                s_arg = "\"" + QDir::toNativeSeparators( s_Program ) + "\" x \"" + QDir::toNativeSeparators( fi.absolutePath() + "/" + fi.baseName() + ".tar" ) + "\" -o\"" + QDir::toNativeSeparators( fi.absolutePath() ) + "\"";
                process.start( s_arg );
                process.waitForFinished( -1 );

                err = removeFile( fi.absolutePath() + "/" + fi.baseName() + ".tar" );
            }
        #endif
    }

    if ( b_delZipFile == true )
        err = removeFile( s_Filename );

    return( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Testet ob die erste Datei innerhalb einer Dateiliste existiert.
*
*   @retval true Erste Datei auf der Liste existiert.
*   @retval false Erste Datei auf der Liste existiert nicht. Evtl. aufgerufene Dateiauswahl wurde abgebrochen.
*/

bool MainWindow::existsFirstFile( const int i_ActionNumber, const QString &s_FilenameFormat, const int i_Extension, QStringList &sl_FilenameList )
{
    QString s_FilenameIn    = "";
    QString s_FilenameOut   = "";

// **********************************************************************************************

    if ( sl_FilenameList.count() > 0 )
    {
        if ( buildFilename( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList.at( 0 ), s_FilenameIn, s_FilenameOut ) == false )
        {
            if ( chooseFiles() == _ERROR_ ) // File not exist ==> choose files
                return( false ); // Error, choose files canceled
        }
    }
    else
    {
        if ( chooseFiles() == _ERROR_ ) // File not exist ==> choose files
            return( false ); // Error, choose files canceled
    }

    if ( containsBinaryFile( gsl_FilenameList ) == true )
        return( false );

    return( true );  // File exists
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Dateiname wird gemaess der Formatanweisung erstellt.
*
*   @param  ActionNumber Die ActionNumber bestimmt den Dateinamen.
*   @param  s_FilenameFormat Das s_FilenameFormat erzeugt zusammen mit ActionNumber und altem Dateinamen den neuen Dateinamen.
*   @param  s_FilenameIn Der Name der Eingangsdatei.
*   @param  s_FilenameOut Der Name der Ausgangsdatei.
*
*   @retval true Dateiname fuer Ausgabedatei erstellt. Inputdatei existiert.
*   @retval false Inputdatei existiert nicht.
*/

bool MainWindow::buildFilename( const int i_ActionNumber, const QString &s_FilenameFormat, const int i_Extension, const QString &s_Filename, QString &s_FilenameIn, QString &s_FilenameOut )
{
    QFileInfo fi( s_Filename );

    QString   s_CompleteBaseName = "";

// **********************************************************************************************

    s_FilenameIn       = fi.absoluteFilePath();
    s_CompleteBaseName = fi.completeBaseName();

    if ( i_ActionNumber > 1 )
    {
        s_FilenameIn = fi.absolutePath() + "/" + s_FilenameFormat + setExtension( i_Extension ) ;

        s_FilenameIn.replace( "//", "/" );
        s_FilenameIn.replace( "%a", QString( tr( "%1" ) ).arg( i_ActionNumber-1 ) );
        s_FilenameIn.replace( "%N", s_CompleteBaseName );
    }

    fi.setFile( s_FilenameIn );

    if ( fi.exists() == true )
    {
        s_FilenameOut = fi.absolutePath() + "/" + s_FilenameFormat + setExtension( i_Extension );

        s_FilenameOut.replace( "//", "/" );
        s_FilenameOut.replace( "%a", QString( tr( "%1" ) ).arg( i_ActionNumber ) );
        s_FilenameOut.replace( "%N", s_CompleteBaseName );

        return( true ); // InFile exists, OutFile has been builded
    }
    else
    {
        s_FilenameIn  = "";
        s_FilenameOut = "";
    }

    return( false ); // File not exists
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Fenstertitel setzen.
*
*   Der Fenstertitel wird als Parameter uebergeben oder aus der Dateiliste erzeugt.
*
*   @param s_WindowTitle Der Fenstertitel
*/

void MainWindow::setWTitle(const int i_ActionNumber, const QString &s_FilenameFormat, const int i_Extension, const QStringList &sl_FilenameList )
{
    QString   s_FilenameIn    = "";
    QString   s_FilenameOut   = "";

    QFileInfo fi;

// **********************************************************************************************

    newWindow();

    switch ( sl_FilenameList.count() )
    {
    case 0:
        setWindowTitle( tr ( "%1 - %2" ).arg( getApplicationName( true ) ).arg( "[no file selected]" ) );
        break;

    case 1:
        buildFilename( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList.at( 0 ), s_FilenameIn, s_FilenameOut );

        fi.setFile( s_FilenameIn );

        if ( fi.exists() == true )
            setWindowTitle( tr( "%1 - [%2]" ).arg( getApplicationName( true ) ).arg( QDir::toNativeSeparators( s_FilenameIn ) ) );
        else
            setWindowTitle( tr ( "%1 - %2" ).arg( getApplicationName( true ) ).arg( "[no file selected]" ) );
        break;

    default:
        fi.setFile( *sl_FilenameList.begin() );
        setWindowTitle( tr( "%1 - [%2 files selected in %3]" ).arg( getApplicationName( true ) ).arg( sl_FilenameList.count() ).arg( QDir::toNativeSeparators( fi.absolutePath() ) ) );
        break;
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Beendigt Tool. */

void MainWindow::endTool( const int err, const int stopProgress, int &i_ActionNumber, const QString &s_FilenameFormat, const int i_Extension, QStringList &sl_FilenameList, const QString &s_doneMessage, const QString &s_canceledMessage, const bool b_clearFilenameList, const bool b_incActionNumber )
{
    if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
    {
        if ( b_incActionNumber == true )
            ++i_ActionNumber;

        if ( b_clearFilenameList == true )
            clearFilenameList( i_ActionNumber, sl_FilenameList );

        showFilenameList( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList );
        setStatusBar( s_doneMessage, 2 );
    }
    else
    {
        if ( b_clearFilenameList == true )
            clearFilenameList( i_ActionNumber, sl_FilenameList );

        showFilenameList( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList );
        setStatusBar( s_canceledMessage, 2 );
    }

    setWTitle( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Liste von Nachrichten anzeigen. */

void MainWindow::showMessage( const QString &s_Message, QStringList &sl_Message )
{
    appendItem( sl_Message, s_Message );
    showList( sl_Message );
    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Nachrichtenliste loeschen. */

void MainWindow::clearMessage( void )
{
    QStringList sl_Message;

    sl_Message.clear();
    showList( sl_Message );
    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Namen des Programmes ermitteln. Evtl. "_" im Namen werden durch Leerzeichen ersetzt.
*
*   @return Name des Programmes
*/

QString MainWindow::getApplicationName( const bool b_replaceUnderline )
{
    QFileInfo fi( QApplication::applicationFilePath() );
    QString s_ApplicationName = fi.baseName();

    if ( b_replaceUnderline == true )
      s_ApplicationName.replace( "_", " " );

    return( s_ApplicationName );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief StatusBar erstellen */

void MainWindow::createStatusBar( const bool b_showProgressBar )
{
    StatusMessage = new QLabel();
    StatusMessage->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    StatusMessage->setIndent( 5 );

    ProgressBar = new QProgressBar();

#if defined(Q_OS_LINUX)
    ProgressBar->setTextVisible( false );
#endif

#if defined(Q_OS_MAC)
    ProgressBar->setTextVisible( true ); // if false, the ProgressBar will not be shown (maybe a Qt bug?)
#endif

#if defined(Q_OS_WIN)
    ProgressBar->setTextVisible( false );
#endif

    if ( b_showProgressBar == true )
    {
        statusBar()->addWidget( StatusMessage, 2 );
        statusBar()->addWidget( ProgressBar, 1 );
    }
    else
    {
        statusBar()->addWidget( StatusMessage, 2 );
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Nachricht im StatusBar anzeigen.
*
*   @param s_Message anzuzeigende Nachricht.
*   @param i_seconds die Nachricht wird i_seconds lang angezeigt und danach geloescht.
*/

void MainWindow::setStatusBar( const QString &s_Message, const int i_seconds )
{
    StatusMessage->setText( s_Message + tr( " " ) );

    if ( i_seconds > 0 )
        QTimer::singleShot( i_seconds*1000, this, SLOT( clearStatusMessage() ) );

    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Filename im StatusBar anzeigen.
*
*   @param s_Filename anzuzeigender Dateiname.
*   @param s_Message anzuzeigende Nachricht.
*   @param i_seconds die Nachricht wird i_seconds lang angezeigt und danach geloescht.
*/

void MainWindow::setStatusBarFileInProgress( const QString &s_Filename, const QString &s_Message, const int i_seconds )
{
    QFileInfo fi( s_Filename );
    setStatusBar( tr( "File in progress: " ) + fi.fileName() + s_Message, i_seconds );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Nachricht im StatusBar loeschen. */

void MainWindow::clearStatusMessage()
{
    StatusMessage->clear();
    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Item an Liste anhaengen.
*
*   @param sl_List Liste.
*   @param s_Item wird an sl_List angehaengt.
*   @param SS SuchString wird in s_Item gesucht.
*   @param RS ReplaceString ersetzt den SuchString in s_Item.
*/

void MainWindow::appendItem( QStringList &sl_List, const QString &s_Item, const QString &SS, const QString &RS )
{
    if ( SS != RS )
    {
        QString s_dItem = s_Item;
        s_dItem.replace( SS, RS );
        sl_List.append( s_dItem );
    }
    else
    {
        sl_List.append( s_Item );
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Liste anzeigen.
*
*   @param sl_List Liste, die angezeigt werden soll.
*/

void MainWindow::showList( const QStringList &sl_List )
{
    Model->setStringList( sl_List );
    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Liste anzeigen.
*
*   @param sl_FilenameList Liste der Dateinamen, die angezeigt werden soll.
*/

void MainWindow::showFilenameList( const int i_ActionNumber, const QString &s_FilenameFormat, const int i_Extension, const QStringList &sl_FilenameList )
{
    QString     s_FilenameIn    = "";
    QString     s_FilenameOut   = "";

    QStringList sl_List;

// **********************************************************************************************

    for ( int i=0; i<sl_FilenameList.count(); i++)
    {
        buildFilename( i_ActionNumber, s_FilenameFormat, i_Extension, sl_FilenameList.at( i ), s_FilenameIn, s_FilenameOut );
        sl_List.append( QDir::toNativeSeparators( s_FilenameIn ) );
    }

    showList( sl_List );

    enableMenuItems( sl_List );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Leert eine Dateiliste und setzt ActionNumer auf 1. */

void MainWindow::clearFilenameList( int &i_ActionNumber, QStringList &sl_FilenameList )
{
    sl_FilenameList.clear();
    i_ActionNumber = 1;
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

void MainWindow::dragEnterEvent( QDragEnterEvent *event )
{
    if ( event->mimeData()->hasFormat( "text/uri-list" ) == true )
        event->acceptProposedAction();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

void MainWindow::dropEvent( QDropEvent *event )
{
    QFileInfo fi;

// **********************************************************************************************

    setWaitCursor();

    QList<QUrl> sl_urls = event->mimeData()->urls();

    if ( sl_urls.isEmpty() == true )
        return;

    clearFilenameList( gi_ActionNumber, gsl_FilenameList );

    for ( int i=0; i<sl_urls.count(); ++i )
    {
        QApplication::processEvents();

        fi.setFile( sl_urls[i].toLocalFile() );

        if ( i == 0 )
        {
            if ( fi.isFile() == true )
            {
                gs_Path = fi.absolutePath();
            }
            else
            {
                if ( fi.isDir() == true )
                    gs_Path = fi.absoluteFilePath();
            }
        }

        if ( fi.isFile() == true )
        {
            addToFilenameList( gsl_FilenameList, fi.filePath() );
        }
        else
        {
            if ( fi.isDir() == true )
                listDir( gsl_FilenameList, fi.filePath() );
        }
    }

// **********************************************************************************************

    gsl_FilenameList.sort();

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    showFilenameList( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );
    setNormalCursor();

    finishedBuildFilelist( true );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2014-08-17

/*! @brief Programm About anzeigen. */

void MainWindow::about()
{
    QString s_newVersionText = "";

    AboutDialog dialog( this );

    dialog.setWindowTitle( tr( "About %1").arg( getApplicationName( true ) ) );
    dialog.ProgramName->setText( getApplicationName( true ) );
    dialog.Version->setText( tr( "V%1" ).arg( _VERSION_ ) );
    dialog.Warranty->setText( tr( "%1 comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. See the License.txt file for details." ).arg( getApplicationName( true ) ) );

    if ( _VERSION_ != gs_Version )
    {
        if ( _PROGRAMDOI_.isEmpty() == true )
        {
            if ( gs_Version == "unknown" )
                s_newVersionText = QString( tr( "Cannot determine the most recent version of %1.<br>Please look at <a href=\"%2\">%3</a> for the<br>latest version." ).arg( getApplicationName( true ) ).arg( _PROGRAMREADME_ ).arg( _PROGRAMREADME_ ) );
            else
                s_newVersionText = QString( tr( "This version of %1 is outdated. Please look at<br><a href=\"%2\">%3</a> for the latest version." ).arg( getApplicationName( true ) ).arg( _PROGRAMREADME_ ).arg( _PROGRAMREADME_ ) );
        }
        else
        {
            if ( gs_Version == "unknown" )
                s_newVersionText = QString( tr( "Cannot determine the most recent version of %1.<br>Please look at <a href=\"https://doi.pangaea.de/%2?format=html\">%3</a> for the<br>latest version." ).arg( getApplicationName( true ) ).arg( _PROGRAMDOI_.mid( 4 ) ).arg( _PROGRAMDOI_ ) );
            else
                s_newVersionText = QString( tr( "This version of %1 is outdated. Please look at<br><a href=\"https://doi.pangaea.de/%2?format=html\">%3</a> for the latest version." ).arg( getApplicationName( true ) ).arg( _PROGRAMDOI_.mid( 4 ) ).arg( _PROGRAMDOI_ ) );
        }
    }
    else
    {
        s_newVersionText = QString( tr( "Program is up-to-date." ) );
    }

    dialog.newVersion->setText( s_newVersionText );
    dialog.newVersion->setTextFormat( Qt::RichText );
    dialog.newVersion->setOpenExternalLinks( true );

    dialog.Contact->setText( "Contact: Rainer Sieger (<a href=\"mailto:rsieger@pangaea.de\">rsieger@pangaea.de</a>)" );
    dialog.Contact->setTextFormat( Qt::RichText );
    dialog.Contact->setOpenExternalLinks( true );

    dialog.resize( dialog.sizeHint() );
    dialog.show();
    dialog.exec();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog auf Dateiebene erzeugen.
*
*   @param i_NumOfFiles Anzahl der Dateien.
*   @param s_FilenameIn Name der Datei, die verarbeitet werden soll.
*   @param s_MessageText Name der Verarbeitungsfunktion.
*/

void MainWindow::initFileProgress( const int i_NumOfFiles, const QString &s_FilenameIn, const QString &s_MessageText )
{
    if ( i_NumOfFiles > 1 )
    {
        FileProgressDialog = new QProgressDialog( s_MessageText, "Abort", 0, i_NumOfFiles, this );

        FileProgressDialog->setWindowModality( Qt::WindowModal );
        FileProgressDialog->setMinimumDuration( 2 );
        FileProgressDialog->setMinimumWidth( 300 );
        FileProgressDialog->show();
        FileProgressDialog->setValue( 0 );

        setStatusBarFileInProgress( s_FilenameIn );

        QApplication::processEvents();
    }

    setWaitCursor();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog auf Dateiebene hochzaehlen
*
*   @param i_NumOfFiles Anzahl der Dateien.
*   @param i_FileNumber Nummer der aktuellen Datei.
*
*   @retval _NOERROR_ Progressdialog wurde erfolgreich hochgezaehlt.
*   @retval _APPBREAK_ Laufender Prozess wurde abgebrochen.
*/

int MainWindow::incFileProgress( const int i_NumOfFiles, const int i_FileNumber )
{
    if ( i_NumOfFiles > 1 )
    {
        FileProgressDialog->setValue( i_FileNumber );

        if ( FileProgressDialog->wasCanceled() == true )
            return( _APPBREAK_ );
    }

    QApplication::processEvents();

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog auf Dateiebene zuruecksetzen.
*
*   Der Progressdialog wird nur zurueckgesetzt, wenn die Anzahl der Dateien groesser 1 ist und
*   der Prozess vorher nicht abgebrochen wurde (stopProgress = 0).
*
*   @param i_NumOfFiles Anzahl der Dateien.
*/

void MainWindow::resetFileProgress( const int i_NumOfFiles )
{
    if ( i_NumOfFiles > 1 )
    {
        FileProgressDialog->hide();
        FileProgressDialog->reset();
    }

    clearStatusMessage();

    setNormalCursor();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog zurueksetzen.
*
*   @param i_totalNumberOfSteps Maximale Anzahl der Schritte.
*/

void MainWindow::initProgress( const int i_totalNumberOfSteps )
{
    ProgressBar->reset();
    ProgressBar->setRange( 0, i_totalNumberOfSteps );
    ProgressBar->setValue( 0 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog erzeugen.
*
*   @param i_NumOfFiles Anzahl der Dateien.
*   @param s_FilenameIn Name der Datei, die verarbeitet werden soll.
*   @param s_MessageText Name der Verarbeitungsfunktion.
*   @param i_totalNumberOfSteps Maximale Anzahl der Schritte.
*/

void MainWindow::initProgress( const int i_NumOfFiles, const QString &s_FilenameIn, const QString &s_MessageText, const int i_totalNumberOfSteps )
{
    switch( i_NumOfFiles )
    {
    case 0:
        break;

    case 1:
        ProgressDialog = new QProgressDialog( s_MessageText, "Abort", 0, i_totalNumberOfSteps, this );
        ProgressDialog->setWindowModality( Qt::WindowModal );
        ProgressDialog->setMinimumDuration( 2 );
        ProgressDialog->setMinimumWidth( 300 );
        ProgressDialog->show();
        setStatusBarFileInProgress( s_FilenameIn );
        break;

    default:
        statusBar()->clearMessage();
        ProgressBar->reset();
        ProgressBar->setRange( 0, i_totalNumberOfSteps );
        ProgressBar->setValue( 0 );
        setStatusBarFileInProgress( s_FilenameIn );
        break;
    }

    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog hochzaehlen.
*
*   @param i_NumOfFiles Anzahl der Dateien.
*   @param ui_filesize Groesse der Datei (gleich Anzahl der Zeichen), die verarbeitet werden soll.
*   @param ui_length Anzahl der bereits eingelesenen Zeichen.
*   @param InputStr Aktuelle Zeile. Daraus laesst sich die Anzahl der eingelesenen Zeichen ermitteln.
*
*   @return Anzahl der bereits eingelesenen Zeichen. 0 wenn Prozess abgebrochen wurde.
*/

unsigned int MainWindow::incProgress( const int i_NumOfFiles, const unsigned int ui_filesize, const unsigned int ui_length, const QString &InputStr )
{
    unsigned int	ui_newLength	= 0;

    int				i_Step			= 0;

// **********************************************************************************************

    ui_newLength = ui_length + (unsigned int) InputStr.length();
    i_Step		 = (int) (((float) ui_newLength/(float) ui_filesize)*100.)+1;

    switch ( i_NumOfFiles )
    {
    case 0:
        break;

    case 1:
        ProgressDialog->setValue( i_Step );

        if ( ProgressDialog->wasCanceled() == true )
            return( _APPBREAK_ );
        break;

    default:
        ProgressBar->setValue( i_Step );
        break;
    }

    QApplication::processEvents();

    return( ui_newLength );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog hochzaehlen.
*
*   @param i_NumOfFiles Anzahl der Dateien.
*   @param i_Step Anzahl der verarbeiteten Schritte.
*
*   @return Anzahl der bereits eingelesenen Zeichen. _APPBREAK_ wenn Prozess abgebrochen wurde.
*/

int MainWindow::incProgress( const int i_NumOfFiles, const int i_Step )
{
    switch ( i_NumOfFiles )
    {
    case 0:
        break;

    case 1:
        ProgressDialog->setValue( i_Step );

        if ( ProgressDialog->wasCanceled() == true )
            return( _APPBREAK_ );
        break;

    default:
        ProgressBar->setValue( i_Step );
        break;
    }

    QApplication::processEvents();

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog zuruecksetzen.
*
*   @param i_NumOfFiles Anzahl der Dateien.
*/

void MainWindow::resetProgress( const int i_NumOfFiles )
{
    switch ( i_NumOfFiles )
    {
    case 0:
        break;

    case 1:
        ProgressDialog->hide();
        ProgressDialog->reset();
        break;

    default:
        ProgressBar->reset();
        break;
    }

    QApplication::processEvents();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Der Cursor wird auf WaitCursor gesetzt. */

void MainWindow::setWaitCursor()
{
#if defined(Q_OS_LINUX)
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
#endif

#if defined(Q_OS_MAC)
    ;
#endif

#if defined(Q_OS_WIN)
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Der Cursor wird auf die Normalform zurueckgesetzt. */

void MainWindow::setNormalCursor()
{
#if defined(Q_OS_LINUX)
    QApplication::restoreOverrideCursor();
#endif

#if defined(Q_OS_MAC)
    ;
#endif

#if defined(Q_OS_WIN)
    QApplication::restoreOverrideCursor();
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Loesche Datei.
*
*   @return Dateisystemfehler
*/

int MainWindow::removeFile( const QString &s_Filename )
{
    QFile file( s_Filename );

    if( file.exists() == true )
    {
        file.remove();
        wait( 500 );
        return( file.error() );
    }

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die Anzahl der Abschnitte in einem String
*
*   @param String
*
*	@return Anzahl
*/

int MainWindow::NumOfSections( const QString &String )
{
    return( String.count( "\t" ) + 1 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Kopiert Elemente einer StringList, die einem Suchmuster entsprechen, in eine neue.
*
*   @param InStringList Eingangsliste
*   @param OutStringList Ausgangsliste
*   @param s_Pattern Suchmuster, * = es werden alle Elemente kopiert
*
*	@return Anzahl der Elemente in der neuen StringList
*/

int MainWindow::copyStringList( const QStringList &InStringList, QStringList &OutStringList, const QString &s_Pattern )
{
    OutStringList.clear();

    if ( s_Pattern == "*" )
    {
        OutStringList = InStringList;
    }
    else
    {
        for ( int i=0; i<InStringList.count(); ++i )
        {
            if ( InStringList.at( i ).contains( s_Pattern ) == true )
                OutStringList.append( InStringList.at( i ) );
        }
    }

    return ( OutStringList.count() );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Test ob String nur Tabs und damit keine Informationen enthaelt.
*
*   @param String
*
*   @retval true String ist leer
*   @retval false String ist nicht leer
*/

bool MainWindow::isEmptyLine( const QString &String )
{
    if ( ( String.count( "\t" ) == String.length() ) || ( String.isEmpty() == true ) )
        return( true );  // Line is empty
    else
        return( false ); // Line is not empty
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2012-02-18

/*! @brief Setzt das End-of-line Zeichen
*
*   @param i_EOL
*
*   @retval s_EOL
*/

QString MainWindow::setEOLChar( const int i_EOL )
{
    QString s_EOL = "\n"; // LF if compiled on MacOS X or Unix, CR+LF if compiled on Windows

    switch ( i_EOL )
    {
    case _MACOS_: // MacOS classic
        s_EOL = "\r";
        break;

#if defined(Q_OS_LINUX)
    case _WIN_:
        s_EOL = "\r\n";
        break;
#endif

#if defined(Q_OS_MAC)
    case _WIN_:
        s_EOL = "\r\n";
        break;
#endif

#if defined(Q_OS_WIN)
    case _WIN_:
        s_EOL = "\n";  // CR+LF if compiled on Windows!
        break;
#endif

    default:  // Unix
        s_EOL = "\n";
        break;
    }

    return( s_EOL );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2012-04-02

/*! @brief Setzt die Datei-Extension
*
*   @param i_Extension
*
*   @retval s_Extension
*/

QString MainWindow::setExtension( const int i_Extension )
{
    QString s_Extension = "";

    switch ( i_Extension )
    {
    case _CSV_:
        s_Extension = ".csv";
        break;
    default:
        s_Extension = ".txt";
        break;
    }

    return( s_Extension );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// Wait

void MainWindow::wait( const int msec )
{
    QTime dieTime = QTime::currentTime().addMSecs( msec );
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// hide window

void MainWindow::hideWindow()
{
#if defined(Q_OS_LINUX)
    ;
#endif

#if defined(Q_OS_MAC)
    if ( isHidden() == false )
    {
        savePreferences();
        setHidden( true );
        newWindowAction->setEnabled( true );
        hideWindowAction->setEnabled( false );
    }
#endif

#if defined(Q_OS_WIN)
    savePreferences();
    showMinimized();
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// show window

void MainWindow::newWindow()
{
#if defined(Q_OS_LINUX)
    ;
#endif

#if defined(Q_OS_MAC)
    if ( isHidden() == true )
    {
        setHidden( false );
        newWindowAction->setEnabled( false );
        hideWindowAction->setEnabled( true );
    }
#endif

#if defined(Q_OS_WIN)
    ;
#endif
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// Filename list contains binary file

bool MainWindow::containsBinaryFile( const QStringList &sl_FilenameList )
{
    QFileInfo fi;
    QString   s_Extension = "";

// **********************************************************************************************

    for ( int i=0; i<sl_FilenameList.count(); i++ )
    {
        fi.setFile( sl_FilenameList.at( i ) );
        s_Extension = fi.suffix().toLower();

        if ( s_Extension == "zip" )
            return( true );

        if ( s_Extension == "gz" )
            return( true );

        if ( s_Extension == "png" )
            return( true );

        if ( s_Extension == "jpg" )
            return( true );

        if ( s_Extension == "gif" )
            return( true );

        if ( s_Extension == "tif" )
            return( true );

        if ( s_Extension == "xls" )
            return( true );

        if ( s_Extension == "doc" )
            return( true );

        if ( s_Extension == "xlsx" )
            return( true );

        if ( s_Extension == "docx" )
            return( true );
    }

    return( false );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// find Zip

QString MainWindow::findZip( const int mode )
{
    QString s_Program = "";

    switch( mode )
    {
    case _ZIP_:
        #if defined(Q_OS_LINUX)
            s_Program = "zip";
        #endif

        #if defined(Q_OS_MAC)
            s_Program = "zip";
        #endif

        break;

    case _GZIP_:
        #if defined(Q_OS_LINUX)
            s_Program = "gzip";
        #endif

        #if defined(Q_OS_MAC)
            s_Program = "gzip";
        #endif

        break;
    }

#if defined(Q_OS_WIN)
    QString s_7zX86exe = QProcessEnvironment::systemEnvironment().value( "ProgramFiles(x86)", "C:" );
    s_7zX86exe.append( "/7-Zip/7z.exe" );
    QFileInfo fi_7zX86exe( s_7zX86exe );

    if ( fi_7zX86exe.exists() == false )
    {
        QString s_7z64exe = QProcessEnvironment::systemEnvironment().value( "ProgramW6432", "C:" );
        s_7z64exe.append( "/7-Zip/7z.exe" );
        QFileInfo fi_7z64exe( s_7z64exe );

        if ( fi_7z64exe.exists() == false )
        {
            QMessageBox::information( this, getApplicationName( true ), tr( "You have to install the\nprogram 7-zip (http://7-zip.org)\nbefore using this function." ) );

            return( "Zip not found" );
        }
        else
        {
            s_Program = fi_7z64exe.absoluteFilePath();
        }
    }
    else
    {
        s_Program = fi_7zX86exe.absoluteFilePath();
    }
#endif

    return( s_Program );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// find Unzip

QString MainWindow::findUnzip( const int mode )
{
    QString s_Program = "";

    switch( mode )
    {
    case _ZIP_:
        #if defined(Q_OS_LINUX)
            s_Program = "unzip";
        #endif

        #if defined(Q_OS_MAC)
            s_Program = "unzip";
        #endif

        break;

    case _GZIP_:
        #if defined(Q_OS_LINUX)
            s_Program = "gzip";
        #endif

        #if defined(Q_OS_MAC)
            s_Program = "gzip";
        #endif

        break;
    }

#if defined(Q_OS_WIN)
    QString s_7zX86exe = QProcessEnvironment::systemEnvironment().value( "ProgramFiles(x86)", "C:" );
    s_7zX86exe.append( "/7-Zip/7z.exe" );
    QFileInfo fi_7zX86exe( s_7zX86exe );

    if ( fi_7zX86exe.exists() == false )
    {
        QString s_7z64exe = QProcessEnvironment::systemEnvironment().value( "ProgramW6432", "C:" );
        s_7z64exe.append( "/7-Zip/7z.exe" );
        QFileInfo fi_7z64exe( s_7z64exe );

        if ( fi_7z64exe.exists() == false )
        {
            QMessageBox::information( this, getApplicationName( true ), tr( "You have to install the\nprogram 7-zip (http://7-zip.org)\nbefore using this function." ) );

            return( "Unzip not found" );
        }
        else
        {
            s_Program = fi_7z64exe.absoluteFilePath();
        }
    }
    else
    {
        s_Program = fi_7zX86exe.absoluteFilePath();
    }
#endif

    return( s_Program );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// find Curl

QString MainWindow::findCurl()
{
    QString s_Program = "";

    #if defined(Q_OS_LINUX)
        s_Program = "curl";
    #endif

    #if defined(Q_OS_MAC)
        s_Program = "curl";
    #endif

    #if defined(Q_OS_WIN)
        s_Program = QCoreApplication::applicationDirPath() + "/" + "curl.exe";
    #endif

    return( s_Program );
}
