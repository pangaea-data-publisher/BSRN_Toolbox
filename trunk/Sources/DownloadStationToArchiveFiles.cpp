/* 2008-05-18                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-12-30

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

int MainWindow::downloadStationToArchiveFiles( structStation *Station_ptr,
                                                  const QString &s_DownloadPath,
                                                  const QString &s_FTPServer,
                                                  const QString &s_User, const QString &s_Password,
                                                  const bool b_DecompressFiles, const bool b_CheckFiles,
                                                  const bool b_CheckAvailability,
                                                  bool b_Station[MAX_NUM_OF_STATIONS+1],
                                                  bool b_Month[MAX_NUM_OF_MONTHS+1],
                                                  bool b_Year[MAX_NUM_OF_YEARS+1],
                                                  QStringList &sl_FilenameList )
{
    QString     s_Filename      = "";

    QString     s_EventLabel	= "";
    QString     s_Month			= "";
    QString     s_Year			= "";

    QString     s_GZIP    		= "";
    QString     s_cURL          = "";
    QString     s_BSRN_fcheck   = "";
    QString     s_Message       = "";

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        QFile fcmd( s_DownloadPath + "get_BSRN_Files.sh" );
    #endif

    #if defined(Q_OS_WIN)
        QFile fcmd( s_DownloadPath + "get_BSRN_Files.cmd" );
    #endif

    #if defined(Q_OS_MAC)
        QFile fcmd( s_DownloadPath + "get_BSRN_Files.sh" );
    #endif

    if ( fcmd.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

    QTextStream tcmd( &fcmd );

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        s_cURL        = "curl";
        s_GZIP        = "gzip";
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
    #endif

    #if defined(Q_OS_MAC)
        s_cURL        = "curl";
        s_GZIP        = "gzip";
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
    #endif

    #if defined(Q_OS_WIN)
        s_cURL        = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "curl.exe\"" );
        s_GZIP        = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "gzip.exe\"" );;
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck.exe\"" );
    #endif

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        tcmd << "#!/bin/bash" << endl;
    #endif

    #if defined(Q_OS_MAC)
        tcmd << "#!/bin/bash" << endl;
    #endif

    #if defined(Q_OS_WIN)
        tcmd << "@echo off" << endl;
    #endif

    if ( b_CheckAvailability == false )
    {
        tcmd << "cd \"" << s_DownloadPath << "\"" << endl << endl;

        s_Message = tr( "Download BSRN station-to-archive files" );

        if ( ( b_CheckFiles == false ) && ( b_DecompressFiles == true ) )
            s_Message = tr( "Download and decompress BSRN station-to-archive files" );

        if ( b_CheckFiles == true )
            s_Message = tr( "Download, decompress, and check BSRN station-to-archive files" );

        tcmd << "echo " << s_Message << endl;
        tcmd << "echo Please wait..." << endl << endl;

        for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
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

                                tcmd << s_cURL;
                                tcmd << " -u " << s_User << ":" << s_Password;
                                tcmd << " -o " << fi_GZ.fileName();
                                tcmd << " ftp://" << s_FTPServer << "//pub/" << s_EventLabel << "/" << fi_GZ.fileName() << endl;

                                s_Filename = fi_GZ.fileName();

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) )
                                {
                                    s_Filename = fi_DAT.fileName();
                                    tcmd << s_GZIP << " -df " << fi_GZ.fileName() << endl;
                                }

                                if ( b_CheckFiles == true )
                                    tcmd << s_BSRN_fcheck << " " << fi_DAT.fileName() << " >> temp.txt" << endl << endl;
                                else
                                    tcmd << endl;

                                sl_FilenameList.append( s_DownloadPath + s_Filename );
                            }
                        }
                    }
                }
            }
        }

        if ( b_CheckFiles == true )
        {
            #if defined(Q_OS_LINUX)
                tcmd << "rm *.rep" << endl;
                tcmd << "mv temp.txt BSRN_fcheck_report.txt" << endl << endl;
            #endif

            #if defined(Q_OS_MAC)
                tcmd << "rm *.rep" << endl << endl;
                tcmd << "mv temp.txt BSRN_fcheck_report.txt" << endl << endl;
            #endif

            #if defined(Q_OS_WIN)
                tcmd << "del *.rep" << endl;
                tcmd << "ren temp.txt BSRN_fcheck_report.txt" << endl << endl;
            #endif

            tcmd << "echo All done" << endl;
        }
        else
        {
            tcmd << "echo All done";
            tcmd << endl;
        }
    }
    else
    {
        tcmd << "echo Check availability of BSRN files" << endl;
        tcmd << "echo Please wait..." << endl;
        tcmd << "cd \"" << s_DownloadPath << "\"" << endl << endl;

        for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
        {
            if ( b_Station[i] == true )
            {
                s_EventLabel = findEventLabel( i, Station_ptr ).toLower();

                tcmd << s_cURL;
                tcmd << " -u " << s_User << ":" << s_Password;
                tcmd << " -o " << s_EventLabel << "_filelist.txt";
                tcmd << " ftp://" << s_FTPServer << "//pub/" << s_EventLabel << "/" << endl;

                sl_FilenameList.append( s_DownloadPath + s_EventLabel + "_filelist.txt" );
            }
        }

        tcmd << endl;
        tcmd << "rm get_BSRN_Files.sh" << endl;    // Linux and macOS
        tcmd << "del get_BSRN_Files.cmd" << endl;  // Windows
        tcmd << "echo All done" << endl;
    }

//-------------------------------------------------------------------------------------------------------

    fcmd.close();

//-------------------------------------------------------------------------------------------------------

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-12-30

/*! @brief Steuerung des Downloads */

void MainWindow::doDownloadStationToArchiveFiles()
{
    int		    err                 = _NOERROR_;
    int         stopProgress        = _NOERROR_;

    QString     s_arg               = "";
    QString     s_Message           = "";

    QStringList sl_FilenameList;

    QProcess    process;

// **********************************************************************************************

    err = doDownloadManagerDialog( gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_RunScript, gb_Station, gb_Month, gb_Year );

    if ( err == QDialog::Accepted )
    {
        err = downloadStationToArchiveFiles( g_Station_ptr, gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year, sl_FilenameList );

        if ( sl_FilenameList.count() > 0 )
        {
            #if defined(Q_OS_LINUX)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );

                s_arg = "chmod u+x \"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";
                process.startDetached( s_arg );

                wait( 500 );
            #endif

            #if defined(Q_OS_MAC)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );

                s_arg = "chmod u+x \"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";
                process.startDetached( s_arg );

                wait( 500 );
            #endif

            #if defined(Q_OS_WIN)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.cmd" );
            #endif

            if ( fcmd.exists() == true )
            {
                QFile frep( gs_DownloadPath + "BSRN_fcheck_report.txt" );

                if ( frep.exists() == true )
                    frep.remove();

                if ( gb_RunScript == true )
                {
                    s_arg = "\"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";

                    #if defined(Q_OS_LINUX)
                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".sh\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            s_Message = checkScriptResults( gb_CheckFiles, gs_DownloadPath, sl_FilenameList );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                    #endif

                    #if defined(Q_OS_MAC)
                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".sh\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            s_Message = checkScriptResults( gb_CheckFiles, gs_DownloadPath, sl_FilenameList );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                    #endif

                    #if defined(Q_OS_WIN)
                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".cmd\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            s_Message = checkScriptResults( gb_CheckFiles, gs_DownloadPath, sl_FilenameList );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                    #endif
                }
                else
                {
                    s_Message = tr( "Please start the script manually from your shell." );
                    QMessageBox::information( this, getApplicationName( true ), s_Message );
                }
            }

            err = _NOERROR_;
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Concatenate files was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-13

QString MainWindow::checkScriptResults( const bool b_CheckFiles, const QString &s_DownloadPath, const QStringList &sl_FilenameList )
{
    int         i_NumOfTurns        = 0;
    int         i_NumOfFiles        = 0;
    int         i_NumOfEmptyFiles   = 0;
    int         i_NumOfMissingFiles = 0;

    bool        b_OK                = true;

    QString     s_Message           = "";

// **********************************************************************************************

    i_NumOfFiles = sl_FilenameList.count();

    if ( b_CheckFiles == true )
    {
        QFileInfo fi( s_DownloadPath + "BSRN_fcheck_report.txt" );

        while ( fi.exists() == false )
            wait( 1000 );
    }

    for ( int i=0; i<i_NumOfFiles; i++ )
    {
        QFileInfo fi( sl_FilenameList.at( i ) );

        if ( b_CheckFiles == false )
        {
            while ( fi.exists() == false )
            {
                wait( 1000 );

                if ( ++i_NumOfTurns > 10 )
                    break;
            }
        }

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

    return( s_Message );
}
