/* CreateReplaceDatabase.cpp */
/* 2015-07-05                */
/* Dr. Rainer Sieger         */

#include "Application.h"

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doCreateReplaceDatabase()
{
    int		err							= _NOERROR_;
    int		i							= 0;
    int		stopProgress				= 0;

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == true )
        return;

    initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Creating search and replace database..." ) );

    while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
    {
        err = createReplaceDatabase( gsl_FilenameList.at( i ), true, _MACOS_ );
        stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
    }

    resetFileProgress( gsl_FilenameList.count() );

    if ( stopProgress == _APPBREAK_ )
        err = _APPBREAK_;

    if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        setStatusBar( tr( "Done" ), 2 );
    else
        setStatusBar( tr( "Creating search and replace database was canceled" ), 2 );

    onError( err );
}
