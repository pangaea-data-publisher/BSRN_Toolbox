/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Testet den LR 0100 und den LR 0300.
*
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param P Pointer auf ein Array von Integern
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::BasicMeasurementsTest( const QString& s_FilenameIn, int *P, const int i_NumOfFiles )
{
    int				n				= 0;
    int				i_P_sum			= 0;

    QString			InputStr		= "";
    QString			SearchString1	= "*C0100";
    QString			SearchString2	= "*U0100";

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

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        P[i] = 0;

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Basic measurements converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

// ***********************************************************************************************************************

    InputStr = tin.readLine().simplified();
    InputStr = tin.readLine().simplified();

    while ( ( tin.atEnd() == false ) && ( InputStr.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr  = tin.readLine().simplified();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( InputStr.startsWith( "*" ) == false )
        {
            InputStr = " " + InputStr + " ";

            if ( InputStr.contains( " 2 " ) ) ++n;
            if ( InputStr.contains( " 3 " ) ) ++n;
            if ( InputStr.contains( " 4 " ) ) ++n;
            if ( InputStr.contains( " 5 " ) ) ++n;
            if ( InputStr.contains( " 21 " ) ) ++n;
            if ( InputStr.contains( " 22 " ) ) ++n;
            if ( InputStr.contains( " 23 " ) ) ++n;
        }
    }

    if ( n == 0 )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( _NOERROR_ );
    }

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr	= tin.readLine();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    if ( InputStr.mid( 11, 4 ).simplified() != "-999" )
                        P[1] = 1;

                    if ( InputStr.mid( 16, 5 ).simplified() != "-99.9" )
                        P[2] = 1;

                    if ( InputStr.mid( 22, 4 ).simplified() != "-999" )
                        P[3] = 1;

                    if ( InputStr.mid( 27, 4 ).simplified() != "-999" )
                        P[4] = 1;

                    if ( InputStr.mid( 34, 4 ).simplified() != "-999" )
                        P[5] = 1;

                    if ( InputStr.mid( 39, 5 ).simplified() != "-99.9" )
                        P[6] = 1;

                    if ( InputStr.mid( 45, 4 ).simplified() != "-999" )
                        P[7] = 1;

                    if ( InputStr.mid( 50, 4 ).simplified() != "-999" )
                        P[8] = 1;

                    InputStr  = tin.readLine();
                    ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( InputStr.length() < 80 )
                    { // Raw data format
                        if ( InputStr.mid( 11, 4 ).simplified() != "-999" )
                            P[9] = 1;

                        if ( InputStr.mid( 16, 5 ).simplified() != "-99.9" )
                            P[10] = 1;

                        if ( InputStr.mid( 22, 4 ).simplified() != "-999" )
                            P[11] = 1;

                        if ( InputStr.mid( 27, 4 ).simplified() != "-999" )
                            P[12] = 1;

                        if ( InputStr.mid( 34, 4 ).simplified() != "-999" )
                            P[13] = 1;

                        if ( InputStr.mid( 39, 5 ).simplified() != "-99.9" )
                            P[14] = 1;

                        if ( InputStr.mid( 45, 4 ).simplified() != "-999" )
                            P[15] = 1;

                        if ( InputStr.mid( 50, 4 ).simplified() != "-999" )
                            P[16] = 1;

                        if ( InputStr.mid( 58, 5 ).simplified() != "-99.9" )
                            P[17] = 1;

                        if ( InputStr.mid( 64, 5 ).simplified() != "-99.9" )
                            P[18] = 1;

                        if ( InputStr.mid( 70, 4 ).simplified() != "-999" )
                            P[19] = 1;
                    }
                    else
                    { // Quality checked format
                        InputStr = InputStr.simplified();

                        for ( int i=1; i<=4; ++i )
                        {
                            if ( InputStr.section( " ", i, i ).toFloat() > -99. )
                                P[8+i] = 1;
                        }

                        for ( int i=6; i<=12; ++i )
                        {
                            if ( InputStr.section( " ", i, i ).toFloat() > -99. )
                                P[7+i] = 1;
                        }
                    }

                    i_P_sum = 0;

                    for ( int i=1; i<=19 ; ++i )
                        i_P_sum += P[i];

                    if ( i_P_sum == 19 )
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

    return( _NOERROR_ );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

/*! @brief Konvertiert den LR 1300.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param Parameter_0001 Pointer auf Array aller Parameter aus LR 0001
*   @param Parameter_0009 Pointer auf Array aller Parameter aus LR 0009
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::BasicMeasurementsConverter( const bool b_Import, const QString& s_FilenameIn, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				err				= 0;

    int             j               = 0;
    int				k				= 0;
    int             l               = 0;
    int				n				= 0;
    int				n_P9			= 0;

    int				i_PIID			= 506;
    int				i_SourceID		= 17;
    int				i_MethodID		= 43;
    int				i_ParameterID	= 1;
    int				i_StationNumber	= 0;

    int				i_Month			= 1;
    int				i_Year			= 2000;
    int				i_Day			= 1;
    int				i_Hour			= 0;
    int				i_Minute		= 0;

    float           f_Latitude      = 0.;
    float           f_Longitude     = 0.;

    QString			OutputStr		= "";

    QString			InputStr	= "";
    QString			InputStr_0300	= "";

    QString			SearchString1	= "*C0009";
    QString			SearchString2	= "*U0009";
    QString			SearchString3	= "*C0100";
    QString			SearchString4	= "*U0100";

    QString			s_StationName		= "";
    QString			s_EventLabel		= "";

    unsigned int	ui_length			= 1;
    unsigned int	ui_filesize			= 1;

    bool			b_Stop				= false;
    bool			b_Out				= false;
    bool			b_gr				= false;
    bool			b_dr				= false;
    bool			b_dif				= false;
    bool			b_dlw				= false;
    bool			b_usr				= false;
    bool			b_ul				= false;
    bool			b_netr				= false;
    bool			b_temp				= false;
    bool			b_relHum			= false;
    bool			b_press				= false;

    bool			b_LR0300			= false;
    bool			b_QualityChecked	= false;

    int				P[MAX_NUM_OF_PARAMETER+1];
    int				PoM[MAX_NUM_OF_PARAMETER+1];

// ***********************************************************************************************************************

    err = BasicMeasurementsTest( s_FilenameIn, P, i_NumOfFiles );

    if ( err == _ERROR_ )
        return( _ERROR_ );

    if ( err == _APPBREAK_ )
        return( _APPBREAK_ );

// ***********************************************************************************************************************

    for ( int i=0; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        Parameter_0001[i].ParameterID = -1;
        Parameter_0001[i].MethodID = -1;

        Parameter_0009[i].ParameterID = -1;
        Parameter_0009[i].MethodID = -1;
    }

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

    InputStr  = tin.readLine();
    ui_length = InputStr.length();

    if ( ( InputStr.startsWith( "*C0001" ) == false ) && ( InputStr.startsWith( "*U0001" ) == false ) )
    {
        fin.close();
        return( -40 );
    }

// ***********************************************************************************************************************

    InputStr  = tin.readLine();
    ui_length = ui_length + InputStr.length();

    i_StationNumber	= InputStr.left( 3 ).toInt();
    i_SourceID		= findInstituteID( i_StationNumber, Station_ptr );
    s_StationName	= findStationName( i_StationNumber, Station_ptr );
    s_EventLabel	= findEventLabel( i_StationNumber, Station_ptr );

// ***********************************************************************************************************************

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

    while ( ( tin.atEnd() == false ) && ( InputStr.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr  = tin.readLine().simplified();
        ui_length = ui_length + InputStr.length();

        if ( InputStr.startsWith( "*" ) == false )
        {
            InputStr.replace( " ", "\t" );
            InputStr.replace( "-1", "" );

            for ( int i=0; i<8; ++i )
            {
                i_ParameterID = InputStr.section( "\t", i, i ).toInt();

                if ( i_ParameterID > 0 )
                {
                    Parameter_0001[i+j+1].ParameterID = i_ParameterID;

                    if ( ( i_ParameterID == 131 ) || ( i_ParameterID == 132 ) || ( i_ParameterID == 141 ) )
                        b_LR0300 = true;

                    ++n;
                }
            }

            j += 8;
        }
    }

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr.startsWith( "*C0002" ) == true ) || ( InputStr.startsWith( "*U0002" ) == true ) )
        {
            InputStr  = tin.readLine();
            ui_length = ui_length + InputStr.length();

            InputStr  = tin.readLine();
            ui_length = ui_length + InputStr.length();

            i_PIID = findPiId( InputStr.left( 38 ).simplified(), Staff_ptr );

            b_Stop = true;
        }
        else
        {
            fin.close();
            return( -40 );
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

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" );

    if ( b_LR0300 == true )
        s_FilenameOut.append( "_0100+0300" );
    else
        s_FilenameOut.append( "_0100" );

    if ( b_Import == true )
        s_FilenameOut.append( "_imp.txt" );
    else
        s_FilenameOut.append( ".txt" );

    QFile fout( fi.absolutePath() + "/" + s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************

    QFile fin0300( fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0300.txt" );

    if ( b_LR0300 == true )
    {
        err = OtherMinuteMeasurementsConverter( true, s_FilenameIn, Staff_ptr, Station_ptr, PoM, i_NumOfFiles );

        if ( ( err != _NOERROR_ ) || ( fin0300.open( QIODevice::ReadOnly | QIODevice::Text ) == false ) )
        {
            fin.close();
            return( -10 );
        }
    }

    QTextStream tin0300( &fin0300 );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Basic measurements converter working (Building)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Building" ) );

// ***********************************************************************************************************************

    InputStr_0300 = tin0300.readLine(); // read header

// ***********************************************************************************************************************
// LR 0009

    l = 0;
    j = 0;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr = tin.readLine();
                ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    if ( ++l <= MAX_NUM_OF_PARAMETER )
                    {
                        Parameter_0009[l].ParameterID	= InputStr.mid( 10, 9 ).simplified().toInt();
                        Parameter_0009[l].MethodID		= InputStr.mid( 20, 5 ).simplified().toInt();

                        i_Day		= InputStr.mid( 1, 2 ).toInt();
                        i_Hour		= InputStr.mid( 4, 2 ).toInt();
                        i_Minute	= InputStr.mid( 7, 2 ).toInt();

                        if ( i_Day == -1 ) i_Day = 1;
                        if ( i_Hour == -1 ) i_Hour = 0;
                        if ( i_Minute == -1 ) i_Minute = 0;

                        Parameter_0009[l].DateTime.setDate( QDate( i_Year, i_Month, i_Day ) );
                        Parameter_0009[l].DateTime.setTime( QTime( i_Hour, i_Minute, 0 ) );

                        n_P9++;
                    }
                }
                else
                {
                    b_Stop = true;
                }
            }
        }
    }

// ***********************************************************************************************************************

    b_Stop = false;

    if ( b_Import == true )
    {
        tout << "/* DATA DESCRIPTION:" << eol;
        tout << "Author:\t" << i_PIID << eol;
        tout << "Source:\t" << i_SourceID << eol;

        if ( s_StationName.endsWith( "Station" ) == true )
        {
            if ( b_LR0300 == true )
                tout << "Title:\tBasic and other measurements of radiation at " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
            else
                tout << "Title:\tBasic measurements of radiation at " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
        }
        else
        {
            if ( b_LR0300 == true )
                tout << "Title:\tBasic and other measurements of radiation at station " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
            else
                tout << "Title:\tBasic measurements of radiation at station " << s_StationName << " (" << dt.toString( "yyyy-MM" ) << ")" << eol;
        }

        tout << "Export Filename:\t" << s_EventLabel << "_radiation_" << dt.toString( "yyyy-MM" ) << eol;
        tout << "Event:\t" << s_EventLabel << eol;
        tout << "PI:\t" << i_PIID << eol;
        tout << "Parameter:";
        tout << "\t1599 * PI: "  << i_PIID << " * METHOD: 43 * FORMAT: yyyy-MM-dd'T'HH:mm" << eol;
        tout << "\t56349 * PI: "  << i_PIID << " * METHOD: 43 * FORMAT: #0" << eol; // Height above ground = 2 m

// ***********************************************************************************************************************
// 0100

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 2 ) && ( b_gr == false ) ) // Global radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 2 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( P[1] > 0 )
                        tout << "\t31460 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[2] > 0 )
                        tout << "\t55905 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[3] > 0 )
                        tout << "\t55906 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[4] > 0 )
                        tout << "\t55907 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( P[1] > 0 )
                        tout << "\t31460 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( P[2] > 0 )
                        tout << "\t55905 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;

                    if ( P[3] > 0 )
                        tout << "\t55906 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( P[4] > 0 )
                        tout << "\t55907 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_gr = true;
            }
        }

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 3 ) && ( b_dr == false ) ) // Direct radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 3 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( P[5] > 0 )
                        tout << "\t45294 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[6] > 0 )
                        tout << "\t55962 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[7] > 0 )
                        tout << "\t55957 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[8] > 0 )
                        tout << "\t55958 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( P[5] > 0 )
                        tout << "\t45294 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                    if ( P[6] > 0 )
                        tout << "\t55962 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;
                    if ( P[7] > 0 )
                        tout << "\t55957 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                    if ( P[8] > 0 )
                        tout << "\t55958 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_dr = true;
            }
        }

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 4 ) && ( b_dif == false ) ) // Diffuse radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 4 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( P[9] > 0 )
                        tout << "\t45293 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[10] > 0 )
                        tout << "\t55961 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[11] > 0 )
                        tout << "\t55959 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                    if ( P[12] > 0 )
                        tout << "\t55960 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( P[9] > 0 )
                        tout << "\t45293 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                    if ( P[10] > 0 )
                        tout << "\t55961 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;
                    if ( P[11] > 0 )
                        tout << "\t55959 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                    if ( P[12] > 0 )
                        tout << "\t55960 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_dif = true;
            }
        }

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 5 ) && ( b_dlw == false ) ) // Downward long-wave radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 5 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( P[13] > 0 )
                        tout << "\t45298 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[14] > 0 )
                        tout << "\t55908 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[15] > 0 )
                        tout << "\t55909 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( P[16] > 0 )
                        tout << "\t55910 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( P[13] > 0 )
                        tout << "\t45298 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( P[14] > 0 )
                        tout << "\t55908 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;

                    if ( P[15] > 0 )
                        tout << "\t55909 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( P[16] > 0 )
                        tout << "\t55910 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_dlw = true;
            }
        }

// ***********************************************************************************************************************
// 0300

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 131 ) && ( b_usr == false ) ) // Reflex radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 131 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( PoM[1] > 0 )
                        tout << "\t55911 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[2] > 0 )
                        tout << "\t55912 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[3] > 0 )
                        tout << "\t55913 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[4] > 0 )
                        tout << "\t55914 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( PoM[1] > 0 )
                        tout << "\t55911 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[2] > 0 )
                        tout << "\t55912 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;

                    if ( PoM[3] > 0 )
                        tout << "\t55913 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[4] > 0 )
                        tout << "\t55914 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_usr = true;
            }
        }

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 132 ) && ( b_ul == false ) ) // Long-wave radiation, upward
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 132 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( PoM[5] > 0 )
                        tout << "\t45299 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[6] > 0 )
                        tout << "\t55915 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[7] > 0 )
                        tout << "\t55916 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[8] > 0 )
                        tout << "\t55917 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( PoM[5] > 0 )
                        tout << "\t45299 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[6] > 0 )
                        tout << "\t55915 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;

                    if ( PoM[7] > 0 )
                        tout << "\t55916 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[8] > 0 )
                        tout << "\t55917 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_ul = true;
            }
        }

        for ( int i=1; i<=n_P9; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 141 ) && ( b_netr == false ) ) // Netto radiation
            {
                k = 0; for ( j=i+1; j<=n_P9; ++j ) if ( Parameter_0009[j].ParameterID == 141 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k > 0 )
                {
                    if ( PoM[9] > 0 )
                        tout << "\t55918 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[10] > 0 )
                        tout << "\t55919 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[11] > 0 )
                        tout << "\t55920 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;

                    if ( PoM[12] > 0 )
                        tout << "\t55921 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0 * Comment: Changed to WRMC No. " << Parameter_0009[k].MethodID << " at " << Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" ) << eol;
                }
                else
                {
                    if ( PoM[9] > 0 )
                        tout << "\t55918 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[10] > 0 )
                        tout << "\t55919 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0.0" << eol;

                    if ( PoM[11] > 0 )
                        tout << "\t55920 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;

                    if ( PoM[12] > 0 )
                        tout << "\t55921 * PI: " << i_PIID << " * METHOD: " << i_MethodID << " * FORMAT: ###0" << eol;
                }

                b_netr = true;
            }
        }

// ***********************************************************************************************************************
// Temperature, relative humidity, station pressure

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 21 ) && ( P[17] > 0 ) && ( b_temp == false ) )
            {
                tout << "\t48820 * PI: " << i_PIID << " * METHOD: " << 4722 << " * FORMAT: ###0.0" << eol;
                b_temp = true;
            }
        }

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 22 ) && ( P[18] > 0 ) && ( b_relHum == false )  )
            {
                tout << "\t2219 * PI: " << i_PIID << " * METHOD: " << 5039 << " * FORMAT: ###0.0" << eol;
                b_relHum = true;
            }
        }

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 23 ) && ( P[19] > 0 ) && ( b_press == false )  )
            {
                tout << "\t48823 * PI: " << i_PIID << " * METHOD: " << 359 << " * FORMAT: ###0" << eol;
                b_press = true;
            }
        }

        b_gr		= false;
        b_dr		= false;
        b_dif		= false;
        b_dlw		= false;
        b_usr		= false;
        b_ul		= false;
        b_netr		= false;
        b_temp		= false;
        b_relHum	= false;
        b_press		= false;

        tout << "Project:\t4094" << eol;
        tout << "Topologic Type:\t8" << eol;
        tout << "Status:\t4" << eol;
        tout << "User:\t1144" << eol;
        tout << "Login:\t3" << eol;

        tout << ReferenceOtherVersionClassic( s_EventLabel, dt );

        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetId( s_EventLabel + "_radiation_" + dt.toString( "yyyy-MM" ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                tout << "DataSet ID:\t" << i_DatasetID << eol;
        }

        tout << "*/" << eol;
    }

    if ( b_Import == true )
        tout << "Event label\t1599\t56349";  // Height above ground = 2 m
    else
        tout << "Station\tDate/Time\tLatitude\tLongitude\tHeight above ground [m]";

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 2 ) && ( b_gr == false ) ) // Global radiation
        {
            if ( b_Import == true )
            {
                if ( P[1] > 0 )
                    tout << "\t31460";

                if ( P[2] > 0 )
                    tout << "\t55905";

                if ( P[3] > 0 )
                    tout << "\t55906";

                if ( P[4] > 0 )
                    tout << "\t55907";
            }
            else
            {
                if ( P[1] > 0 )
                    tout << "\tShort-wave downward (GLOBAL) radiation [W/m**2]";

                if ( P[2] > 0 )
                    tout << "\tShort-wave downward (GLOBAL) radiation, standard deviation [W/m**2]";

                if ( P[3] > 0 )
                    tout << "\tShort-wave downward (GLOBAL) radiation, minimum [W/m**2]";

                if ( P[4] > 0 )
                    tout << "\tShort-wave downward (GLOBAL) radiation, maximum [W/m**2]";
            }

            b_gr = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 3 ) && ( b_dr == false ) ) // Direct radiation
        {
            if ( b_Import == true )
            {
                if ( P[5] > 0 )
                    tout << "\t45294";

                if ( P[6] > 0 )
                    tout << "\t55962";

                if ( P[7] > 0 )
                    tout << "\t55957";

                if ( P[8] > 0 )
                    tout << "\t55958";
            }
            else
            {
                if ( P[5] > 0 )
                    tout << "\tDirect radiation [W/m**2]";

                if ( P[6] > 0 )
                    tout << "\tDirect radiation, standard deviation [W/m**2]";

                if ( P[7] > 0 )
                    tout << "\tDirect radiation, minimum [W/m**2]";

                if ( P[8] > 0 )
                    tout << "\tDirect radiation, maximum [W/m**2]";
            }

            b_dr = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 4 ) && ( b_dif == false ) ) // Diffuse radiation
        {
            if ( b_Import == true )
            {
                if ( P[9] > 0 )
                    tout << "\t45293";

                if ( P[10] > 0 )
                    tout << "\t55961";

                if ( P[11] > 0 )
                    tout << "\t55959";

                if ( P[12] > 0 )
                    tout << "\t55960";
            }
            else
            {
                if ( P[9] > 0 )
                    tout << "\tDiffuse radiation [W/m**2]";

                if ( P[10] > 0 )
                    tout << "\tDiffuse radiation, standard deviation [W/m**2]";

                if ( P[11] > 0 )
                    tout << "\tDiffuse radiation, minimum [W/m**2]";

                if ( P[12] > 0 )
                    tout << "\tDiffuse radiation, maximum [W/m**2]";
            }

            b_dif = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 5 ) && ( b_dlw == false ) ) // Long-wave downward radiation
        {
            if ( b_Import == true )
            {
                if ( P[13] > 0 )
                    tout << "\t45298";
                if ( P[14] > 0 )
                    tout << "\t55908";
                if ( P[15] > 0 )
                    tout << "\t55909";
                if ( P[16] > 0 )
                    tout << "\t55910";
            }
            else
            {
                if ( P[13] > 0 )
                    tout << "\tLong-wave downward radiation [W/m**2]";
                if ( P[14] > 0 )
                    tout << "\tLong-wave downward radiation, standard deviation [W/m**2]";
                if ( P[15] > 0 )
                    tout << "\tLong-wave downward radiation, minimum [W/m**2]";
                if ( P[16] > 0 )
                    tout << "\tLong-wave downward radiation, maximum [W/m**2]";
            }

            b_dlw = true;
        }
    }

    if ( b_LR0300 == true )
    {
        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 131 ) && ( b_usr == false ) ) // Reflex radiation
            {
                if ( b_Import == true )
                {
                    if ( PoM[1] > 0 )
                        tout << "\t55911";

                    if ( PoM[2] > 0 )
                        tout << "\t55912";

                    if ( PoM[3] > 0 )
                        tout << "\t55913";

                    if ( PoM[4] > 0 )
                        tout << "\t55914";
                }
                else
                {
                    if ( PoM[1] > 0 )
                        tout << "\tShort-wave upward (REFLEX) radiation [W/m**2]";

                    if ( PoM[2] > 0 )
                        tout << "\tShort-wave upward (REFLEX) radiation, standard deviation [W/m**2]";

                    if ( PoM[3] > 0 )
                        tout << "\tShort-wave upward (REFLEX) radiation, minimum [W/m**2]";

                    if ( PoM[4] > 0 )
                        tout << "\tShort-wave upward (REFLEX) radiation, maximum [W/m**2]";
                }

                b_usr = true;
            }
        }

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 132 ) && ( b_ul == false ) ) // Long-wave upward radiation
            {
                if ( b_Import == true )
                {
                    if ( PoM[5] > 0 )
                        tout << "\t45299";
                    if ( PoM[6] > 0 )
                        tout << "\t55915";
                    if ( PoM[7] > 0 )
                        tout << "\t55916";
                    if ( PoM[8] > 0 )
                        tout << "\t55917";
                }
                else
                {
                    if ( PoM[5] > 0 )
                        tout << "\tLong-wave upward radiation [W/m**2]";

                    if ( PoM[6] > 0 )
                        tout << "\tLong-wave upward radiation, standard deviation [W/m**2]";

                    if ( PoM[7] > 0 )
                        tout << "\tLong-wave upward radiation, minimum [W/m**2]";

                    if ( PoM[8] > 0 )
                        tout << "\tLong-wave upward radiation, maximum [W/m**2]";
                }

                b_ul = true;
            }
        }

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 141 ) && ( b_netr == false ) ) // Net radiation
            {
                if ( b_Import == true )
                {
                    if ( PoM[9] > 0 )
                        tout << "\t55918";

                    if ( PoM[10] > 0 )
                        tout << "\t55919";

                    if ( PoM[11] > 0 )
                        tout << "\t55920";

                    if ( PoM[12] > 0 )
                        tout << "\t55921";
                }
                else
                {
                    if ( PoM[9] > 0 )
                        tout << "\tNet radiation [W/m**2]";

                    if ( PoM[10] > 0 )
                        tout << "\tNet radiation, standard deviation [W/m**2]";

                    if ( PoM[11] > 0 )
                        tout << "\tNet radiation, minimum [W/m**2]";

                    if ( PoM[12] > 0 )
                        tout << "\tNet radiation, maximum [W/m**2]";
                }

                b_netr = true;
            }
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 21 ) && ( P[17] > 0 ) && ( b_temp == false ) ) // Air temperature
        {
            if ( b_Import == true )
                tout << "\t48820";
            else
                tout << "\tAir temperature [deg C]";

            b_temp = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 22 ) && ( P[18] > 0 ) && ( b_relHum == false ) ) // relative humidity
        {
            if ( b_Import == true )
                tout << "\t2219";
            else
                tout << "\tHumidity, relative [%]";

            b_relHum = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 23 ) && ( P[19] > 0 ) && ( b_press == false ) ) // Station pressure
        {
            if ( b_Import == true )
                tout << "\t48823";
            else
                tout << "\tStation pressure [hPa]";

            b_press = true;
        }
    }

    tout << eol;

//	b_Stop = true;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr.startsWith( SearchString3 ) == true ) || ( InputStr.startsWith( SearchString4 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr = tin.readLine();
                ui_length     = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    i_Day		= InputStr.mid( 1, 2 ).toInt();
                    i_Minute	= InputStr.mid( 4, 4 ).toInt();

                    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                    dt.setTime( QTime( 0, 0, 0 ) );
                    dt = dt.addSecs( i_Minute*60 );

                    b_Out	  = false;

                    if ( b_Import == false )
                        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ) + "\t" + tr( "2" );
                    else
                        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + tr( "2" );

                    if ( b_gr == true ) // global radiation = 2
                    {
                        if ( P[1] > 0 )
                        {
                            if ( InputStr.mid( 11, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 11, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[2] > 0 )
                        {
                            if ( InputStr.mid( 16, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 16, 5 ).simplified();
                            }
                            else
                                OutputStr += "\t";

                        }

                        if ( P[3] > 0 )
                        {
                            if ( InputStr.mid( 22, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 22, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";

                        }

                        if ( P[4] > 0 )
                        {
                            if ( InputStr.mid( 27, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 27, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    if ( b_dr == true ) // direct radiation = 3
                    {
                        if ( P[5] > 0 )
                        {
                            if ( InputStr.mid( 34, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 34, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";

                        }

                        if ( P[6] > 0 )
                        {
                            if ( InputStr.mid( 39, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 39, 5 ).simplified();
                            }
                            else
                                OutputStr += "\t";

                        }

                        if ( P[7] > 0 )
                        {
                            if ( InputStr.mid( 45, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 45, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";

                        }

                        if ( P[8] > 0 )
                        {
                            if ( InputStr.mid( 50, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 50, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    InputStr = tin.readLine();
                    ui_length     = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( InputStr.length() < 80 )
                    { // Raw data format
                        if ( b_dif == true ) // diffuse radiation = 4
                        {
                            if ( P[9] > 0 )
                            {
                                if ( InputStr.mid( 11, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 11, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[10] > 0 )
                            {
                                if ( InputStr.mid( 16, 5 ).simplified() != "-99.9" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 16, 5 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[11] > 0 )
                            {
                                if ( InputStr.mid( 22, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 22, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[12] > 0 )
                            {
                                if ( InputStr.mid( 27, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 27, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";
                            }
                        }

                        if ( b_dlw == true ) // downward long-wave radiation = 5
                        {
                            if ( P[13] > 0 )
                            {
                                if ( InputStr.mid( 34, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 34, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[14] > 0 )
                            {
                                if ( InputStr.mid( 39, 5 ).simplified() != "-99.9" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 39, 5 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[15] > 0 )
                            {
                                if ( InputStr.mid( 45, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 45, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";

                            }

                            if ( P[16] > 0 )
                            {
                                if ( InputStr.mid( 50, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr += "\t" + InputStr.mid( 50, 4 ).simplified();
                                }
                                else
                                    OutputStr += "\t";
                            }
                        }
                    }
                    else
                    { // Quality checked format
                        b_QualityChecked = true;

                        InputStr = InputStr.simplified();

                        if ( b_dif == true ) // diffuse radiation = 4
                        {
                            for ( int i=1; i<=4; ++i )
                            {
                                if ( P[8+i] > 0 )
                                {
                                    if ( InputStr.section( " ", i, i ).toFloat() > -99. )
                                    {
                                        b_Out = true;
                                        OutputStr += "\t" + InputStr.section( " ", i, i );
                                    }
                                    else
                                        OutputStr += "\t";
                                }
                            }
                        }

                        if ( b_dlw == true ) // downward long-wave radiation = 5
                        {
                            for ( int i=6; i<=9; ++i )
                            {
                                if ( P[7+i] > 0 )
                                {
                                    if ( InputStr.section( " ", i, i ).toFloat() > -99. )
                                    {
                                        b_Out = true;
                                        OutputStr += "\t" + InputStr.section( " ", i, i );
                                    }
                                    else
                                        OutputStr += "\t";
                                }
                            }
                        }
                    }

                    if ( ( ( b_usr == true ) || ( b_ul == true ) || ( b_netr == true ) ) && ( tin0300.atEnd() == false ) )
                    {
                        InputStr_0300 = tin0300.readLine();

                        if ( InputStr_0300.section( "\t", 1, 1) == dt.toString( "yyyy-MM-ddThh:mm" ) )
                        {
                            if ( b_usr == true ) // Reflex radiation = 131
                            {
                                b_Out = true;

                                if ( PoM[1] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 3, 3 );

                                if ( PoM[2] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 4, 4 );

                                if ( PoM[3] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 5, 5 );

                                if ( PoM[4] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 6, 6 );
                            }

                            if ( b_ul == true ) // Long-wave radiation, upward = 132
                            {
                                b_Out = true;

                                if ( PoM[5] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t",  7,  7 );

                                if ( PoM[6] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t",  8,  8 );

                                if ( PoM[7] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t",  9,  9 );

                                if ( PoM[8] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 10, 10 );
                            }

                            if ( b_netr == true ) // Netto radiation = 141
                            {
                                b_Out = true;

                                if ( PoM[9] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 11, 11 );

                                if ( PoM[10] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 12, 12 );

                                if ( PoM[11] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 13, 13 );

                                if ( PoM[12] > 0 )
                                    OutputStr += "\t" + InputStr_0300.section( "\t", 14, 14 );
                            }
                        }
                        else
                        {
                            tout << "_ERROR_ - 0100 and 0300 not compatible" << eol;
                            b_Stop = true;
                        }
                    }

                    if ( b_QualityChecked == false )
                    { // Raw data format
                        if ( b_temp == true ) // air temperature = 21
                        {
                            if ( InputStr.mid( 58, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 58, 5 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( b_relHum == true ) // relative humidity = 22
                        {
                            if ( InputStr.mid( 64, 5 ).toFloat() > 0. )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 64, 5 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( b_press == true ) // station pressure = 23
                        {
                            if ( InputStr.mid( 70, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.mid( 70, 4 ).simplified();
                            }
                            else
                                OutputStr += "\t";
                        }
                    }
                    else
                    {
                        if ( b_temp == true ) // air temperature = 21
                        {
                            if ( InputStr.section( " ", 10, 10 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.section( " ", 10, 10 );
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( b_relHum == true ) // relative humidity = 22
                        {
                            if ( InputStr.section( " ", 11, 11 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.section( " ", 11, 11 );
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( b_press == true ) // station pressure = 23
                        {
                            if ( InputStr.section( " ", 12, 12 ) != "-999" )
                            {
                                b_Out = true;
                                OutputStr += "\t" + InputStr.section( " ", 12, 12 );
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    if ( b_Out == true )
                        tout << OutputStr << eol;
                }
                else
                {
                    b_Stop = true;
                }
            }
        }
        else
        {
            InputStr = tin.readLine();
            ui_length     = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
        }
    }

//---------------------------------------------------------------------------------------------------

    resetProgress( i_NumOfFiles );

    fin.close();

    fout.close();

    if ( b_LR0300 == true )
    {
        fin0300.close();
        fin0300.remove();
    }

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Basic Measurements Converters, LR 0100 */

void MainWindow::doBasicMeasurementsConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    structParameter	*Parameter_0001_ptr	= NULL;
    structParameter	*Parameter_0009_ptr	= NULL;

    Parameter_0001_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Basic measurements converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = BasicMeasurementsConverter( b_Import, gsl_FilenameList.at( i ), Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Basic measurements converter was canceled" ), false, false );

// **********************************************************************************************

    if ( Parameter_0001_ptr != NULL )
    {
        delete []Parameter_0001_ptr;
        Parameter_0001_ptr = NULL;
    }

    if ( Parameter_0009_ptr != NULL )
    {
        delete []Parameter_0009_ptr;
        Parameter_0009_ptr = NULL;
    }

// **********************************************************************************************

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Basic Measurements Converters im Import-Mode, LR 0100 */

void MainWindow::doBasicMeasurementsImportConverter()
{
    doBasicMeasurementsConverter( true );
}
