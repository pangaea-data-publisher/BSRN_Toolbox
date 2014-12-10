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
                                                  bool b_Station[MAX_NUM_OF_STATION+1],
                                                  bool b_Month[MAX_NUM_OF_MONTH+1],
                                                  bool b_Year[MAX_NUM_OF_YEAR+1] )
{
    int		i_Num_of_Files			= 0;

    QString	s_EventLabel			= "";
    QString s_EventLabel_checked	= "";
    QString s_Month					= "";
    QString	s_Year					= "";

    QString s_Program				= "";

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

    QFile fout( s_DownloadPath + "get_BSRN_Files.txt" );
    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
    {
        fcmd.close();
        return( -20 );
    }

//-----------------------------------------------------------------------------------------------------------------------

    QTextStream tcmd( &fcmd );
    QTextStream tout( &fout );

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        if ( b_CheckAvailability == false )
        {
            tcmd << "#!/bin/bash" << endl;
            tcmd << "echo Get BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -n " << s_FTPServer << " < get_BSRN_Files.txt" << " > get_BSRN_Files.log" << endl;
            tcmd << "echo Download completed" << endl;

            if ( b_CheckFiles == true )
            {
                tcmd << endl;
                tcmd << "echo Decompress and check files" << endl;
                tcmd << "echo Please wait..." << endl << endl;
            }
            else
            {
                if ( b_DecompressFiles == true )
                {
                    tcmd << endl;
                    tcmd << "echo Decompress files" << endl;
                    tcmd << "echo Please wait..." << endl << endl;
                }
            }
        }
        else
        {
            tcmd << "#!/bin/bash" << endl;
            tcmd << "echo Check availability of BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -n " << s_FTPServer << " < get_BSRN_Files.txt" << " > get_BSRN_Files.log" << endl;
        }
    #endif

    #if defined(Q_OS_WIN)
        if ( b_CheckAvailability == false )
        {
            tcmd << "@echo off" << endl;
            tcmd << "echo Get BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -v -s:\"" << QDir::toNativeSeparators( s_DownloadPath + "get_BSRN_Files.txt" ) << "\" > \"" << QDir::toNativeSeparators( s_DownloadPath + "get_BSRN_Files.log" ) << "\"" << endl;
            tcmd << "echo Download completed" << endl;

            if ( b_CheckFiles == true )
            {
                tcmd << endl;
                tcmd << "echo Decompress and check files" << endl;
                tcmd << "echo Please wait..." << endl << endl;
            }
            else
            {
                if ( b_DecompressFiles == true )
                {
                    tcmd << endl;
                    tcmd << "echo Decompress files" << endl;
                    tcmd << "echo Please wait..." << endl << endl;
                }
            }
        }
        else
        {
            tcmd << "@echo off" << endl;
            tcmd << "echo Check availability of BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -v -s:\"" << QDir::toNativeSeparators( s_DownloadPath + "get_BSRN_Files.txt" ) << "\" > \"" << QDir::toNativeSeparators( s_DownloadPath + "get_BSRN_Files.log" ) << "\"" << endl;
        }
    #endif

    #if defined(Q_OS_MAC)
        if ( b_CheckAvailability == false )
        {
            tcmd << "#!/bin/bash" << endl;
            tcmd << "echo Get BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -d > get_BSRN_Files.log -n " << s_FTPServer << " < get_BSRN_Files.txt" << endl;
            tcmd << "echo Download completed" << endl;

            if ( b_CheckFiles == true )
            {
                tcmd << endl;
                tcmd << "echo Decompress and check files" << endl;
                tcmd << "echo Please wait..." << endl << endl;
            }
            else
            {
                if ( b_DecompressFiles == true )
                {
                    tcmd << endl;
                    tcmd << "echo Decompress files" << endl;
                    tcmd << "echo Please wait..." << endl << endl;
                }
            }
        }
        else
        {
            tcmd << "#!/bin/bash" << endl;
            tcmd << "echo Check availability of BSRN files" << endl;
            tcmd << "echo Please wait..." << endl;
            tcmd << "cd \"" << s_DownloadPath << "\"" << endl;
            tcmd << "ftp -d > get_BSRN_Files.log -n " << s_FTPServer << " < get_BSRN_Files.txt" << endl;
        }
    #endif

//-----------------------------------------------------------------------------------------------------------------------

    #if defined(Q_OS_LINUX)
        tout << "user " << s_User << " " << s_Password << endl;
        tout << "type binary" << endl;
    #endif

    #if defined(Q_OS_WIN)
        tout << "open " << s_FTPServer << endl;
        tout << s_User << endl;
        tout << s_Password << endl;
        tout << "type binary" << endl;
    #endif

    #if defined(Q_OS_MAC)
        tout << "user " << s_User << " " << s_Password << endl;
        tout << "type binary" << endl;
    #endif

    for ( int i=1; i<=MAX_NUM_OF_STATION; ++i )
    {
        if ( b_Station[i] == true )
        {
            s_EventLabel = findEventLabel( i, Station_ptr ).toLower();

            for ( int j=1; j<=MAX_NUM_OF_MONTH; ++j )
            {
                if ( b_Month[j] == true )
                {
                    s_Month.sprintf( "%02d", j );

                    for ( int k=1; k<=MAX_NUM_OF_YEAR; k++ )
                    {
                        if ( b_Year[k] == true )
                        {
                            if ( b_CheckAvailability == false )
                            {
                                s_Year = QString( "%1" ).arg( k+1991 ).right( 2 );

                                QFileInfo fi_GZ( s_DownloadPath + "/" + s_EventLabel + s_Month + s_Year + ".dat.gz" );
                                QFileInfo fi_DAT( s_DownloadPath + "/" + s_EventLabel + s_Month + s_Year + ".dat" );

                                tout << "get ";
                                tout << "\"./" << s_EventLabel << "/" << fi_GZ.fileName() << "\"";
                                tout << " ";
                                tout << "\"" << QDir::toNativeSeparators( fi_GZ.absoluteFilePath().replace( "//", "/" ) ) << "\"" << endl;

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) )
                                {
                                    tcmd << "echo unzip " << fi_GZ.fileName() << endl;

                                    #if defined(Q_OS_LINUX)
                                        tcmd << "gzip -df " << fi_GZ.fileName() << endl << endl;
                                    #endif

                                    #if defined(Q_OS_WIN)
                                        s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "gzip.exe" );
                                        tcmd << "\"" << s_Program << "\" -df \"" << QDir::toNativeSeparators( fi_GZ.absoluteFilePath().replace( "//", "/" ) ) << "\"" << endl << endl;
                                    #endif

                                    #if defined(Q_OS_MAC)
                                        tcmd << "gzip -df " << fi_GZ.fileName() << endl << endl;
                                    #endif
                                }

                                if ( b_CheckFiles == true )
                                {
                                    tcmd << "echo BSRN_fcheck " << fi_DAT.fileName() << endl;

                                    #if defined(Q_OS_LINUX)
                                        s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck" );
                                        tcmd << "\"" << s_Program << "\" " << fi_DAT.fileName() << endl<< endl;
                                    #endif

                                    #if defined(Q_OS_WIN)
                                        s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck.exe" );
                                        tcmd << "\"" << s_Program << "\" \"" << QDir::toNativeSeparators( fi_DAT.absoluteFilePath().replace( "//", "/" ) ) << "\"" << endl<< endl;
                                    #endif

                                    #if defined(Q_OS_MAC)
                                        s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck" );
                                        tcmd << "\"" << s_Program << "\" " << fi_DAT.fileName() << endl<< endl;
                                    #endif
                                }
                            }
                            else
                            {
                                if ( s_EventLabel != s_EventLabel_checked )
                                {
                                    tout << "mls ";
                                    tout << "\"./" << s_EventLabel << "\"";
                                    tout << " ";
                                    tout << "\"" << s_DownloadPath << s_EventLabel << "_availability.txt\"" << endl;
                                    tout << "y" << endl;

                                    s_EventLabel_checked = s_EventLabel;
                                }
                            }

                            i_Num_of_Files++;
                        }
                    }
                }
            }
        }
    }

    tout << "bye" << endl;

    tcmd << "echo All done" << endl;

    #if defined(Q_OS_WIN)
        tcmd << "Pause" << endl;
    #endif

//-------------------------------------------------------------------------------------------------------

    fcmd.close();
    fout.close();

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
    int		 err          = _NOERROR_;
    int      stopProgress = _NOERROR_;
    int      ret          = QMessageBox::Yes;

    QString  s_arg        = "";
    QString  s_Message    = "";

    QProcess process;

    bool     b_questionStartScript = false;

// **********************************************************************************************

    err = doDownloadManagerDialog( gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

    if ( err == QDialog::Accepted )
    {
        err = downloadStationToArchiveFiles( g_Station_ptr, gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

        if ( err > 0 )
        {
            #if defined(Q_OS_LINUX)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );
            #endif

            #if defined(Q_OS_WIN)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.cmd" );
            #endif

            #if defined(Q_OS_MAC)
                QFile fcmd( gs_DownloadPath + "get_BSRN_Files.sh" );
            #endif

            QFile ftxt( gs_DownloadPath + "get_BSRN_Files.txt" );

            if ( ( fcmd.exists() == true ) && ( ftxt.exists() == true ) )
            {
                if ( b_questionStartScript == true )
                {
                    s_Message	= "Building of script succesful.\nStart it now?";
                    ret			= QMessageBox::question( this, "BSRN Toolbox", s_Message, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton );
                }

                if ( ret == QMessageBox::No )
                {
                    s_Message = "Start the script\n\n    " + QDir::toNativeSeparators( fcmd.fileName() ) + "\n\nmanually from your shell.";
                    QMessageBox::information( this, getApplicationName( true ), s_Message );
                }
                else
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
                            s_Message = "All done. Files have been downloaded to\n" + QDir::toNativeSeparators( gs_DownloadPath );
                            QMessageBox::information( this, getApplicationName( true ), s_Message );
                        }
                        // - Holger
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
