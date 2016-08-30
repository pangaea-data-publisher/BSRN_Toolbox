/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-09-27

/*! @brief Testet eine Liste von Dateien nacheinander mit dem Programm "BSRN_fcheck.exe". Dieses
*   Programm muss sich im gleichen Verzeichnis befinden wie das Programm "BSRN Toolbox".
*/

void MainWindow::doCheckStationToArchiveFiles()
{
    int         i				= 0;
    int         err				= 0;
    int         stopProgress	= 0;

    QString     s_BSRN_fcheck   = "";
    QString     s_arg           = "";

    QStringList sl_FilenameOut;

    QProcess    process;

// **********************************************************************************************

    #if defined(Q_OS_LINUX)
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
    #endif

    #if defined(Q_OS_MAC)
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck\"" );
//      s_BSRN_fcheck = "\"/Users/rsieger/Development/GitHub/BSRN_Toolbox/trunk/Tools/f_check/BSRN_fcheck_MacOS\"";
    #endif

    #if defined(Q_OS_WIN)
        s_BSRN_fcheck = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck.exe\"" );
    #endif

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Checking station-to-archive files ..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
        {
            setStatusBar( tr( "Checking " ) + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + tr( " ..." ) );

            s_arg = s_BSRN_fcheck;
            s_arg.append( " \"" + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + "\"" );

            process.start( s_arg );
            process.waitForFinished( -1 );

            appendItem( sl_FilenameOut, gsl_FilenameList.at( i ), ".dat", ".rep" );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );

// ************************************************************************************************
// Concatenate report files

        if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
            err = concatenateFiles( gs_Path + "/" + "BSRN_fcheck_report.txt", sl_FilenameOut, tr( "Building report file ..." ), 0, true );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Check of station-to-archive file was canceled" ), false, false );

    onError( err );
}
