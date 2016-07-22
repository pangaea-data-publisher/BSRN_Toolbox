/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Liest den Record 0008 und speichert ihn in einer Datei.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::RadiationInstrumentsConverter(const QString& s_FilenameIn, QStringList& sl_FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int i_NumOfFiles )
{
    int				i_Year			= 2000;
    int				i_Month			= 1;
    int				i_Day			= 1;
    int				i_StationNumber	= 0;
    int				i_MethodID		= 0;

    QString			InputStr		= "";

    QString			SearchString1	= "*C0008";
    QString			SearchString2	= "*U0008";

    QString			s_StationName		= "";
    QString			s_EventLabel		= "";
    QString			s_Identification	= "";
    QString			s_Device			= "";

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

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Radiation instruments converter working ..." ), 100 );

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

    QFile fout( fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0008.txt" );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

    appendItem( sl_FilenameOut, fout.fileName() );

// ***********************************************************************************************************************
// LR0008

    tout << "File name\tStation ID\tEvent label\tStation\tYYYY-MM\t";
    tout << "PANGAEA method\tPANGAEA method ID" << endl;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( ( InputStr.startsWith( "*" ) == false ) )
                {
                    // Pyranometer, Kipp & Zonen, CM11, Serial No. 924048, WRMC no. 1001

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    s_Device = InputStr.left( 30 ).simplified(); // Manufacturer
                    s_Device.append( tr( ", " ) + InputStr.mid( 31, 15 ).simplified() ); // Model
                    s_Device.append( tr( ", SN " ) + InputStr.mid( 47, 18 ).simplified() );	// Serial number

                    if ( s_Device.contains( "PIR" ) == true )
                        s_Device = "Pyrgeometer, " + s_Device;

                    if ( s_Device.contains( "NIP" ) == true )
                        s_Device = "Pyrheliometer, " + s_Device;

                    if ( s_Device.contains( "MS-52" ) == true )
                        s_Device = "Pyrheliometer, " + s_Device;

                    if ( s_Device.contains( "MS-53" ) == true )
                        s_Device = "Pyrheliometer, " + s_Device;

                    if ( s_Device.contains( "CH1" ) == true )
                        s_Device = "Pyrheliometer, " + s_Device;

                    if ( s_Device.contains( "CM11" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "CM21" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "CM22" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "CM31" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "SR-75" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "M-115M" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "MS-43" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "MS-801" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "8-48" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    if ( s_Device.contains( "PSP" ) == true )
                        s_Device = "Pyranometer, " + s_Device;

                    tout << s_EventLabel.toLower() << dt.toString( "MMyy" ) << ".dat" << "\t";
                    tout << i_StationNumber << "\t" << s_EventLabel << "\t" << s_StationName << "\t" << dt.toString( "yyyy-MM" );
                    tout << "\t" << s_Device;
//					tout << ", " << InputStr.mid( 66, 8 ).simplified(); // date of purchase
                    tout << ", WRMC No. " << InputStr.mid( 75, 5 ).simplified();  // WRMC no

                    i_MethodID = findMethodID( i_StationNumber, InputStr.mid( 75, 5 ).simplified().toInt(), Method_ptr );

                    tout << "\t" << i_MethodID << endl;

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
                }
                else
                    b_Stop = true;
            }
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

/*! @brief Steuerung des Radiation Instruments Converters, LR0008 */

void MainWindow::doRadiationInstrumentsConverter()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QStringList sl_FilenameOut;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Radiation instruments converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = RadiationInstrumentsConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );

        if ( err == _NOERROR_ )
            err = concatenateFiles( gs_Path + "/" + "BSRN_LR0008.txt", sl_FilenameOut, tr( "Building LR0008 file..." ), 1, true );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Radiation instruments converter was canceled" ), false, false );

    onError( err );
}
