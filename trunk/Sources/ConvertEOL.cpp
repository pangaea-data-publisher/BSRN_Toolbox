/* convertEOL.cpp			*/
/* 2008-12-19               */
/* Dr. Rainer Sieger        */

#include "Application.h"

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

int MainWindow::convertEOL( const QString &s_FilenameIn, const bool b_convertEOL, const int i_OS )
{
    bool	b_isWindowsEOL	= false;
    bool	b_isMacOS9EOL	= false;
    bool	b_isAllOSEOL	= false;

// **********************************************************************************************

    if ( b_convertEOL == true )
    {
        QFileInfo fi( s_FilenameIn );

        QFile fin( s_FilenameIn );
        if ( !fin.open( QIODevice::ReadOnly ) )
            return( -10 );

        QByteArray ba = fin.read( 10000 );

        switch ( i_OS )
        {
        case _WIN_:
            if ( ba.contains( "\r\n" ) == true ) // Windows
                b_isWindowsEOL = true;
            break;

        case _MACOS_:
            if ( ( ba.contains( "\r" ) == true ) && ( ba.contains( "\n" ) == false ) )  // MacOS 9
                b_isMacOS9EOL = true;
            break;

        case _WINMAC_:
            if ( ba.contains( "\r" ) == true ) // Windows or MacOS 9
                b_isAllOSEOL  = true;
            break;

        default:
            break;
        }

        if ( ( b_isAllOSEOL == true ) || ( b_isWindowsEOL == true ) || ( b_isMacOS9EOL == true ) )
        {
            fin.reset();
            ba.clear();
            ba.resize( fin.size() );
            ba = fin.readAll();
        }

        fin.close();

// **********************************************************************************************

        if ( b_isAllOSEOL == true )
        {
            QFile fout( fi.absolutePath() + "/~" + fi.baseName() );
            if ( !fout.open( QIODevice::WriteOnly ) ) // | QIODevice::Text ) )
                return( -20 );

            ba.replace( "\r\n", "\n" ); // Windows -> Unix
            ba.replace( "\r", "\n" ); // MacOS -> Unix

            fout.write( ba );

            fout.close();

            fin.remove();
            fout.rename( s_FilenameIn );
        }

// **********************************************************************************************

        if ( b_isWindowsEOL == true )
        {
            QFile fout( fi.absolutePath() + "/~" + fi.baseName() );
            if ( !fout.open( QIODevice::WriteOnly ) ) // | QIODevice::Text ) )
                return( -20 );

            ba.replace( "\r\n", "\n" ); // Windows -> Unix

            fout.write( ba );

            fout.close();

            fin.remove();
            fout.rename( s_FilenameIn );
        }

// **********************************************************************************************

        if ( b_isMacOS9EOL == true )
        {
            QFile fout( fi.absolutePath() + "/~" + fi.baseName() );
            if ( !fout.open( QIODevice::WriteOnly ) ) // | QIODevice::Text ) )
                return( -20 );

            ba.replace( "\r", "\n" ); // MacOS 9 -> Unix

            fout.write( ba );

            fout.close();

            fin.remove();
            fout.rename( s_FilenameIn );
        }
    }

// **********************************************************************************************

    return( _NOERROR_ );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doWinConvertEOL()
{
    int		err							= _NOERROR_;
    int		i							= 0;
    int		stopProgress				= 0;

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == false )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Converting end-of-line..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
        {
            err = convertEOL( gsl_FilenameList.at( i ), true, _WIN_ );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// *************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Converting end-of-line was canceled" ), false, false );

    onError( err );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doMacOSConvertEOL()
{
    int		err							= _NOERROR_;
    int		i							= 0;
    int		stopProgress				= 0;

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == true )
        return;

    initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Converting end-of-line..." ) );

    while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
    {
        err = convertEOL( gsl_FilenameList.at( i ), true, _MACOS_ );
        stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
    }

    resetFileProgress( gsl_FilenameList.count() );

    if ( stopProgress == _APPBREAK_ )
        err = _APPBREAK_;

    if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        setStatusBar( tr( "Done" ), 2 );
    else
        setStatusBar( tr( "Converting end-of-line was canceled" ), 2 );

    onError( err );
}
