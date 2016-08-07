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
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QString s_Program       = "";

    QStringList sl_FilenameOut;

    QProcess process;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        #if defined(Q_OS_LINUX)
            s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck" );
        #endif

        #if defined(Q_OS_WIN)
            s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck.exe" );
        #endif

        #if defined(Q_OS_MAC)
            s_Program = QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" + "BSRN_fcheck" );
        #endif

        QFileInfo fi( s_Program );

        if ( fi.exists() == false )
        {
            #if defined(Q_OS_LINUX)
                err = -30;
            #endif

            #if defined(Q_OS_WIN)
                err = -31;
            #endif

            #if defined(Q_OS_MAC)
                err = -32;
            #endif
        }
        else
        {
            initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Checking station-to-archive files ..." ) );

            while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
            {
                setStatusBar( tr( "Checking " ) + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + tr( " ..." ) );

                process.start( "\"" + s_Program + "\" \"" + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + "\"" );
                process.waitForFinished();

                appendItem( sl_FilenameOut, gsl_FilenameList.at( i ), ".dat", ".rep" );

                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }

            resetFileProgress( gsl_FilenameList.count() );

// ************************************************************************************************
// Concatenate report files

            if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
            {
                QFile frep( gs_Path + "BSRN_fcheck_reports.txt" );
                if ( frep.exists() == true )
                    frep.remove();

                err = concatenateFiles( gs_Path + "/" + "BSRN_fcheck_reports.txt", sl_FilenameOut, tr( "Building report file ..." ), 0, true );
            }
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Check of station-to-archive file was canceled" ), false, false );

    onError( err );
}
