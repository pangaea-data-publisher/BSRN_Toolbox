/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-09-27

/*! @brief Komprimieren von Dateien mit gzip. Unter Windows muss sich das Programm gzip.exe
*   im gleichen Verzeichnis befinden wie das Programm "BSRN Toolbox".
*/

void MainWindow::doCompressFile()
{
    int		i              = 0;
    int		err            = 0;
    int		stopProgress   = 0;

    QString s_Program      = "";

// **********************************************************************************************

    #if defined(Q_OS_LINUX)
        s_Program = "gzip";
    #endif

    #if defined(Q_OS_MAC)
        s_Program = "gzip";
    #endif

    #if defined(Q_OS_WIN)
        s_Program = QCoreApplication::applicationDirPath() + "/" + "gzip.exe";
    #endif

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Compressing files ..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            setStatusBar( tr( "Compress " ) + gsl_FilenameList.at( i ) + tr( " ..." ) );

            compressFile( gsl_FilenameList.at( i ), _GZIP_, s_Program );

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
