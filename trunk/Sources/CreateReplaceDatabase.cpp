/* CreateReplaceDatabase.cpp */
/* 2015-07-05                */
/* Dr. Rainer Sieger         */

#include <QInputDialog>
#include <QMessageBox>

#include "Application.h"

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

int MainWindow::createReplaceDatabase( const QString &s_FilenameIn, const int i_fristReferenceID, const int i_NumOfFiles )
{
    QString			InputStr		= "";

    int             i_ReferenceID   = i_fristReferenceID;

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    if ( ( fi.exists() == false ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        return( _FILENOEXISTS_ );

    if ( i_fristReferenceID < 1 )
        return( _APPBREAK_ );

// ***********************************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );

    ui_filesize = fin.size();

// ***********************************************************************************************************************

    QFile fout( fi.absolutePath() + "/" + "Station-to-archive_References.rdb" );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

// ***********************************************************************************************************************

    QTextStream tin( &fin );
    QTextStream tout( &fout );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Creating search and replace database..." ), 100 );

// ***********************************************************************************************************************

    InputStr  = tin.readLine();
    ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
    {
        InputStr = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        tout << InputStr.section( "\t", 3, 3 ) << "\t" << QString( "%1" ).arg( i_ReferenceID++ ) << eol;
    }

// ***********************************************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( i_ReferenceID );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doCreateReplaceDatabase()
{
    int		err							= _NOERROR_;

    int     i_firstReferenceID          = 0;
    int     i_lastReferenceID           = 0;

    bool    b_OK                        = false;

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFile();

    if ( gsl_FilenameList.isEmpty() == true )
        err = _ERROR_;

// *************************************************************************************

    i_firstReferenceID = QInputDialog::getInt( this, "Set reference ID", "Give the ID of the first reference\nof BSRN station-to-archive file:\n", gi_lastReferenceID, 41804, 999999, 10, &b_OK );

    if ( b_OK == false )
        err = _APPBREAK_;

// *************************************************************************************

    if ( err == _NOERROR_ )
    {
        i_lastReferenceID = createReplaceDatabase( gsl_FilenameList.at( 0 ), i_firstReferenceID, 1 );

        if ( ( i_lastReferenceID > gi_lastReferenceID ) && ( i_lastReferenceID > _APPBREAK_ ) )
            gi_lastReferenceID = i_lastReferenceID;

        if ( i_lastReferenceID <= _APPBREAK_ )
            err = _ERROR_;
    }

// *************************************************************************************

    if ( err == _NOERROR_ )
    {
        setStatusBar( tr( "Done" ), 2 );

        QMessageBox::information( this, getApplicationName( true ), tr( "The search and replace database\n\n    \"Station-to-archive_References.rdb\"\n\nhas been created. Use this file together with PanTool for replacing the reference IDs in BSRN import files." ) );
    }
    else
    {
        setStatusBar( tr( "Creating search and replace database was canceled" ), 2 );
    }

    onError( err );
}
