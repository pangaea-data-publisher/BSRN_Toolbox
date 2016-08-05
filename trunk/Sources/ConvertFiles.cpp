/* ConvertFiles.cpp			*/
/* 2009-06-17               */
/* Dr. Rainer Sieger        */

#include "Application.h"

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************
// 2016-06-09
// remove Latitude and Longitude from Quality Check output file

int MainWindow::convertFile( const QString& s_FilenameIn, const int i_NumOfFiles )
{
    int         i             = 0;
    int         n             = 0;
    int         m             = 0;
    int         s             = 0;

    int         i_PosPosition = -1;
    int         i_PosSWD      = -1;
    int         i_PosSumSW    = -1;

    QString     s_FilenameOut = s_FilenameIn;
    QString     s_FilenameSDW = s_FilenameIn;

    QStringList sl_Input;

// ************************************************************************************************

    s_FilenameOut.replace( "_temp", "" );
    s_FilenameSDW.replace( "_temp", "_SDW_SumSW" );

    QFile::remove( s_FilenameOut );
    QFile::remove( s_FilenameSDW );

// ************************************************************************************************

    n = readFile( s_FilenameIn, sl_Input, _SYSTEM_, i_NumOfFiles );

// ***********************************************************************************************************************

    setStatusBarFileInProgress( s_FilenameIn );

    if ( sl_Input.at( 0 ) == "/* DATA DESCRIPTION:" )
    {
        while ( ( sl_Input.at( i ).contains( "*/" ) == false ) && ( i < n ) )
            i++;

        if ( i == n )
            return( _NODATAFOUND_ );

        m = NumOfSections( sl_Input.at( ++i ) ) - 1;
        s = i;

// ***********************************************************************************************************************
// Output of SWD and SumSW

        i = s;

        for ( int j=0; j<m; j++ )
        {
            if ( sl_Input.at( i ).section( "\t", j, j ).toLower() == "swd [w/m**2]" )
            {
                i_PosSWD = j;
                break;
            }
        }

        for ( int j=0; j<m; j++ )
        {
            if ( sl_Input.at( i ).section( "\t", j, j ).toLower() == "sumsw [w/m**2]" )
            {
                i_PosSumSW = j;
                break;
            }
        }

        if ( ( i_PosSWD > -1 ) && ( i_PosSumSW > -1 ) )
        {
            QFile::remove( s_FilenameSDW );

            QFile foutSWD( s_FilenameSDW );

            if ( foutSWD.open( QIODevice::WriteOnly | QIODevice::Text) == false )
                return( -20 );

            QTextStream toutSWD( &foutSWD );

            i = s;

            while ( i < n )
            {
                if ( ( sl_Input.at( i ).section( "\t", i_PosSWD, i_PosSWD ).isEmpty() == false ) && ( sl_Input.at( i ).section( "\t", i_PosSumSW, i_PosSumSW ).isEmpty() == false ) )
                {
//                  toutSWD << sl_Input.at( i ).section( "\t", 0, 0 ) << "\t";                     // Date/Time
                    toutSWD << sl_Input.at( i ).section( "\t", i_PosSWD, i_PosSWD ) << "\t";       // SWD
                    toutSWD << sl_Input.at( i ).section( "\t", i_PosSumSW, i_PosSumSW ) << endl;   // SumSW
                }

                i++;
            }

            foutSWD.close();
        }

// ***********************************************************************************************************************
// Output of data whitout position

        i = s;

        for ( int j=0; j<m; j++ )
        {
            if ( ( sl_Input.at( i ).section( "\t", j, j ).toLower() == "latitude" ) && ( sl_Input.at( i ).section( "\t", j+1, j+1 ).toLower() == "longitude" ) )
            {
                i_PosPosition = j;
                break;
            }
        }

        if ( i_PosPosition > -1 )
        {
            QFile fout( s_FilenameOut );

            if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
                return( -20 );

            QTextStream tout( &fout );

            i = 0;

            while ( ( sl_Input.at( i ).contains( "*/" ) == false ) && ( i < n ) )
            {
                if ( ( sl_Input.at( i ).contains( tr("Latitude") ) == false ) && ( sl_Input.at( i ).contains( tr("Longitude") ) == false ) )
                    tout << sl_Input.at( i ) << endl;

                i++;
            }

            tout << sl_Input.at( i++ ) << endl;

            i = s;

            while ( i < n )
            {
                tout << sl_Input.at( i ).section( "\t", 0, i_PosPosition-1 ) << "\t";
                tout << sl_Input.at( i ).section( "\t", i_PosPosition+2, m ) << endl;

                i++;
            }

            fout.close();

            QFile::remove( s_FilenameIn );
        }
        else
        {
            QFile::remove( s_FilenameOut );
            QFile::rename( s_FilenameIn, s_FilenameOut );
        }


    }

    return( _NOERROR_ );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************
// unformated

int MainWindow::convertFile( const QString& s_FilenameIn, const QString& s_FilenameOut, const QString& s_MissingValue, const int i_FieldDelimiter, const int i_NumOfFiles )
{
    int   n             = 0;
    int	  stopProgress	= 0;

    unsigned int	ui_length	= 1;
    unsigned int	ui_filesize = 1;

    QString InputStr            = "";
    QString s_FieldDelimiter    = "";

// ************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );

    ui_filesize = fin.size();

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

// ***********************************************************************************************************************

    QTextStream tin( &fin );
    QTextStream tout( &fout );

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Tab converter working..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn );

// ***********************************************************************************************************************
// field delimiter

    switch ( i_FieldDelimiter )
    {
        case _COMMA_:
            s_FieldDelimiter = ",";
            break;
        case _SEMICOLON_:
            s_FieldDelimiter = ";";
            break;
        case _SPACE_:
            s_FieldDelimiter = " ";
            break;
        default:
            s_FieldDelimiter = "\t";
            break;
    }

// ***********************************************************************************************************************
// Header

    InputStr  = tin.readLine();
    ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
    n         = NumOfSections( InputStr );

    tout << "\"" << InputStr.section( "\t", 0, 0 ) << "\"";

    for ( int i=1; i<n; ++i )
        tout << s_FieldDelimiter << "\"" << InputStr.section( "\t", i, i ) << "\"";

    tout << endl;

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
        n         = NumOfSections( InputStr );

        if ( InputStr.isEmpty() == false )
        {
            if ( InputStr.section( "\t", 0, 0 ).isEmpty() == true )
                tout << s_MissingValue;
            else
                tout << InputStr.section( "\t", 0, 0 );

            for ( int i=1; i<n; ++i )
            {
                if ( InputStr.section( "\t", i, i ).isEmpty() == true )
                    tout << s_FieldDelimiter << s_MissingValue;
                else
                    tout << s_FieldDelimiter << InputStr.section( "\t", i, i );
            }
        }

        tout << endl;
    }

// *************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    if ( stopProgress == _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************
// formated

int MainWindow::convertFile( const QString& s_FilenameIn, const QString& s_FilenameOut, const int i_FieldAlignment, const int i_FieldWidth, const QString& s_MissingValue, const int i_NumOfFiles )
{
    int   i             = 0;
    int   n             = 0;
    int   m             = 0;

    int	  stopProgress	= 0;

    unsigned int	ui_filesize = 1;

    QString         InputStr    = "";

    QStringList     sl_Input;


// ************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( ( n = readFile( s_FilenameIn, sl_Input, _SYSTEM_ ) ) < 1 )
        return( -10 );

    ui_filesize = fin.size();

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

// ***********************************************************************************************************************

    QTextStream tout( &fout );

    switch ( i_FieldAlignment )
    {
        case QTextStream::AlignLeft:
            tout.setFieldAlignment( QTextStream::AlignLeft );
            break;
        case QTextStream::AlignRight:
            tout.setFieldAlignment( QTextStream::AlignRight );
            break;
    }

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Tab converter working..." ), sl_Input.count() );

    setStatusBarFileInProgress( s_FilenameIn );

// ***********************************************************************************************************************

    while ( ( i<n ) && ( stopProgress != _APPBREAK_ ) )
    {
        InputStr  = sl_Input.at( i );

        m = NumOfSections( InputStr );

        if ( InputStr.isEmpty() == false )
        {
            tout.setFieldWidth( i_FieldWidth );

            tout << InputStr.section( "\t", 0, 0 );

            for ( int j=1; j<m; ++j )
            {
                if ( InputStr.section( "\t", j, j ).isEmpty() == true )
                    tout << " " + s_MissingValue;
                else
                    tout << " " + InputStr.section( "\t", j, j );
            }
        }

        tout.setFieldWidth( 0 );
        tout << endl;

        stopProgress = incProgress( i_NumOfFiles, ++i );
    }

// *************************************************************************************

    resetProgress( i_NumOfFiles );

    fout.close();

    if ( stopProgress == _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doConvertUnformatted()
{
    int		err							= _NOERROR_;
    int		i							= 0;
    int		stopProgress				= 0;

    QString s_FilenameOut               = "";

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == false )
    {
        err = doFormatUnformattedOptionsDialog( gs_MissingValue, gi_FieldDelimiter );

        if ( err == QDialog::Accepted )
        {
            initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Converting tab files..." ) );

            err = _NOERROR_;

            while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
            {
                QFileInfo fi( gsl_FilenameList.at( i ) );

                s_FilenameOut = fi.absolutePath() + "/" + "zz_" + fi.baseName() + ".txt";

                err = convertFile( gsl_FilenameList.at( i ), s_FilenameOut, gs_MissingValue, gi_FieldDelimiter, gsl_FilenameList.count() );
                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }

            resetFileProgress( gsl_FilenameList.count() );
        }
        else
        {
            err = _CHOOSEABORTED_;
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

    if ( stopProgress == _APPBREAK_ )
        err = _APPBREAK_;

    if ( ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        setStatusBar( tr( "Done" ), 2 );
    else
        setStatusBar( tr( "Converting tab files was canceled" ), 2 );

    onError( err );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doConvertFormatted()
{
    int     i              = 0;
    int		err            = _NOERROR_;
    int		stopProgress   = 0;

    QString s_FilenameOut  = "";

// *************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == false )
    {
        err = doFormatFormattedOptionsDialog( gi_FieldAlignment, gi_FieldWidth, gs_MissingValue );

        if ( err == QDialog::Accepted )
        {
            initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Converting tab files..." ) );

            err = _NOERROR_;

            while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
            {
                QFileInfo fi( gsl_FilenameList.at( i ) );

                s_FilenameOut = fi.absolutePath() + "/" + "zz_" + fi.baseName() + ".txt";

                err = convertFile( gsl_FilenameList.at( i ), s_FilenameOut, gi_FieldAlignment, gi_FieldWidth, gs_MissingValue, gsl_FilenameList.count() );

                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }

            resetFileProgress( gsl_FilenameList.count() );
        }
        else
        {
            err = _CHOOSEABORTED_;
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// *************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Converting tab files was canceled" ), false, false );

    onError( err );
}
