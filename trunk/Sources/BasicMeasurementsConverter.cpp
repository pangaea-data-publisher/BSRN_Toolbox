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

    QString			InputStr_LR0100	= "";
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

    InputStr_LR0100 = tin.readLine().simplified();
    InputStr_LR0100 = tin.readLine().simplified();

    while ( ( tin.atEnd() == false ) && ( InputStr_LR0100.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr_LR0100  = tin.readLine().simplified();
        ui_length        = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

        if ( InputStr_LR0100.startsWith( "*" ) == false )
        {
            InputStr_LR0100 = " " + InputStr_LR0100 + " ";

            if ( InputStr_LR0100.contains( " 2 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 3 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 4 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 5 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 21 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 22 " ) ) ++n;
            if ( InputStr_LR0100.contains( " 23 " ) ) ++n;
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
        InputStr_LR0100	= tin.readLine();
        ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

        if ( ( InputStr_LR0100.startsWith( SearchString1 ) == true ) || ( InputStr_LR0100.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr_LR0100	= tin.readLine();
                ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

                if ( InputStr_LR0100.startsWith( "*" ) == false )
                {
                    if ( InputStr_LR0100.mid( 11, 4 ).simplified() != "-999" )
                        P[1] = 1;

                    if ( InputStr_LR0100.mid( 16, 5 ).simplified() != "-99.9" )
                        P[2] = 1;

                    if ( InputStr_LR0100.mid( 22, 4 ).simplified() != "-999" )
                        P[3] = 1;

                    if ( InputStr_LR0100.mid( 27, 4 ).simplified() != "-999" )
                        P[4] = 1;

                    if ( InputStr_LR0100.mid( 34, 4 ).simplified() != "-999" )
                        P[5] = 1;

                    if ( InputStr_LR0100.mid( 39, 5 ).simplified() != "-99.9" )
                        P[6] = 1;

                    if ( InputStr_LR0100.mid( 45, 4 ).simplified() != "-999" )
                        P[7] = 1;

                    if ( InputStr_LR0100.mid( 50, 4 ).simplified() != "-999" )
                        P[8] = 1;

                    InputStr_LR0100 = tin.readLine();
                    ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

                    if ( InputStr_LR0100.length() < 80 )
                    { // Raw data format
                        if ( InputStr_LR0100.mid( 11, 4 ).simplified() != "-999" )
                            P[9] = 1;

                        if ( InputStr_LR0100.mid( 16, 5 ).simplified() != "-99.9" )
                            P[10] = 1;

                        if ( InputStr_LR0100.mid( 22, 4 ).simplified() != "-999" )
                            P[11] = 1;

                        if ( InputStr_LR0100.mid( 27, 4 ).simplified() != "-999" )
                            P[12] = 1;

                        if ( InputStr_LR0100.mid( 34, 4 ).simplified() != "-999" )
                            P[13] = 1;

                        if ( InputStr_LR0100.mid( 39, 5 ).simplified() != "-99.9" )
                            P[14] = 1;

                        if ( InputStr_LR0100.mid( 45, 4 ).simplified() != "-999" )
                            P[15] = 1;

                        if ( InputStr_LR0100.mid( 50, 4 ).simplified() != "-999" )
                            P[16] = 1;

                        if ( InputStr_LR0100.mid( 58, 5 ).simplified() != "-99.9" )
                            P[17] = 1;

                        if ( InputStr_LR0100.mid( 64, 5 ).simplified() != "-99.9" )
                            P[18] = 1;

                        if ( InputStr_LR0100.mid( 70, 4 ).simplified() != "-999" )
                            P[19] = 1;
                    }
                    else
                    { // Quality checked format
                        InputStr_LR0100 = InputStr_LR0100.simplified();

                        for ( int i=1; i<=4; ++i )
                        {
                            if ( InputStr_LR0100.section( " ", i, i ).toFloat() > -99. )
                                P[8+i] = 1;
                        }

                        for ( int i=6; i<=12; ++i )
                        {
                            if ( InputStr_LR0100.section( " ", i, i ).toFloat() > -99. )
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

/*! @brief Konvertiert den LR 0100.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param b_showSelectParameterDialog soll der Select Parameter Dialog anzeigt werden?
*   @param i_Mode mit oder ohne LR0300
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

int MainWindow::BasicMeasurementsConverter( const bool b_Import, const bool b_showSelectParameterDialog, const int i_Mode, const QString& s_FilenameIn, QString& s_FilenameOut, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				err				= 0;

    int             j               = 0;
    int				k				= 0;
    int             l               = 0;
    int				n				= 0;
    int             offset          = 0;

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

    QString			InputStr_LR0100	= "";
    QString			InputStr_LR0300	= "";

    QString			SearchString1	= "*C0009";
    QString			SearchString2	= "*U0009";
    QString			SearchString3	= "*C0100";
    QString			SearchString4	= "*U0100";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";
    QString         s_DatasetID     = "";
    QString         s_Comment       = "";

    QStringList     sl_Parameter;

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

    for ( int i=0; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        Parameter_0001[i].ParameterID = -1;
        Parameter_0001[i].MethodID    = -1;

        Parameter_0009[i].ParameterID = -1;
        Parameter_0009[i].MethodID    = -1;

        P[i]   = 0;
        PoM[i] = 0;
    }

// ***********************************************************************************************************************

    if ( i_Mode != LR0300 )
        err = BasicMeasurementsTest( s_FilenameIn, P, i_NumOfFiles );

    if ( err == _ERROR_ )
        return( _ERROR_ );

    if ( err == _APPBREAK_ )
        return( _APPBREAK_ );

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

    InputStr_LR0100 = tin.readLine();
    ui_length       = InputStr_LR0100.length();

    if ( ( InputStr_LR0100.startsWith( "*C0001" ) == false ) && ( InputStr_LR0100.startsWith( "*U0001" ) == false ) )
    {
        fin.close();
        return( -40 );
    }

// ***********************************************************************************************************************

    InputStr_LR0100 = tin.readLine();
    ui_length       = ui_length + InputStr_LR0100.length();

    i_StationNumber	= InputStr_LR0100.left( 3 ).toInt();
    i_SourceID		= findInstituteID( i_StationNumber, Station_ptr );
    s_StationName	= findStationName( i_StationNumber, Station_ptr );
    s_EventLabel	= findEventLabel( i_StationNumber, Station_ptr );

// ***********************************************************************************************************************

    i_Month	= InputStr_LR0100.mid( 4, 2 ).toInt();
    i_Year	= InputStr_LR0100.mid( 7, 4 ).toInt();

    QDateTime dt = QDateTime().toUTC();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );

// ***********************************************************************************************************************

    if ( checkFilename( fi.fileName(), s_EventLabel, InputStr_LR0100.mid( 4, 2 ), InputStr_LR0100.mid( 9, 2 ) ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -41 );
    }

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( InputStr_LR0100.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr_LR0100 = tin.readLine().simplified();
        ui_length       = ui_length + InputStr_LR0100.length();

        if ( InputStr_LR0100.startsWith( "*" ) == false )
        {
            InputStr_LR0100.replace( " ", "\t" );
            InputStr_LR0100.replace( "-1", "" );

            for ( int i=0; i<8; ++i )
            {
                i_ParameterID = InputStr_LR0100.section( "\t", i, i ).toInt();

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
        if ( ( InputStr_LR0100.startsWith( "*C0002" ) == true ) || ( InputStr_LR0100.startsWith( "*U0002" ) == true ) )
        {
            InputStr_LR0100 = tin.readLine();
            ui_length       = ui_length + InputStr_LR0100.length();

            InputStr_LR0100 = tin.readLine();
            ui_length       = ui_length + InputStr_LR0100.length();

            i_PIID = findPiID( InputStr_LR0100.left( 38 ).simplified(), Staff_ptr );

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
        InputStr_LR0100 = tin.readLine();
        ui_length       = ui_length + InputStr_LR0100.length();

        if ( ( InputStr_LR0100.startsWith( "*C0004" ) == true ) || ( InputStr_LR0100.startsWith( "*U0004" ) == true ) )
        {
            for ( int i=0; i<6; i++ )
            {
                InputStr_LR0100 = tin.readLine();
                ui_length       = ui_length + InputStr_LR0100.length();
            }

            f_Latitude  = InputStr_LR0100.simplified().section( " ", 0, 0 ).toFloat() - 90.;
            f_Longitude = InputStr_LR0100.simplified().section( " ", 1, 1 ).toFloat() - 180.;

            b_Stop = true;
        }
    }

    b_Stop = false;

// ***********************************************************************************************************************

    s_FilenameOut = fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" );

    switch ( i_Mode )
    {
    case LR0100:
        s_FilenameOut.append( "_0100" );

        if ( b_LR0300 == true )
            s_FilenameOut.append( "+0300" );

        break;

    case LR0300:
        s_FilenameOut.append( "_0300" );
        break;

    default:
        s_FilenameOut.append( "_error" );
        break;
    }

    if ( b_Import == true )
        s_FilenameOut.append( "_imp.txt" );
    else
        s_FilenameOut.append( ".txt" );

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************
// Test data file and build LR0300 temp file

    QFile fin0300( fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0300_temp.txt" );

    QTextStream tin0300( &fin0300 );

    if ( b_LR0300 == true )
    {
        if ( ( err = OtherMinuteMeasurementsConverter( true, s_FilenameIn, Staff_ptr, Station_ptr, PoM, i_NumOfFiles ) ) != _NOERROR_ )
        {
            fin.close();
            fout.close();
            fout.remove();

            return( err );
        }

        if ( fin0300.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        {
            fin.close();
            fout.close();
            fout.remove();

            return( -20 );
        }
    }

// ***********************************************************************************************************************

    if ( b_Import == false )
    {
        offset = 4;

        if ( b_showSelectParameterDialog == true )
        {
            if ( i_Mode != LR0300 )
            {
                if ( checkSelectedParameter( offset, P ) == _NOERROR_ )
                    err = doSelectParametersDialog( LR0100, P );
                else
                    err = QDialog::Accepted;

                if ( err == QDialog::Accepted )
                {
                    if ( checkSelectedParameter( offset, PoM ) == _NOERROR_ )
                        err = doSelectParametersDialog( LR0100plusLR0300, PoM );
                }
            }
            else
            {
                P[0] = 0;

                for ( int i=2; i<MAX_NUM_OF_PARAMETER; i++ )
                    P[i] = 0;

                err = _NODATAFOUND_;

                if ( checkSelectedParameter( offset, PoM ) == _NOERROR_ )
                {
                    err = doSelectParametersDialog( LR0300, PoM );

                    for ( int i=0; i<=offset; i++ )
                    {
                        if ( PoM[i] > 0 )
                            P[i] = 1;
                    }
                }
            }

            if ( err != QDialog::Accepted )
            {
                fin.close();
                fout.close();
                fout.remove();

                if ( b_LR0300 == true )
                {
                    fin0300.close();
                    fin0300.remove();
                }

                switch ( err )
                {
                case QDialog::Rejected:
                    return( _CHOOSEABORTED_ );
                    break;
                case _NODATAFOUND_:
                    return( _NODATAFOUND_ );
                    break;
                default:
                    return( _ERROR_ );
                    break;
                }
            }
        }
        else
        {
            // select parameters for Quality Check Recommended V2.0

            for ( int i=0; i<=19+offset; i++ )
                P[i] = 0;

            for ( int i=0; i<=13+offset; i++ )
                PoM[i] = 0;

            P[1] = 1; P[2] = 1; P[3] = 1; // Date/Time, Latitutude, Longitude
            P[1+offset] = 1; P[5+offset] = 1; P[9+offset] = 1; P[13+offset] = 1;

            if ( b_LR0300 == true )
            {
                PoM[1] = 1; // Date/Time
                PoM[1+offset] = 1; PoM[5+offset] = 1; PoM[9+offset] = 1;
            }

            P[17+offset] = 1; P[18+offset] = 1; P[19+offset] = 1; // Air temperature, relative Humidity, Station pressure
        }

        if ( ( err = checkSelectedParameter( offset, P, PoM ) ) != _NOERROR_ )
        {
            fin.close();
            fout.close();
            fout.remove();

            if ( b_LR0300 == true )
            {
                fin0300.close();
                fin0300.remove();
            }

            return( err );
        }
    }

// ***********************************************************************************************************************

    if ( i_Mode != LR0300 )
        initProgress( i_NumOfFiles, s_FilenameIn, tr( "Basic measurements converter working (Building)..." ), 100 );
    else
        initProgress( i_NumOfFiles, s_FilenameIn, tr( "Other measurements converter working (Building)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Building" ) );

// ***********************************************************************************************************************

    if ( b_LR0300 == true )
        InputStr_LR0300 = tin0300.readLine(); // read header

// ***********************************************************************************************************************
// LR 0009

    l = 0;
    j = 0;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr_LR0100 = tin.readLine();
        ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

        if ( ( InputStr_LR0100.startsWith( SearchString1 ) == true ) || ( InputStr_LR0100.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr_LR0100 = tin.readLine();
                ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

                if ( InputStr_LR0100.startsWith( "*" ) == false )
                {
                    if ( ++l <= MAX_NUM_OF_PARAMETER )
                    {
                        Parameter_0009[l].ParameterID	= InputStr_LR0100.mid( 10, 9 ).simplified().toInt();
                        Parameter_0009[l].MethodID		= InputStr_LR0100.mid( 20, 5 ).simplified().toInt();

                        i_Day		= InputStr_LR0100.mid( 1, 2 ).toInt();
                        i_Hour		= InputStr_LR0100.mid( 4, 2 ).toInt();
                        i_Minute	= InputStr_LR0100.mid( 7, 2 ).toInt();

                        if ( i_Day == -1 ) i_Day = 1;
                        if ( i_Hour == -1 ) i_Hour = 0;
                        if ( i_Minute == -1 ) i_Minute = 0;

                        Parameter_0009[l].DateTime.setDate( QDate( i_Year, i_Month, i_Day ) );
                        Parameter_0009[l].DateTime.setTime( QTime( i_Hour, i_Minute, 0 ) );

                        n++;
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
        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetID( QString( "%1_radiation_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                s_DatasetID = DataSetID( num2str( i_DatasetID ) );
            else
                s_DatasetID = DataSetID( QString( "@%1_radiation_%2@" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ) );
        }

        sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );
        sl_Parameter.append( Parameter( num2str( 56349 ), num2str( i_PIID ), num2str( 43 ), tr( "###0" ) ) );

// ***********************************************************************************************************************
// LR0100 - build data description header

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 2 ) && ( b_gr == false ) ) // Global radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 2 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[1] > 0 )
                    sl_Parameter.append( Parameter( num2str( 31460 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ),   s_Comment ) );
                if ( P[2] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55905 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[3] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55906 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ),   s_Comment ) );
                if ( P[4] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55907 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ),   s_Comment ) );

                b_gr = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 3 ) && ( b_dr == false ) ) // Direct radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 3 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[5] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45294 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[6] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55962 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[7] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55957 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[8] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55958 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_dr = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 4 ) && ( b_dif == false ) ) // Diffuse radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 4 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[9] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45293 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[10] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55961 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[11] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55959 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[12] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55960 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_dif = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 5 ) && ( b_dlw == false ) ) // Downward long-wave radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 5 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[13] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45298 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[14] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55908 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[15] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55909 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[16] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55910 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_dlw = true;
            }
        }

// ***********************************************************************************************************************
// LR0300 - build data description header

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 131 ) && ( b_usr == false ) ) // Reflex radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 131 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( PoM[1] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55911 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[2] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55912 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( PoM[3] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55913 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[4] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55914 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_usr = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 132 ) && ( b_ul == false ) ) // Long-wave radiation, upward
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 132 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( PoM[5] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45299 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[6] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55915 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( PoM[7] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55916 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[8] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55917 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_ul = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 141 ) && ( b_netr == false ) ) // Netto radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 141 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( PoM[9] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55918 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[10] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55919 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( PoM[11] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55920 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( PoM[12] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55921 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_netr = true;
            }
        }

// ***********************************************************************************************************************
// LR0100 - Temperature - build data description header, relative humidity, station pressure

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 21 ) && ( P[17] > 0 ) && ( b_temp == false ) )
            {
                sl_Parameter.append( Parameter( num2str( 48820 ), num2str( i_PIID ), tr( "4722" ), tr( "###0.0" ) ) );
                b_temp = true;
            }
        }

// ***********************************************************************************************************************
// LR0100 - Relative humidity - build data description header

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 22 ) && ( P[18] > 0 ) && ( b_relHum == false )  )
            {
                sl_Parameter.append( Parameter( num2str( 2219 ), num2str( i_PIID ), tr( "5039" ), tr( "###0.0" ) ) );
                b_relHum = true;
            }
        }

// ***********************************************************************************************************************
// LR0100 - Station pressure - build data description header

        for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
        {
            if ( ( Parameter_0001[i].ParameterID == 23 ) && ( P[19] > 0 ) && ( b_press == false )  )
            {
                sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), tr( "359" ), tr( "###0.0" ) ) );
                b_press = true;
            }
        }

// ***********************************************************************************************************************

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
    }

// ***********************************************************************************************************************
// write data description header

    if ( ( b_Import == true ) && ( sl_Parameter.count() > 2 ) )
    {
        tout << OpenDataDescriptionHeader();
        tout << s_DatasetID;
        tout << ReferenceOtherVersion( s_EventLabel, Reference_ptr, dt );
        tout << AuthorIDs( num2str( i_PIID ) );
        tout << SourceID( num2str( i_SourceID ) );

        if ( b_LR0300 == true )
            tout << DatasetTitle( tr( "Basic and other measurements of radiation at" ), s_StationName, dt );
        else
            tout << DatasetTitle( tr( "Basic measurements of radiation at" ), s_StationName, dt );

        tout << ExportFilename( s_EventLabel, tr( "radiation" ), dt );
        tout << EventLabel( s_EventLabel );
        tout << Parameter( sl_Parameter );
        tout << ProjectIDs( num2str( 4094 ) );
        tout << TopologicTypeID( num2str( 8 ) );
        tout << StatusID( num2str( 4 ) );
        tout << UserIDs( num2str( 1144 ) );
        tout << LoginID( num2str( 3 ) );
        tout << CloseDataDescriptionHeader();
    }

// ***********************************************************************************************************************
// write data header

    tout << writeGeocodeHeader( b_Import, P );

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 2 ) && ( b_gr == false ) ) // Global radiation
        {
            if ( b_Import == true )
            {
                if ( P[1] > 0 ) tout << "\t31460";
                if ( P[2] > 0 ) tout << "\t55905";
                if ( P[3] > 0 ) tout << "\t55906";
                if ( P[4] > 0 ) tout << "\t55907";
            }
            else
            {
                if ( P[1+offset] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation [W/m**2]";
                if ( P[2+offset] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, standard deviation [W/m**2]";
                if ( P[3+offset] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, minimum [W/m**2]";
                if ( P[4+offset] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, maximum [W/m**2]";
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
                if ( P[5] > 0 ) tout << "\t45294";
                if ( P[6] > 0 ) tout << "\t55962";
                if ( P[7] > 0 ) tout << "\t55957";
                if ( P[8] > 0 ) tout << "\t55958";
            }
            else
            {
                if ( P[5+offset] > 0 ) tout << "\tDirect radiation [W/m**2]";
                if ( P[6+offset] > 0 ) tout << "\tDirect radiation, standard deviation [W/m**2]";
                if ( P[7+offset] > 0 ) tout << "\tDirect radiation, minimum [W/m**2]";
                if ( P[8+offset] > 0 ) tout << "\tDirect radiation, maximum [W/m**2]";
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
                if ( P[9]  > 0 ) tout << "\t45293";
                if ( P[10] > 0 ) tout << "\t55961";
                if ( P[11] > 0 ) tout << "\t55959";
                if ( P[12] > 0 ) tout << "\t55960";
            }
            else
            {
                if ( P[9+offset]  > 0 ) tout << "\tDiffuse radiation [W/m**2]";
                if ( P[10+offset] > 0 ) tout << "\tDiffuse radiation, standard deviation [W/m**2]";
                if ( P[11+offset] > 0 ) tout << "\tDiffuse radiation, minimum [W/m**2]";
                if ( P[12+offset] > 0 ) tout << "\tDiffuse radiation, maximum [W/m**2]";
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
                if ( P[13] > 0 ) tout << "\t45298";
                if ( P[14] > 0 ) tout << "\t55908";
                if ( P[15] > 0 ) tout << "\t55909";
                if ( P[16] > 0 ) tout << "\t55910";
            }
            else
            {
                if ( P[13+offset] > 0 ) tout << "\tLong-wave downward radiation [W/m**2]";
                if ( P[14+offset] > 0 ) tout << "\tLong-wave downward radiation, standard deviation [W/m**2]";
                if ( P[15+offset] > 0 ) tout << "\tLong-wave downward radiation, minimum [W/m**2]";
                if ( P[16+offset] > 0 ) tout << "\tLong-wave downward radiation, maximum [W/m**2]";
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
                    if ( PoM[1] > 0 ) tout << "\t55911";
                    if ( PoM[2] > 0 ) tout << "\t55912";
                    if ( PoM[3] > 0 ) tout << "\t55913";
                    if ( PoM[4] > 0 ) tout << "\t55914";
                }
                else
                {
                    if ( PoM[1+offset] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation [W/m**2]";
                    if ( PoM[2+offset] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, standard deviation [W/m**2]";
                    if ( PoM[3+offset] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, minimum [W/m**2]";
                    if ( PoM[4+offset] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, maximum [W/m**2]";
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
                    if ( PoM[5] > 0 ) tout << "\t45299";
                    if ( PoM[6] > 0 ) tout << "\t55915";
                    if ( PoM[7] > 0 ) tout << "\t55916";
                    if ( PoM[8] > 0 ) tout << "\t55917";
                }
                else
                {
                    if ( PoM[5+offset] > 0 ) tout << "\tLong-wave upward radiation [W/m**2]";
                    if ( PoM[6+offset] > 0 ) tout << "\tLong-wave upward radiation, standard deviation [W/m**2]";
                    if ( PoM[7+offset] > 0 ) tout << "\tLong-wave upward radiation, minimum [W/m**2]";
                    if ( PoM[8+offset] > 0 ) tout << "\tLong-wave upward radiation, maximum [W/m**2]";
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
                    if ( PoM[9]  > 0 ) tout << "\t55918";
                    if ( PoM[10] > 0 ) tout << "\t55919";
                    if ( PoM[11] > 0 ) tout << "\t55920";
                    if ( PoM[12] > 0 ) tout << "\t55921";
                }
                else
                {
                    if ( PoM[9+offset]  > 0 ) tout << "\tNet radiation [W/m**2]";
                    if ( PoM[10+offset] > 0 ) tout << "\tNet radiation, standard deviation [W/m**2]";
                    if ( PoM[11+offset] > 0 ) tout << "\tNet radiation, minimum [W/m**2]";
                    if ( PoM[12+offset] > 0 ) tout << "\tNet radiation, maximum [W/m**2]";
                }

                b_netr = true;
            }
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 21 ) && ( b_temp == false ) ) // Air temperature
        {
            if ( b_Import == true )
            {
                if ( P[17] > 0 )
                    tout << "\t48820";
            }
            else
            {
                if ( P[17+offset] > 0 )
                   tout << "\tAir temperature [deg C]";
            }

            b_temp = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 22 ) && ( b_relHum == false ) ) // relative humidity
        {
            if ( b_Import == true )
            {
                if ( P[18] > 0 )
                    tout << "\t2219";
            }
            else
            {
                if ( P[18+offset] > 0 )
                    tout << "\tHumidity, relative [%]";
            }

            b_relHum = true;
        }
    }

    for ( int i=1; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        if ( ( Parameter_0001[i].ParameterID == 23 ) && ( b_press == false ) ) // Station pressure
        {
            if ( b_Import == true )
            {
                if ( P[19] > 0 )
                    tout << "\t48823";
            }
            else
            {
                if ( P[19+offset] > 0 )
                    tout << "\tStation pressure [hPa]";
            }

            b_press = true;
        }
    }

    tout << eol;

// ***********************************************************************************************************************
// write data

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr_LR0100.startsWith( SearchString3 ) == true ) || ( InputStr_LR0100.startsWith( SearchString4 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr_LR0100 = tin.readLine();
                ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

                if ( InputStr_LR0100.startsWith( "*" ) == false )
                {
                    i_Day		= InputStr_LR0100.mid( 1, 2 ).toInt();
                    i_Minute	= InputStr_LR0100.mid( 4, 4 ).toInt();

                    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                    dt.setTime( QTime( 0, 0, 0 ) );
                    dt = dt.addSecs( i_Minute*60 );

                    b_Out = false;

                    OutputStr = buildGeocodeEntries( b_Import, P, dt, s_EventLabel, f_Latitude, f_Longitude );

                    if ( b_gr == true ) // global radiation = 2
                    {
                        if ( P[1+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 11, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 11, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 16, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 16, 5 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 22, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 22, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 27, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 27, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }
                    }

                    if ( b_dr == true ) // direct radiation = 3
                    {
                        if ( P[5+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 34, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 34, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 39, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 39, 5 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 45, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 45, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8+offset] > 0 )
                        {
                            if ( InputStr_LR0100.mid( 50, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 50, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }
                    }

                    InputStr_LR0100 = tin.readLine();
                    ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );

                    if ( InputStr_LR0100.length() < 80 )
                    { // Raw data format
                        if ( b_dif == true ) // diffuse radiation = 4
                        {
                            if ( P[9+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 11, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 11, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[10+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 16, 5 ).simplified() != "-99.9" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 16, 5 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[11+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 22, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 22, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[12+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 27, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 27, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }

                        if ( b_dlw == true ) // downward long-wave radiation = 5
                        {
                            if ( P[13+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 34, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 34, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[14+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 39, 5 ).simplified() != "-99.9" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 39, 5 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[15+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 45, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 45, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }

                            if ( P[16+offset] > 0 )
                            {
                                if ( InputStr_LR0100.mid( 50, 4 ).simplified() != "-999" )
                                {
                                    b_Out = true;
                                    OutputStr.append( "\t" + InputStr_LR0100.mid( 50, 4 ).simplified() );
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }
                    }
                    else
                    { // Quality checked format
                        b_QualityChecked = true;

                        InputStr_LR0100 = InputStr_LR0100.simplified();

                        if ( b_dif == true ) // diffuse radiation = 4
                        {
                            for ( int i=1; i<=4; ++i )
                            {
                                if ( P[8+i+offset] > 0 )
                                {
                                    if ( InputStr_LR0100.section( " ", i, i ).toFloat() > -99. )
                                    {
                                        b_Out = true;
                                        OutputStr.append( "\t" + InputStr_LR0100.section( " ", i, i ) );
                                    }
                                    else
                                        OutputStr.append( "\t" );
                                }
                            }
                        }

                        if ( b_dlw == true ) // downward long-wave radiation = 5
                        {
                            for ( int i=6; i<=9; ++i )
                            {
                                if ( P[7+i+offset] > 0 )
                                {
                                    if ( InputStr_LR0100.section( " ", i, i ).toFloat() > -99. )
                                    {
                                        b_Out = true;
                                        OutputStr.append( "\t" + InputStr_LR0100.section( " ", i, i ) );
                                    }
                                    else
                                        OutputStr.append( "\t" );
                                }
                            }
                        }
                    }

                    if ( ( b_LR0300 == true ) && ( ( b_usr == true ) || ( b_ul == true ) || ( b_netr == true ) ) && ( tin0300.atEnd() == false ) )
                    {
                        InputStr_LR0300 = tin0300.readLine();

                        if ( InputStr_LR0300.section( "\t", 0, 0 ) == dt.toString( "yyyy-MM-ddThh:mm" ) )
                        {
                            if ( b_usr == true ) // Reflex radiation = 131
                            {
                                b_Out = true;

                                if ( PoM[1+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 1, 1 ) );

                                if ( PoM[2+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 2, 2 ) );

                                if ( PoM[3+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 3, 3 ) );

                                if ( PoM[4+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 4, 4 ) );
                            }

                            if ( b_ul == true ) // Long-wave radiation, upward = 132
                            {
                                b_Out = true;

                                if ( PoM[5+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 5, 5 ) );

                                if ( PoM[6+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 6, 6 ) );

                                if ( PoM[7+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 7, 7 ) );

                                if ( PoM[8+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 8, 8 ) );
                            }

                            if ( b_netr == true ) // Netto radiation = 141
                            {
                                b_Out = true;

                                if ( PoM[9+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t",  9,  9 ) );

                                if ( PoM[10+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 10, 10 ) );

                                if ( PoM[11+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 11, 11 ) );

                                if ( PoM[12+offset] > 0 )
                                    OutputStr.append( "\t" + InputStr_LR0300.section( "\t", 12, 12 ) );
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
                        if ( ( P[17+offset] == 1 ) && ( b_temp == true ) ) // air temperature = 21
                        {
                            if ( InputStr_LR0100.mid( 58, 5 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 58, 5 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( ( P[18+offset] == 1 ) && ( b_relHum == true ) ) // relative humidity = 22
                        {
                            if ( InputStr_LR0100.mid( 64, 5 ).toFloat() > 0. )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 64, 5 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( ( P[19+offset] == 1 ) && ( b_press == true ) ) // station pressure = 23
                        {
                            if ( InputStr_LR0100.mid( 70, 4 ).simplified() != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.mid( 70, 4 ).simplified() );
                            }
                            else
                                OutputStr.append( "\t" );
                        }
                    }
                    else
                    {
                        if ( ( P[17+offset] == 1 ) && ( b_temp == true ) ) // air temperature = 21
                        {
                            if ( InputStr_LR0100.section( " ", 10, 10 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.section( " ", 10, 10 ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( ( P[18+offset] == 1 ) && ( b_relHum == true ) ) // relative humidity = 22
                        {
                            if ( InputStr_LR0100.section( " ", 11, 11 ).simplified() != "-99.9" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.section( " ", 11, 11 ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( ( P[19+offset] == 1 ) && ( b_press == true ) ) // station pressure = 23
                        {
                            if ( InputStr_LR0100.section( " ", 12, 12 ) != "-999" )
                            {
                                b_Out = true;
                                OutputStr.append( "\t" + InputStr_LR0100.section( " ", 12, 12 ) );
                            }
                            else
                                OutputStr.append( "\t" );
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
            InputStr_LR0100 = tin.readLine();
            ui_length       = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr_LR0100 );
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

    if ( ( err = checkSelectedParameter( offset, PoM ) ) != _NOERROR_ )
    {
        QString s_FilenameLR0100 = s_FilenameOut;
        QFile::rename( s_FilenameOut, s_FilenameLR0100.replace( "0100+0300", "0100" ) );
    }

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2003-08-02

/*! @brief Steuerung des Basic Measurements Converters, LR 0100 */

void MainWindow::doBasicMeasurementsConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QString s_FilenameOut   = "";

// **********************************************************************************************

    structParameter	*Parameter_0001_ptr	= NULL;
    structParameter	*Parameter_0009_ptr	= NULL;

    Parameter_0001_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        if ( b_Import == true )
            readBsrnReferenceIDs( false );

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Basic measurements converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = BasicMeasurementsConverter( b_Import, true, LR0100, gsl_FilenameList.at( i ), s_FilenameOut, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

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
// 2003-08-02

/*! @brief Steuerung des Basic Measurements Converters im Import-Mode, LR 0100 */

void MainWindow::doBasicMeasurementsImportConverter()
{
    doBasicMeasurementsConverter( true );
}
