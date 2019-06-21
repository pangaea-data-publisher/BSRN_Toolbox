/* 2008-05-18                 */
/* Dr. Rainer Sieger          */

#include "Application.h"
#include <QDebug>

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
                                                       bool b_Year[MAX_NUM_OF_YEARS+1],
                                                       int& i_NumOfEmptyFiles,int& i_NumOfMissingFiles, int& i_numOfChecks)
{
    int         i                   = 1;
    int         m                   = 0;
    int         stopProgress        = 0;

    int         i_NumOfDownloads    = 0;
    int         i_NumOfStations     = 0;
    int         i_NumOfYears        = 0;
    int         i_NumOfMonths       = 0;


    QString     s_EventLabel        = "";
    QString     s_Month             = "";
    QString     s_Year              = "";

    QString     s_GZIP              = "";
    QString     s_Curl              = "";
    QString     s_BSRN_fcheck       = "";

    QString     s_arg               = "";

    QString     s_Message           = "";

    QStringList sl_FilenameList;
    QStringList sl_ReportList;

    QProcess    process;

    QString targetfile = "";

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
// calculate number of downloads

        for ( int i=1; i<=MAX_NUM_OF_STATIONS; i++ )  //100
            if ( b_Station[i] == true )
                i_NumOfStations++;

        //for ( int i=1; i<MAX_NUM_OF_MONTHS; i++ )
        for ( int i=1; i<=MAX_NUM_OF_MONTHS; i++ )  //12
            if ( b_Month[i] == true )
                i_NumOfMonths++;

        //for ( int i=1; i<MAX_NUM_OF_YEARS; i++ )
          for ( int i=1; i<=MAX_NUM_OF_YEARS; i++ )  //30
            if ( b_Year[i] == true )
                i_NumOfYears++;

//-----------------------------------------------------------------------------------------------------------------------

    if ( b_CheckAvailability == false )
    {
        s_Message = tr( "Download BSRN station-to-archive files" );

        if ( ( b_CheckFiles == false ) && ( b_DecompressFiles == true ) )
            s_Message = tr( "Download and decompress BSRN station-to-archive files" );

        else if (( b_CheckFiles == true ) && ( b_DecompressFiles == true ) )
            s_Message = tr( "Download, decompress, and check BSRN station-to-archive files" );

        else if (( b_CheckFiles == false ) && ( b_DecompressFiles == false ))
            s_Message = tr( "Download BSRN station-to-archive files" );

        //impossible to download as zip and to check a zipped file!
        else if (( b_CheckFiles == true ) && ( b_DecompressFiles == false ))
            s_Message = tr( "Impossible, to check zipped BSRN station-to-archive files! Files will be unzipped!" );

//-----------------------------------------------------------------------------------------------------------------------

        i_NumOfDownloads = i_NumOfStations * i_NumOfYears * i_NumOfMonths;
        i_numOfChecks = i_NumOfStations * i_NumOfYears * i_NumOfMonths;

        //QString checks=QString("%1 Files are checked").arg(i_numOfChecks);
        //QMessageBox::information( this, getApplicationName( true ), checks );

        initFileProgress( i_NumOfDownloads, "Start download of station-to-archive files", s_Message );

//-----------------------------------------------------------------------------------------------------------------------

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

                                targetfile="";
                                targetfile=fi_DAT.absoluteFilePath();
                                removeFile( fi_GZ.absoluteFilePath() );
                                removeFile( fi_DAT.absoluteFilePath() );
                                removeFile( fi_REP.absoluteFilePath() );

                                setStatusBar( tr( "Downloading " ) + QDir::toNativeSeparators( fi_GZ.fileName() ) + tr( " ..." ) );

                                s_arg = s_Curl;
                                s_arg.append( " -u " + s_User + ":" + s_Password );
                                s_arg.append( " -o " + fi_GZ.fileName() );
                                s_arg.append( " ftp://" + s_FTPServer + "/" + s_EventLabel + "/" + fi_GZ.fileName() );

                                qDebug() << s_arg;

                                //start process, targetfile is: fi_DAT.absoluteFilePath()
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

                                if ( ( b_DecompressFiles == true ) || ( b_CheckFiles == true ) ){
                                    //decompress:
                                    QFileInfo fi( fi_DAT.absoluteFilePath());
                                    if ( fi.exists() == true ){
                                        sl_FilenameList.append( fi_DAT.absoluteFilePath() );
                                        if ( fi.size() == 0 )
                                        {
                                            i_NumOfEmptyFiles++;

                                        }
                                    }
                                    else
                                    {
                                        i_NumOfMissingFiles++;

                                    }

                                }
                                else {
                                    //get as zip files fi_GZ:
                                    QFileInfo fi( fi_GZ.absoluteFilePath());
                                    if ( fi.exists() == true ){
                                        sl_FilenameList.append( fi_GZ.absoluteFilePath() );
                                        if ( fi.size() == 0 )
                                        {
                                            i_NumOfEmptyFiles++;

                                        }
                                    }
                                    else
                                    {
                                        i_NumOfMissingFiles++;

                                    }

                                }

                                if ( b_CheckFiles == true )
                                    sl_ReportList.append( fi_REP.absoluteFilePath() );


                                stopProgress = incFileProgress( i_NumOfDownloads, ++m );
                            }
                        }
                    }
                }
            }

            i++;
        }

        resetFileProgress( i_NumOfDownloads );

        if ( b_CheckFiles == true )
        {
            removeFile( s_DownloadPath + "/" + "BSRN_fcheck_report.txt" );
            concatenateFiles( s_DownloadPath + "/" + "BSRN_fcheck_report.txt", sl_ReportList, tr( "Concatenate files working ..." ), 0, true );
        }
    }
    else
    {
        initFileProgress( i_NumOfStations, tr( "Check availability for station " ), tr( "Check Availability" ) );

        i_numOfChecks = i_NumOfStations;

        while ( ( i < MAX_NUM_OF_STATIONS ) && ( stopProgress != _APPBREAK_ ) )
        {
            if ( b_Station[i] == true )
            {
                s_EventLabel = findEventLabel( i, Station_ptr ).toLower();

                setStatusBar( tr( "Check availability for station " ) + QDir::toNativeSeparators( s_EventLabel ) + tr( " ..." ) );

                s_arg = s_Curl;
                s_arg.append( " -u " + s_User + ":" + s_Password );
                s_arg.append( " -o " + s_EventLabel + "_filelist.txt" );
                s_arg.append( " ftp://" + s_FTPServer + "/" + s_EventLabel + "/" );

                //looking for targetfile s_DownloadPath + s_EventLabel + "_filelist.txt":
                targetfile="";
                targetfile=s_DownloadPath + s_EventLabel + "_filelist.txt";

                process.start( s_arg );
                process.waitForFinished( -1 );

                qDebug() << s_arg;

                QFileInfo fi( targetfile );

                if ( fi.exists() == true )
                    {
                        if ( fi.size() == 0 )
                        {
                            i_NumOfEmptyFiles++;

                        }
                     sl_FilenameList.append( s_DownloadPath + s_EventLabel + "_filelist.txt" );
                    }  //exists
                else
                {
                    i_NumOfMissingFiles++;

                }
                stopProgress = incFileProgress( i_NumOfStations, ++m );
            }

            i++;
        }

        resetFileProgress( i_NumOfStations );
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

    int   i_NumOfEmptyFiles = 0;
    int   i_NumOfMissingFiles = 0;
    int   i_numOfChecks = 0;

    QStringList sl_FilenameList;

// **********************************************************************************************

    err = doDownloadManagerDialog( gs_DownloadPath, gs_FTPServer, gs_User, gs_Password, gb_DecompressFiles,
                                   gb_CheckFiles, gb_CheckAvailability, gb_Station, gb_Month, gb_Year );

    if ( err == QDialog::Accepted )
    {
        sl_FilenameList = downloadStationToArchiveFiles( g_Station_ptr, gs_DownloadPath, gs_FTPServer, gs_User, gs_Password,
                                                         gb_DecompressFiles, gb_CheckFiles, gb_CheckAvailability,
                                                         gb_Station, gb_Month, gb_Year, i_NumOfEmptyFiles, i_NumOfMissingFiles,i_numOfChecks  );


        //QString miss=QString("cecks%1 empty%2  missing%3 Files have been downloaded").arg(i_numOfChecks).arg(i_NumOfEmptyFiles).arg(i_NumOfMissingFiles);
        //QMessageBox::information( this, getApplicationName( true ), miss );

        checkScriptResults( sl_FilenameList , i_NumOfEmptyFiles, i_NumOfMissingFiles,i_numOfChecks, gb_CheckAvailability);
    }

// **********************************************************************************************

    endTool( _NOERROR_, _NOERROR_, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Download of files was canceled" ), false, false );

    onError( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-08-13

void MainWindow::checkScriptResults( const QStringList &sl_FilenameList, int i_NumOfEmptyFiles, int i_NumOfMissingFiles, int i_NumOfChecks, const  bool b_CheckAvailability )
{
    int         i_NumOfFiles        = 0;
    //int         i_NumOfEmptyFiles   = 0;
    //int         i_NumOfMissingFiles = 0;
    int         diff                = -1;

    bool        b_OK                = true;

    QString     s_Message           = "";

// **********************************************************************************************

    i_NumOfFiles = sl_FilenameList.count();
    diff = i_NumOfChecks-i_NumOfFiles;

    if(b_CheckAvailability==false) {

    if( diff==0 && i_NumOfChecks>0 )

     {

         //s_Message = tr( "All files  have been downloaded." );
         QString succ=QString("All files (%1 from %2) have been downloaded.").arg(i_NumOfFiles).arg(i_NumOfChecks);
         s_Message = succ;

     }
    else if( diff==0 && i_NumOfChecks==0 )

     {

         //s_Message = tr( "All files  have been downloaded." );
         QString succ=QString("No Checks - No Result\r\nPlease check your settings!").arg(i_NumOfFiles).arg(i_NumOfChecks);
         s_Message = succ;

     }
    else if(diff==i_NumOfChecks)
    {

         s_Message.append( tr( "No file has been downloaded.\r\nPlease also check your account!" ) );
     }
    else
    {

        QString miss=QString("%1 from %2 possible files has/have been downloaded.\r\nBefore calling the support, please check the availability of files!").arg(i_NumOfFiles).arg(i_NumOfChecks);
        s_Message = miss;
        b_OK = false;
    }

    }  //b_CheckAvailability=false
    else {

            if( diff==0 && i_NumOfChecks>0 )
            {

                //s_Message = tr( "All files have been downloaded." );
                QString succ=QString("All station filelists (%1 from %2) have been downloaded.").arg(i_NumOfFiles).arg(i_NumOfChecks);
                s_Message = succ;

            }
            else if( diff==0 && i_NumOfChecks==0 )

             {

                 //s_Message = tr( "All files have been downloaded." );
                 QString succ=QString("No Checks - No Result\r\nPlease check your settings!").arg(i_NumOfFiles).arg(i_NumOfChecks);
                 s_Message = succ;

             }
            else if(diff==i_NumOfChecks)
            {

                s_Message.append( tr( "No station filelist has been downloaded.\r\nPlease also check your account!" ) );
            }
            else
            {

            QString miss=QString("%1 from %2 possible station filelists has/have been downloaded.\r\nBefore calling the support, please check the availability of files!").arg(i_NumOfFiles).arg(i_NumOfChecks);
            s_Message = miss;
            b_OK = false;
            }
    }


    switch ( i_NumOfEmptyFiles )
    {
    case 0:
        break;

    case 1:
        s_Message.append( tr( "\r\nIn addition: One file is empty. " ) );
        b_OK = false;
        break;

    default:
        s_Message.append( tr( "\r\nIn addition: Some files are empty. " ) );
        b_OK = false;
        break;
    }



QMessageBox::information( this, getApplicationName( true ), s_Message );


////////////////////////////////////old  parts
    /*for ( int i=0; i<i_NumOfFiles; i++ )
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
    }*/

    //if ( b_OK == true )
    //{
      /* switch ( i_NumOfFiles )

        {
        case 0:
            s_Message.append( tr( "No file has been downloaded. Please also check your account!" ) );
            break;

        case 1:
            s_Message = tr( "File has been downloaded." );
            break;

        default:
            s_Message = tr( "All files have been downloaded." );
            break;
        }*/
    //}




    /*else
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
        }*/


}
