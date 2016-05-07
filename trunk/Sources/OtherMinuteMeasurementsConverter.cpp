/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Konvertiert den LR 0300.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param FilenameIn Dateiname der Inputdatei
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param P Pointer auf Array von Integern
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::OtherMinuteMeasurementsConverter( const bool b_Import, const QString& s_FilenameIn, structStaff *Staff_ptr, structStation *Station_ptr, int *P, const int i_NumOfFiles )
{
    int				i_StationNumber				= 0;
    int             i_SourceID                  = 0;
    int             i_PIID                      = 0;
    int				i_Month						= 1;
    int				i_Year						= 2000;
    int				i_Day						= 1;
    int				i_Minute					= 0;

    float           f_Latitude      = 0.;
    float           f_Longitude     = 0.;

    QString			InputStr		= "";

    QString			SearchString1	= "*C0300";
    QString			SearchString2	= "*U0300";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;
    bool			b_found			= false;

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    QFile fin( s_FilenameIn );
    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        P[i] = 0;

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Other measurements converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

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
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( "*C0002" ) == true ) || ( InputStr.startsWith( "*U0002" ) == true ) )
        {
            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            i_PIID = findPiID( InputStr.left( 38 ).simplified(), Staff_ptr );

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
// LR0300

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0300.txt";

    QFile fout( fi.absolutePath() + "/" + s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            b_found = true;

            if ( b_Import == true )
            {
                tout << "1599\t56349";  // Height above ground = 2 m
                tout << "\t55911\t55912\t55913\t55914";
                tout << "\t45299\t55915\t55916\t55917";
                tout << "\t55918\t55919\t55920\t55921" << endl;
            }
            else
            {
                tout << "Station\tDate/Time\tLatitude\tLongitude\tHeight above ground [m]\t";
                tout << "Short-wave upward (REFLEX) radiation [W/m**2]\tShort-wave upward (REFLEX) radiation, standard deviation [W/m**2]\tShort-wave upward (REFLEX) radiation, minimum [W/m**2]\tShort-wave upward (REFLEX) radiation, maximum [W/m**2]\t";
                tout << "Long-wave upward radiation [W/m**2]\tLong-wave upward radiation, standard deviation [W/m**2]\tLong-wave upward radiation, minimum [W/m**2]\tLong-wave upward radiation, maximum [W/m**2]\t";
                tout << "Net radiation [W/m**2]\tNet radiation, standard deviation [W/m**2]\tNet radiation, minimum [W/m**2]\tNet radiation, maximum [W/m**2]" << endl;
            }

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr = tin.readLine();
                ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    i_Day		= InputStr.mid( 1, 2 ).toInt();
                    i_Minute	= InputStr.mid( 4, 4 ).toInt();

                    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                    dt.setTime( QTime( 0, 0, 0 ) );
                    dt = dt.addSecs( i_Minute*60 );

                    if ( b_Import == false )
                        tout << s_EventLabel << "\t" << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t" << num2str( f_Latitude ) << "\t" << num2str( f_Longitude ) << "\t" << tr( "2" ) << "\t";
                    else
                        tout << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t";

                    if ( InputStr.mid( 9, 6 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 9, 6 ).simplified() << "\t";
                        P[1] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 16, 5 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 16, 5 ).simplified() << "\t";
                        P[2] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 22, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 22, 4 ).simplified() << "\t";
                        P[3] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 27, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 27, 4 ).simplified() << "\t";
                        P[4] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 32, 6 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 32, 6 ).simplified() << "\t";
                        P[5] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 39, 5 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 39, 5 ).simplified() << "\t";
                        P[6] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 45, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 45, 4 ).simplified() << "\t";
                        P[7] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 50, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 50, 4 ).simplified() << "\t";
                        P[8] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 55, 6 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 55, 6 ).simplified() << "\t";
                        P[9] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 62, 5 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 62, 5 ).simplified() << "\t";
                        P[10] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 68, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 68, 4 ).simplified() << "\t";
                        P[11] = 1;
                    }
                    else
                        tout << "\t";

                    if ( InputStr.mid( 73, 4 ).toFloat() > -99. )
                    {
                        tout << InputStr.mid( 73, 4 ).simplified() << endl;
                        P[12] = 1;
                    }
                    else
                        tout << endl;
                }
                else
                {
                    b_Stop = true;
                }
            }
        }
        else
        {
            InputStr	= tin.readLine();
            ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
        }
    }

//---------------------------------------------------------------------------------------------------

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( b_found == false )
        fout.remove();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 21.11.2007

/*! @brief Steuerung des Other Minute Measurements Converters, LR 0300 */

void MainWindow::doOtherMinuteMeasurementsConverter()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    int		P[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Other minute measurements convert working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = OtherMinuteMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, P, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Other minute measurements converter was canceled" ), false, false );

    onError( err );
}

