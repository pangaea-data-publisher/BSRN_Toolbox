/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Testet den LR 1100.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param P Pointer auf ein Array von Integern
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::RadiosondeMeasurementsTest( const QString& s_FilenameIn, int *P, const int i_NumOfFiles )
{
    int				i_P_sum			= 0;

    QString			InputStr		= "";
    QString			SearchString1	= "*C0005";
    QString			SearchString2	= "*U0005";
    QString			SearchString3	= "*C1100";
    QString			SearchString4	= "*U1100";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;
    bool			b_OK			= false;

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    if ( ( fi.exists() == false ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        return( _FILENOEXISTS_ );

// ***********************************************************************************************************************

    QFile fin( s_FilenameIn );

    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        P[i] = 0;

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Radiosonde measurements converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_OK == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( InputStr.simplified().right( 1 ) == "Y" )
                b_OK = true;
        }
    }

    if ( b_OK == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( _NODATAFOUND_ );
    }

    b_Stop = false;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr	= tin.readLine();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString3 ) == true ) || ( InputStr.startsWith( SearchString4 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    if ( InputStr.mid( 16, 4 ).simplified() != "-999" )		// Pressure
                        P[1] = 1;

                    if ( InputStr.mid( 27, 5 ).simplified() != "-99.9" )	// Temperature
                        P[2] = 1;

                    if ( InputStr.mid( 33, 6 ).simplified() != "-999.9" )	// Dew point
                        P[3] = 1;

                    if ( InputStr.mid( 40, 3 ).simplified() != "-99" )		// Wind direction
                        P[4] = 1;

                    if ( InputStr.mid( 44, 3 ).simplified() != "-99" )		// Wind speed
                        P[5] = 1;

                    if ( InputStr.mid( 48, 4 ).simplified() != "-9.9" )		// Ozone
                        P[6] = 1;

                    i_P_sum = 0;

                    for ( int i=1; i<=6 ; ++i )
                        i_P_sum += P[i];

                    if ( i_P_sum == 6 )
                        b_Stop = true;
                }
                else
                    b_Stop = true;
            }
        }
    }

// ***********************************************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Konvertiert den LR 1100.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::RadiosondeMeasurementsConverter( const bool b_Import, const QString& s_FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				err				= 0;

    int				i_PIID			= 506;
    int				i_SourceID		= 17;
    int				i_MethodID		= 43;
    int				i_StationNumber	= 0;
    int				i_Distance		= 0;

    int				i_Month			= 1;
    int				i_Year			= 2000;
    int				i_Day			= 1;
    int				i_Minute		= 0;

    float           f_Latitude      = 0.;
    float           f_Longitude     = 0.;

    QString			InputStr		= "";

    QString			SearchString1	= "*C0005";
    QString			SearchString2	= "*U0005";
    QString			SearchString3	= "*C0008";
    QString			SearchString4	= "*U0008";
    QString			SearchString5	= "*C1100";
    QString			SearchString6	= "*U1100";

    QString			s_StationName				= "";
    QString			s_EventLabel				= "";
    QString			s_Remarks					= "";
    QString			s_RadiosondeIdentification	= "";
    QString			s_Location					= "";

    unsigned int	ui_length					= 1;
    unsigned int	ui_filesize					= 1;

    bool			b_RadiosondeMeasurements	= false;
    bool			b_Stop						= false;
    bool			b_Comment					= false;

    int				P[MAX_NUM_OF_PARAMETER+1];

// ***********************************************************************************************************************

    err = RadiosondeMeasurementsTest( s_FilenameIn, P, i_NumOfFiles );

    if ( err == _NODATAFOUND_ )
        return( _NOERROR_ );

    if ( err == _APPBREAK_ )
        return( _APPBREAK_ );

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    QFile fin( s_FilenameIn );
    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Radiosonde measurements converter working (Building)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Building" ) );

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
    i_SourceID		= findInstituteID( i_StationNumber, Station_ptr );
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

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( "*C0002" ) == true ) || ( InputStr.startsWith( "*U0002" ) == true ) )
        {
            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            i_PIID = findPiId( InputStr.left( 38 ).simplified(), Staff_ptr );

            b_Stop = true;
        }
    }

    b_Stop = false;

// ***********************************************************************************************************************
// find position

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = ui_length + InputStr.length();

        if ( ( InputStr.startsWith( "*C0004" ) == true ) || ( InputStr.startsWith( "*U0004" ) == true ) )
        {
            for ( int i=0; i<6; i++ )
            {
                InputStr  = tin.readLine();
                ui_length = ui_length + InputStr.length();
            }

            f_Latitude  = InputStr.simplified().section( " ", 0, 0 ).toFloat() - 90.;
            f_Longitude = InputStr.simplified().section( " ", 1, 1 ).toFloat() - 180.;

            b_Stop = true;
        }
    }

    b_Stop = false;

// ***********************************************************************************************************************

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_1100";

    if ( b_Import == true )
        s_FilenameOut.append( "_imp.txt" );
    else
        s_FilenameOut.append( ".txt" );

    QFile fout( fi.absolutePath() + "/" + s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************
// 1100

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            InputStr = tin.readLine();
            ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( InputStr.simplified().right( 1 ) == "Y" )
            {
                b_RadiosondeMeasurements = true;

                InputStr = tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                s_RadiosondeIdentification = InputStr.left( 30 ).simplified();

                if ( InputStr.mid( 73, 5 ).simplified().isEmpty() == false )
                    s_RadiosondeIdentification += ", " + InputStr.mid( 73, 5 ).simplified();

                i_MethodID = findMethodID( s_RadiosondeIdentification, Method_ptr );

                s_Location	= InputStr.mid( 30, 25 ).simplified();
                i_Distance	= InputStr.mid( 57, 3 ).toInt();

                InputStr = tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                s_Remarks = InputStr.simplified();

                s_Remarks.replace( "no remarks", "" );
                s_Remarks.replace( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "" );
                s_Remarks.replace( "XXX", "" );
            }

            b_Stop = true;
        }

        if ( ( b_RadiosondeMeasurements == false ) && ( ( InputStr.startsWith( SearchString3 ) == true ) || ( InputStr.startsWith( SearchString4 ) == true ) ) )
            b_Stop = true;
    }

// ***********************************************************************************************************************
// 1100

    if ( b_RadiosondeMeasurements == true )
    {
        b_Stop = false;

        if ( b_Import == true )
        {
            tout << "/* DATA DESCRIPTION:" << eol;
            tout << "Author:\t" << i_PIID << eol;
            tout << "Source:\t" << i_SourceID << eol;

            if ( s_EventLabel == "BIL" )
                tout << "Title:\tRadiosonde measurements from station S. Great Plains (" << dt.toString( "yyyy-MM" ) << ")" << eol;
            else
            {
                if ( s_StationName.endsWith( "Station" ) == true )
                    tout << "Title:\tRadiosonde measurements from " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
                else
                    tout << "Title:\tRadiosonde measurements from station " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
            }

            tout << "Export Filename:\t" << s_EventLabel << "_radiosonde_" << dt.toString( "yyyy-MM" ) << eol;
            tout << "Event:\t" << s_EventLabel << eol;
            tout << "PI:\t" << i_PIID << eol;
            tout << "Parameter:";
            tout << "\t1599 * PI: "  << i_PIID << " * METHOD: 43 * FORMAT: yyyy-MM-dd'T'HH:mm" << eol;
            tout << "\t4607 * PI: "  << i_PIID << " * METHOD: 43 * FORMAT: ####0" << eol;

            if ( P[1] > 0 )
                tout << "\t49378 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ####0" << eol;

            if ( P[2] > 0 )
                tout << "\t4610 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ##0.0" << eol;

            if ( P[3] > 0 )
                tout << "\t4611 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ##0.0" << eol;

            if ( P[4] > 0 )
                tout << "\t2221 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ##0" << eol;

            if ( P[5] > 0 )
                tout << "\t18906 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ##0" << eol;

            if ( P[6] > 0 )
                tout << "\t45289 * PI: "  << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ##0.0" << eol;

            if ( i_Distance > 0 )
            {
                b_Comment = true;
                tout << "DataSet Comment:\tStart location: " << s_Location << "; Distance from radiation site: " << i_Distance << " km";
            }

            if ( s_Remarks.isEmpty() == false )
            {
                if ( b_Comment == true )
                    tout << "; " << s_Remarks;
                else
                    tout << "DataSet Comment:\t" << s_Remarks;

                b_Comment = true;
            }

            if ( ( s_EventLabel == "E13" ) || ( s_EventLabel == "BIL" ) )
            {
                if ( s_Remarks.isEmpty() == false )
                    tout << ". ";

                tout << "The radiosonde measurements from station S. Great Plains are used for Billings and E13";
            }

            if ( b_Comment == true )
                tout << eol;

            tout << "Project:\t4094" << eol;
            tout << "Topologic Type:\t5" << eol;
            tout << "Status:\t4" << eol;
            tout << "User:\t1144" << eol;
            tout << "Login:\t3" << eol;
            tout << ReferenceOtherVersion( s_EventLabel, dt, i_PIID, s_StationName ) << eol;

            if ( b_overwriteDataset == true )
            {
                int i_DatasetID = findDatasetId( s_EventLabel + "_radiosonde_" + dt.toString( "yyyy-MM" ), Dataset_ptr );

                if ( i_DatasetID > 0 )
                    tout << "DataSet ID:\t" << i_DatasetID << eol;
            }

            tout << "*/" << eol;
        }
    }

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString5 ) == true ) || ( InputStr.startsWith( SearchString6 ) == true ) )
        {
            if ( b_Import == true )
            {
                tout << "Event label\t1599\t4607";

                if ( P[1] > 0 )
                    tout << "\t49378";

                if ( P[2] > 0 )
                    tout << "\t4610";

                if ( P[3] > 0 )
                    tout << "\t4611";

                if ( P[4] > 0 )
                    tout << "\t2221";

                if ( P[5] > 0 )
                    tout << "\t18906";

                if ( P[6] > 0 )
                    tout << "\t45289";

                tout << eol;
            }
            else
            {
                tout << "Station\tDate/Time\tLatitude\tLongitude\tAltitude [m]";

                if ( P[1] > 0 )
                    tout << "\tPressure, at given altitude [hPa]";

                if ( P[2] > 0 )
                    tout << "\tTemperature, air [deg C]";

                if ( P[3] > 0 )
                    tout << "\tDew/frost point [deg C]";

                if ( P[4] > 0 )
                    tout << "\tWind direction [deg]";

                if ( P[5] > 0 )
                    tout << "\tWind speed [m/sec]";

                if ( P[6] > 0 )
                    tout << "\tOzone [mPa]";

                tout << eol;
            }

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    i_Day		= InputStr.mid( 1, 2 ).toInt();
                    i_Minute	= InputStr.mid( 4, 4 ).toInt();

                    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                    dt.setTime( QTime( 0, 0, 0 ) );
                    dt = dt.addSecs( i_Minute*60 );

                    if ( b_Import == false )
                        tout << s_EventLabel << "\t" << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t" << QString( "%1" ).arg( f_Latitude ) << "\t" << QString( "%1" ).arg( f_Longitude ) << "\t" << InputStr.mid( 21, 5 ).simplified();
                    else
                        tout << s_EventLabel << "\t" << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t" << InputStr.mid( 21, 5 ).simplified();

                    if ( P[1] > 0 )
                    {
                        if ( InputStr.mid( 16, 4 ).simplified() != "-999" )
                            tout << "\t" << InputStr.mid( 16, 4 ).simplified(); // Pressure
                        else
                            tout << "\t";
                    }

                    if ( P[2] > 0 )
                    {
                        if ( InputStr.mid( 27, 5 ).simplified() != "-99.9" )
                            tout << "\t" << InputStr.mid( 27, 5 ).simplified(); // Temperature
                        else
                            tout << "\t";
                    }

                    if ( P[3] > 0 )
                    {
                        if ( InputStr.mid( 33, 6 ).simplified() != "-999.9" )
                            tout << "\t" << InputStr.mid( 33, 6 ).simplified(); // dew point
                        else
                            tout << "\t";
                    }

                    if ( P[4] > 0 )
                    {
                        if ( InputStr.mid( 40, 3 ).simplified() != "-99" )
                            tout << "\t" << InputStr.mid( 40, 3 ).simplified(); // wind direction
                        else
                            tout << "\t";
                    }

                    if ( P[5] > 0 )
                    {
                        if ( InputStr.mid( 44, 3 ).simplified() != "-99" )
                            tout << "\t" << InputStr.mid( 44, 3 ).simplified(); // wind speed
                        else
                            tout << "\t";
                    }

                    if ( P[6] > 0 )
                    {
                        if ( InputStr.mid( 48, 4 ).simplified() != "-9.9" )
                            tout << "\t" << InputStr.mid( 48, 4 ).simplified();  // ozone concentration
                        else
                            tout << "\t";
                    }

                    tout << eol;

//					b_Stop = true;
                }
                else
                {
                    b_Stop = true;
                }
            }
        }
    }

// ***********************************************************************************************************************

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

/*! @brief Steuerung des Radiosonde Measurements Converters, LR 1100 */

void MainWindow::doRadiosondeMeasurementsConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Radiosonde measurements converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = RadiosondeMeasurementsConverter( b_Import, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Radiosonde measurements converter was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Radiosonde Measurements Converters im Import-Mode, LR 1100 */

void MainWindow::doRadiosondeMeasurementsImportConverter()
{
    doRadiosondeMeasurementsConverter( true );
}

