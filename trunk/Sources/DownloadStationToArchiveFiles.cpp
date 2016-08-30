/* 2008-05-18                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-30

/*! @brief Erzeugen und ausfuehren einer Batch-Datei. Die Batch-Datei liesst mit Hilfe eines
*          ftp Clients Station-to-archive Dateien und speichert sie ab.
*
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*   @param s_DownloadPath Pfad in dem die Dateien gespeichert werden sollen
*   @param s_FTPServer Name des Servers
*   @param s_User Accountname
*   @param s_Password Password
*	@param b_DecompressFiles Wenn true werden die Dateien mit GZip entpackt
*	@param b_CheckFiles Wenn true werden die Dateien mit BSRN_fcheck geprueft
*	@param b_CheckAvailability Wenn true wird nur die Verfuegbarkeit der Dateien geprueft
*	@param b_Station Array von Flags, wenn Flag[i] = true werden die Datei der ausgewaehlten Station geladen
*	@param b_Month Array von Monaten
*	@param b_Year Array von Jahren
*/

QStringList MainWindow::downloadStationToArchiveFiles( structStation *Station_ptr,
                                                       const QString &s_DownloadPath,
                                                       const QString &s_FTPServer,
                                                       const QString &s_User, const QString &s_Password,
                                                       const bool b_DecompressFiles, const bool b_CheckFiles,
                                                       const bool b_CheckAvailability,
                                                       bool b_Station[MAX_NUM_OF_STATIONS+1],
                                                       bool b_Month[MAX_NUM_OF_MONTHS+1],
                                                       bool b_Year[MAX_NUM_OF_YEARS+1] )
{
    int         i               = 0;
    int         stopProgress    = 0;

    QString     s_EventLabel	= "";
    QString     s_Month			= "";
    QString     s_Year			= "";

    QString     s_GZIP    		= "";
    QString     s_Curl          = "";
    QString     s_BSRN_fcheck   = "";

    QString     s_arg           = "";

    QString     s_Message       = "";

    QStringList sl_FilenameList;
    QStringList sl_ReportList;

    QProcess    process;

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        s_Curl        = "curl";
        s_GZIP        = "gzip";
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
    #endif

    #if defined(Q_OS_MAC)
        s_Curl        = "curl";
        s_GZIP        = "gzip";
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
//      s_BSRN_fcheck = "\"/Users/rsieger/Development/GitHub/BSRN_Toolbox/trunk/Tools/f_check/BSRN_fcheck_MacOS\"";
    #endif

    #if defined(Q_OS_WIN)
        s_Curl        = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "curl.exe\"" );
        s_GZIP        = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "gzip.exe\"" );;
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck.exe\"" );
    #endif

    QDir::setCurrent( s_DownloadPath );

//-----------------------------------------------------------------------------------------------------------------------

    if ( b_CheckAvailability == false )
    {
        s_Message = tr( "Download BSRN station-to-archive files" );

        if ( ( b_CheckFiles == false ) && ( b_DecompressFiles == true ) )
            s_Message = tr( "Download and decompress BSRN station-to-archive files" );

        if ( b_CheckFiles == true )
            s_Message = tr( "Download, decompress, and check BSRN station-to-archive files" );

//-----------------------------------------------------------------------------------------------------------------------

        initFileProgress( MAX_NUM_OF_STATIONS, "Start download of station-to-archive files", s_Message );

        while ( ( i < MAX_NUM_OF_STATIONS ) && ( stopProgress != _APPBREAK_ ) )
        {
            if ( b_Station[i] == true )
            {
                s_EventLabel = findEventLabel( i, Station_ptr ).toLower();

                for ( int j=1; j<=MAX_NUM_OF_MONTHS; ++j )
                {
                    if ( b_Month[j] == true )
                    {
                        s_Month.sprintf( "%02d", j );

                        for ( int k=1; k<=MAX_NUM_OF_YEARS; k++ )
                        {
                            if ( b_Year[k] == true )
                            {
                                s_Year = QString( "%1" ).arg( k+1991 ).right( 2 );

                                QFileInfo fi_GZ( s_DownloadPath + "/" + s_EventLabel + s_Month + s_Year + ".dat.gz" );
                                QFileInfo fi_DAT( s_DownloadPath + "/" + s_EventLabel + s_Month + s_Year + ".dat" );
                                QFileInfo fi_REP( s_DownloadPath + "/" + s_EventLabel + s_Month + s_Year + ".rep" );

                                setStatusBar( tr( "Downloading " ) + QDir::toNativeSeparators( fi_GZ.fileName() ) + tr( " ..." ) );

                                s_arg = s_Curl;
                                s_arg.append( " -u " + s_User + ":" + s_Password );
                                s_arg.append( " -o " + fi_GZ.fileName() );
                                s_arg.append( " ftp://" + s_FTPServer + "//pub/" + s_EventLabel + "/" + fi_GZ.fileName() );

                                process.start( s_arg );
                                process.waitForFinished( -1 );

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) )
                                {
                                    s_arg = s_GZIP + " -df " + fi_DAT.fileName();

                                    process.start( s_arg );
                                    process.waitForFinished( -1 );
                                }

                                if ( b_CheckFiles == true )
                                {
                                    s_arg = s_BSRN_fcheck + " " + fi_DAT.fileName();

                                    process.start( s_arg );
                                    process.waitForFinished( -1 );
                                }

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) )
                                    sl_FilenameList.append( fi_DAT.absoluteFilePath() );
                                else
                                    sl_FilenameList.append( fi_DAT.absoluteFilePath() );

                                if ( b_CheckFiles == true )
                                    sl_ReportList.append( fi_REP.absoluteFilePath() );
                            }
                        }
                    }
                }
            }

            stopProgress = incFileProgress( MAX_NUM_OF_STATIONS, ++i );
        }

        resetFileProgress( MAX_NUM_OF_STATIONS );

        if ( b_CheckFiles == true )
            concatenateFiles( s_DownloadPath + "/" + "BSRN_fcheck_report.txt", sl_ReportList, tr( "Concatenate files working ..." ), 0, true );
    }
    else
    {
        initFileProgress( MAX_NUM_OF_STATIONS, tr( "Check availability for station " ), tr( "Check Availability" ) );

        while ( ( i < MAX_NUM_OF_STATIONS ) && ( stopProgress != _APPBREAK_ ) )
        {
            if ( b_Station[i] == true )
            {
                s_EventLabel = findEventLabel( i, Station_ptr ).toLower();

                setStatusBar( tr( "Check availability for station " ) + QDir::toNativeSeparators( s_EventLabel ) + tr( " ..." ) );

                s_arg = s_Curl;
                s_arg.append( " -u " + s_User + ":" + s_Password );
                s_arg.append( " -o " + s_EventLabel + "_filelist.txt" );
                s_arg.append( " ftp://" + s_FTPServer + "//pub/" + s_EventLabel + "/" );

                process.start( s_arg );
                process.waitForFinished( -1 );

                sl_FilenameList.append( s_DownloadPath + s_EventLabel + "_filelist.txt" );
            }

            stopProgress = incFileProgress( MAX_NUM_OF_STATIONS, ++i );
        }

        resetFileProgress( MAX_NUM_OF_STATIONS );
    }

//-------------------------------------------------------------------------------------------------------

    setStatusBar( tr( "Done" ) );

    return( sl_FilenameList );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-30

/*! @brief Steuerung des Downloads */

void MainWindow::doDownloadStationToArchiveFiles()
{
    int         err = _NOERROR_;

    QStringList sl_FilenameList;

// **********************************************************************************************

    err = doDownloadManagerDialog( gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles,
                                   gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

    if ( err == QDialog::Accepted )
    {
        sl_FilenameList = downloadStationToArchiveFiles( g_Station_ptr, gs_DownloadPath, gs_FTPServer, gs_User, gs_Password,
                                                         gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability,
                                                         gb_Station, gb_Month, gb_Year );

        checkScriptResults( sl_FilenameList );
    }

// **********************************************************************************************

    endTool( _NOERROR_, _NOERROR_, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Download of files was canceled" ), false, false );

    onError( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-13

void MainWindow::checkScriptResults( const QStringList &sl_FilenameList )
{
    int         i_NumOfFiles        = 0;
    int         i_NumOfEmptyFiles   = 0;
    int         i_NumOfMissingFiles = 0;

    bool        b_OK                = true;

    QString     s_Message           = "";

// **********************************************************************************************

    i_NumOfFiles = sl_FilenameList.count();

    for ( int i=0; i<i_NumOfFiles; i++ )
    {
        QFileInfo fi( sl_FilenameList.at( i ) );

        if ( fi.exists() == true )
        {
            if ( fi.size() == 0 )
            {
                i_NumOfEmptyFiles++;
                b_OK = false;
            }
        }
        else
        {
            i_NumOfMissingFiles++;
            b_OK = false;
        }
    }

    if ( b_OK == true )
    {
        switch ( i_NumOfFiles )
        {
        case 0:
            break;

        case 1:
            s_Message = tr( "File has been downloaded." );
            break;

        default:
            s_Message = tr( "All files have been downloaded." );
            break;
        }
    }
    else
    {
        s_Message = tr( "Something is wrong. " );

        switch ( i_NumOfMissingFiles )
        {
        case 0:
            break;

        case 1:
            s_Message.append( tr( "One station-to-archive file is missing. " ) );
            break;

        default:
            s_Message.append( tr( "Some Station-to-archive files are missing. " ) );
            break;
        }

        switch ( i_NumOfEmptyFiles )
        {
        case 0:
            break;

        case 1:
            s_Message.append( tr( "One file is empty. " ) );
            break;

        default:
            s_Message.append( tr( "Some files are empty. " ) );
            break;
        }

        s_Message.append( tr( "Before calling the support, please check the availibilty of files." ) );
    }

    QMessageBox::information( this, getApplicationName( true ), s_Message );
}
