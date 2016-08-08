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
                                                  const QString& s_DownloadPath,
                                                  const QString& s_FTPServer,
                                                  const QString& s_User, const QString& s_Password,
                                                  const bool b_DecompressFiles, const bool b_CheckFiles,
                                                  const bool b_CheckAvailability,
                                                  bool b_Station[MAX_NUM_OF_STATIONS+1],
                                                  bool b_Month[MAX_NUM_OF_MONTHS+1],
                                                  bool b_Year[MAX_NUM_OF_YEARS+1] )
{
    int		i_Num_of_Files	= 0;

    QString	s_EventLabel	= "";
    QString s_Month			= "";
    QString	s_Year			= "";

    QString s_GZIP    		= "";
    QString s_cURL          = "";
    QString s_BSRN_fcheck   = "";
    QString s_Message       = "";

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

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) )
                                    tcmd << s_GZIP << " -df " << fi_GZ.fileName() << endl;

                                if ( b_CheckFiles == true )
                                    tcmd << s_BSRN_fcheck << " " << fi_DAT.fileName() << " >> temp.txt" << endl << endl;
                                else
                                    tcmd << endl;
                            }

                            i_Num_of_Files++;
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

                i_Num_of_Files++;
            }
        }

        tcmd << endl;
        tcmd << "rm get_BSRN_Files.*" << endl;
        tcmd << "echo All done" << endl;
    }

//-------------------------------------------------------------------------------------------------------

    fcmd.close();

//-------------------------------------------------------------------------------------------------------

    return( i_Num_of_Files );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-12-30

/*! @brief Steuerung des Downloads */

void MainWindow::doDownloadStationToArchiveFiles()
{
    int		 err            = _NOERROR_;
    int      stopProgress   = _NOERROR_;

    int      i_Num_of_Files = 0;

    QString  s_arg          = "";
    QString  s_Message      = "";

    QProcess process;

    bool     b_startScript  = true;

// **********************************************************************************************

    qDebug() << gb_DecompressFiles << gb_CheckFiles << gb_CheckAvailability;

    err = doDownloadManagerDialog( gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

    qDebug() << gb_DecompressFiles << gb_CheckFiles << gb_CheckAvailability;

    if ( err == QDialog::Accepted )
    {
        i_Num_of_Files = downloadStationToArchiveFiles( g_Station_ptr, gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

        if ( i_Num_of_Files > 0 )
        {
            #if defined(Q_OS_LINUX)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );
            #endif

            #if defined(Q_OS_MAC)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );
            #endif

            #if defined(Q_OS_WIN)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.cmd" );
            #endif

            if ( fcmd.exists() == true )
            {
                QFile frep( gs_DownloadPath + "BSRN_fcheck_report.txt" );
                if ( frep.exists() == true )
                    frep.remove();

                wait( 500 );

                if ( b_startScript == true )
                {
                    #if defined(Q_OS_LINUX)
                        s_arg = "chmod u+x \"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";
                        process.startDetached( s_arg );

                        // + Rainer
                        // s_Message = "Please start the script\n\n    get_BSRN_Files.sh\n\nmanually from the command line. You can do this by typing:\n\n     cd " + gs_DownloadPath + "\n     ./get_BSRN_Files.sh";
                        // QMessageBox::information( this, getApplicationName( true ), s_Message );
                        // - Rainer

                        // + Holger
                        s_arg = "\"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";

                        wait( 500 );

                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".cmd\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            if ( ( gb_CheckFiles == true ) && ( gb_CheckAvailability == false ) )
                            {
                                QFileInfo fi( gs_DownloadPath + "BSRN_fcheck_report.txt" );
                                while ( fi.exists() == false )
                                    wait( 1000 );
                            }

                            s_Message = "All done. Files have been downloaded to\n" + QDir::toNativeSeparators( gs_DownloadPath );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                        // - Holger
                    #endif

                    #if defined(Q_OS_MAC)
                        s_arg = "chmod u+x \"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";
                        process.startDetached( s_arg );

                        // + Rainer
                        // s_Message = "Please start the script\n\n    get_BSRN_Files.sh\n\nmanually from the command line. You can do this by typing:\n\n     cd " + gs_DownloadPath + "\n     ./get_BSRN_Files.sh";
                        // QMessageBox::information( this, getApplicationName( true ), s_Message );
                        // - Rainer

                        // + Holger
                        s_arg = "\"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";

                        wait( 500 );

                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".cmd\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            if ( ( gb_CheckFiles == true ) && ( gb_CheckAvailability == false ) )
                            {
                                QFileInfo fi( gs_DownloadPath + "BSRN_fcheck_report.txt" );
                                while ( fi.exists() == false )
                                    wait( 1000 );
                            }

                            s_Message = "All done. Files have been downloaded to\n" + QDir::toNativeSeparators( gs_DownloadPath );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                        // - Holger
                    #endif

                    #if defined(Q_OS_WIN)
                        s_arg = "\"" + QDir::toNativeSeparators( fcmd.fileName() ) + "\"";

                        if ( process.startDetached( s_arg ) == false )
                        {
                            s_Message = "Cannot start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + ".cmd\n\n Please start the script manually from your shell.";
                            QMessageBox::warning( this, getApplicationName( true ), s_Message );
                        }
                        else
                        {
                            if ( ( gb_CheckFiles == true ) && ( gb_CheckAvailability == false ) )
                            {
                                QFileInfo fi( gs_DownloadPath + "BSRN_fcheck_report.txt" );
                                while ( fi.exists() == false )
                                    wait( 1000 );
                            }

                            s_Message = "All done. Files have been downloaded to\n" + QDir::toNativeSeparators( gs_DownloadPath );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                    #endif
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
