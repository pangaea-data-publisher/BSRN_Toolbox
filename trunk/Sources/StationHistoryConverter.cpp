/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Liest den Record 0007 und speichert ihn in einer Datei.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::StationHistoryConverter( const QString& s_FilenameIn, QStringList& sl_FilenameOut, structStation *Station_ptr, const int i_NumOfFiles )
{
    int				i_Year			= 2000;
    int				i_Month			= 1;
    int				i_Day			= 1;
    int				i_Minute		= 1;
    int				i_StationNumber	= 0;

    QString			InputStr		= "";

    QString			SearchString1	= "*C0007";
    QString			SearchString2	= "*U0007";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    if ( ( fi.exists() == false ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        return( _FILENOEXISTS_ );

// ***********************************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );

    ui_filesize = fin.size();

// ***********************************************************************************************************************

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Station history converter working ..." ), 100 );

// ***********************************************************************************************************************

    InputStr  = tin.readLine();
    ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

    if ( ( InputStr.startsWith( "*C0001" ) == false ) && ( InputStr.startsWith( "*U0001" ) == false ) )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -40 );
    }

// ***********************************************************************************************************************

    InputStr  = tin.readLine();
    ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

    i_StationNumber	= InputStr.left( 3 ).toInt();
    s_StationName	= findStationName( i_StationNumber, Station_ptr );
    s_EventLabel	= findEventLabel( i_StationNumber, Station_ptr );

// ***********************************************************************************************************************

    i_Day   = 1;
    i_Month	= InputStr.mid( 4, 2 ).toInt();
    i_Year	= InputStr.mid( 7, 4 ).toInt();

    QDateTime dt = QDateTime().toUTC();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );

// ***********************************************************************************************************************

    if ( checkFilename( fi.fileName(), s_EventLabel, InputStr.mid( 4, 2 ), InputStr.mid( 9, 2 ) ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -41 );
    }

// ***********************************************************************************************************************

    QFile fout( fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0007.txt" );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

    appendItem( sl_FilenameOut, fout.fileName() );

// ***********************************************************************************************************************
// LR0007

    tout << "File name\tStation ID\tEvent label\tStation\tYYYY-MM\t";
    tout << "Date/Time when change occured\tMethod est. cloud amount (digital proc.)\t";
    tout << "Method est. cloud height (with instrument)\tMethod est. cloud liquid water cont.\t";
    tout << "Method est. cloud aerosol vertical distr.\tMethod est. water vapor pressure v. d.\t";
    tout << "SYNOP flags" << endl;

    tout << s_EventLabel.toLower() << dt.toString( "MMyy" ) << ".dat" << "\t";
    tout << i_StationNumber << "\t" << s_EventLabel << "\t" << s_StationName << "\t";
    tout << dt.toString( "yyyy-MM" );

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( InputStr != " -1 -1 -1" )
            {
                i_Day		= InputStr.mid( 1, 2 ).toInt();
                i_Minute	= InputStr.mid( 4, 4 ).toInt();

                dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                dt.setTime( QTime( 0, 0, 0 ) );
                dt = dt.addSecs( i_Minute*60 );

                tout << "\t" << dt.toString( "yyyy-MM-ddThh:mm" );
            }
            else
                tout << "\t";

            InputStr  = tin.readLine().simplified().replace( "XXX", "" );
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr;
            else
                tout << "\t";

            InputStr  = tin.readLine().simplified().replace( "XXX", "" );
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr;
            else
                tout << "\t";

            InputStr  = tin.readLine().simplified().replace( "XXX", "" );
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr;
            else
                tout << "\t";

            InputStr  = tin.readLine().simplified().replace( "XXX", "" );
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr;
            else
                tout << "\t";

            InputStr  = tin.readLine().simplified().replace( "XXX", "" );
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr;
            else
                tout << "\t";

            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if (InputStr.isEmpty() == false )
                tout << "\t" << InputStr << endl;
            else
                tout << endl;

            b_Stop = true;
        }

        // Abort if first data record reached
        if ( ( InputStr.startsWith( "*C0100" ) == true ) || ( InputStr.startsWith( "*U0100" ) == true ) )
            b_Stop = true;
    }

//---------------------------------------------------------------------------------------------------

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Station History Converters, LR0007 */

void MainWindow::doStationHistoryConverter()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QStringList sl_FilenameOut;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Station history converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = StationHistoryConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );

        if ( err == _NOERROR_ )
            err = concatenateFiles( gs_Path + "/" + "BSRN_LR0007.txt", sl_FilenameOut, tr( "Building LR0007 file..." ), 1, true );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Station history converter was canceled" ), false, false );

    onError( err );
}

