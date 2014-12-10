/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-09-27

/*! @brief Komprimieren von Dateien mit gzip. Gzip
*   muss sich im gleichen Verzeichnis befinden wie das Programm "BSRN Toolbox".
*/

void MainWindow::doCompressFile()
{
    int		i                   = 0;
    int		err                 = 0;
    int		stopProgress        = 0;

    QString s_GZipProgramName   = "";

    QProcess process;

// **********************************************************************************************

    #if defined(Q_OS_LINUX)
        s_GZipProgramName = "gzip";
    #endif

    #if defined(Q_OS_WIN)
        s_GZipProgramName = "\"" + QDir::toNativeSeparators( QCoreApplication::applicationDirPath() + "/" ) + "gzip.exe" + "\"";
    #endif

    #if defined(Q_OS_MAC)
        s_GZipProgramName = "gzip";
    #endif

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Compressing files ..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            setStatusBar( tr( "Compress " ) + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + tr( " ..." ) );

            process.start( s_GZipProgramName + " \"" + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + "\"" );
            process.waitForFinished();

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Compress files was canceled" ), true );

    onError( err );
}
