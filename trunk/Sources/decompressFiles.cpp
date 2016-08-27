/* 2016-11-03                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-01-04

void MainWindow::doDecompressFiles()
{
    int		  i              = 0;
    int		  err            = 0;
    int       stopProgress   = 0;

    QString   s_ProgramUnzip = "";
    QString   s_ProgramGzip  = "";

    QFileInfo fi;

// **********************************************************************************************

    s_ProgramUnzip = findUnzip( _ZIP_ );
    s_ProgramGzip  = findUnzip( _GZIP_ );

    if ( ( s_ProgramUnzip != "Unzip not found" ) && ( s_ProgramGzip != "Unzip not found" ) )
    {
        existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );

        if ( gsl_FilenameList.count() > 0 )
        {
            initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Decompressing files ..." ) );

            while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
            {
                setStatusBar( tr( "Decompress " ) + QDir::toNativeSeparators( gsl_FilenameList.at( i ) ) + tr( " ..." ) );

                fi.setFile( gsl_FilenameList.at( i ) );

                if ( fi.suffix().toLower() == "zip" )
                    err = decompressFile( gsl_FilenameList.at( i ), false, _ZIP_, s_ProgramUnzip );

                if ( fi.suffix().toLower() == "gz" )
                    err = decompressFile( gsl_FilenameList.at( i ), false, _GZIP_, s_ProgramGzip );

                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }

            resetFileProgress( gsl_FilenameList.count() );
        }
        else
        {
            err = _CHOOSEABORTED_;
        }
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Decompressing files was canceled" ), true );

    onError( err );
}
