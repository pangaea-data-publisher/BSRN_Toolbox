/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Liest den Record 0006 und speichert ihn in einer Datei.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::OzoneEquipmentConverter( const QString& s_FilenameIn, QStringList& sl_FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int i_NumOfFiles )
{
    int				i_Year			= 2000;
    int				i_Month			= 1;
    int				i_Day			= 1;
    int				i_StationNumber	= 0;
    int				i_MethodID		= 0;

    QString			InputStr		= "";

    QString			s_OzoneInstrumentIdentification = "";

    QString			SearchString1	= "*C0006";
    QString			SearchString2	= "*U0006";
    QString			SearchString3	= "*C0007";
    QString			SearchString4	= "*U0007";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;

// ***********************************************************************************************************************

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    if ( ( fi.exists() == false ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        return( -1 );

// ***********************************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

// ***********************************************************************************************************************

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Ozone equipment converter working ..." ), 100 );

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

    QFile fout( fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0006.txt" );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

    appendItem( sl_FilenameOut, fout.fileName() );

// ***********************************************************************************************************************
// LR 0006

    tout << "File name\tStation ID\tEvent label\tStation\tYYYY-MM\t";
    tout << "Manufacturer\tLocation\tDistance from radiation site [km]\t";
    tout << "Identification number of ozone instrument\tRemarks\tPANGAEA Method ID" << endl;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            tout << s_EventLabel.toLower() << dt.toString( "MMyy" ) << ".dat" << "\t";
            tout << i_StationNumber << "\t" << s_EventLabel << "\t" << s_StationName << "\t" << dt.toString( "yyyy-MM" ) << "\t";

            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( InputStr.simplified().right( 1 ) == "Y" )
            {
                InputStr  = tin.readLine();
                ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                tout << InputStr.left( 30 ).simplified() << "\t";		// manufacturer
                tout << InputStr.mid( 30, 25 ).simplified() << "\t";	// location
                tout << InputStr.mid( 57, 3 ).simplified() << "\t";	// distance from radiation site [km]
                tout << InputStr.mid( 61, 5 ).simplified() << "\t";	// identification number of ozone instrument

                s_OzoneInstrumentIdentification = InputStr.left( 30 ).simplified();

                if ( InputStr.mid( 61, 5 ).simplified().isEmpty() == false )
                    s_OzoneInstrumentIdentification += ", " + InputStr.mid( 61, 5 ).simplified();

                i_MethodID = findMethodID( s_OzoneInstrumentIdentification, Method_ptr );

                InputStr  = tin.readLine();
                ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                tout << InputStr.simplified().replace( "XXX", "" ) << "\t" << i_MethodID << endl;
            }

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

/*! @brief Steuerung des Ozone Equipment Converters, LR 0006 */

void MainWindow::doOzoneEquipmentConverter()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QStringList sl_FilenameOut;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Ozone equipment converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = OzoneEquipmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );

        if ( err == _NOERROR_ )
            err = concatenateFiles( gs_Path + "/" + "BSRN_LR0006.txt", sl_FilenameOut, tr( "Building LR0006 file..." ), 1, true );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Ozone equipment converter was canceled" ), false, false );

    onError( err );
}
