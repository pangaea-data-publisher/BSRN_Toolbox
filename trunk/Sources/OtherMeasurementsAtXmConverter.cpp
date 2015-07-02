/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Testet den LR 3xxx.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param P Pointer auf ein Array von Integern
*   @param i_Height Hoehe des Messortes ueber Grund
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::OtherMeasurementsAtXmTest( const QString& s_FilenameIn, int *P, const int i_Height, const int i_NumOfFiles )
{
    int				err				= _NOERROR_;
    int				i_P_sum			= 0;

    QString			InputStr		= "";
    QString			SearchString1	= "*C3" + InputStr.sprintf( "%03d", i_Height );
    QString			SearchString2	= "*U3" + InputStr.sprintf( "%03d", i_Height );
    QString			d				= "";

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

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Other measurements at " ) + num2str( i_Height ) + tr( " m converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

// ***********************************************************************************************************************

    InputStr = tin.readLine().simplified();
    InputStr = tin.readLine().simplified();

    while ( ( tin.atEnd() == false ) && ( InputStr.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr = tin.readLine().simplified();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( InputStr.startsWith( "*" ) == false )
        {
            InputStr.append( " " );

            if ( InputStr.contains( d.sprintf( " 2%06d ", i_Height*100 ) ) )	err++;
            if ( InputStr.contains( d.sprintf( " 131%06d ", i_Height*100 ) ) )	err++;
            if ( InputStr.contains( d.sprintf( " 5%06d ", i_Height*100 ) ) )	err++;
            if ( InputStr.contains( d.sprintf( " 132%06d ", i_Height*100 ) ) )	err++;
            if ( InputStr.contains( d.sprintf( " 21%06d ", i_Height*100 ) ) )	err++;
            if ( InputStr.contains( d.sprintf( " 22%06d ", i_Height*100 ) ) )	err++;
        }
    }

    if ( err == _NOERROR_ )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( _NODATAFOUND_ );
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
                    if ( InputStr.mid( 11, 4 ).simplified().toInt() > -90 )
                        P[1] = 1;

                    if ( InputStr.mid( 16, 5 ).simplified().toFloat() > -90. )
                        P[2] = 1;

                    if ( InputStr.mid( 22, 4 ).simplified().toInt() > -90 )
                        P[3] = 1;

                    if ( InputStr.mid( 27, 4 ).simplified().toInt() > -90 )
                        P[4] = 1;

                    if ( InputStr.mid( 34, 4 ).simplified().toInt() > -90 )
                        P[5] = 1;

                    if ( InputStr.mid( 39, 5 ).simplified().toFloat() > -90. )
                        P[6] = 1;

                    if ( InputStr.mid( 45, 4 ).simplified().toInt() > -90 )
                        P[7] = 1;

                    if ( InputStr.mid( 50, 4 ).simplified().toInt() > -90 )
                        P[8] = 1;

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( InputStr.mid( 11, 4 ).simplified().toInt() > -90 )
                        P[9] = 1;

                    if ( InputStr.mid( 16, 5 ).simplified().toFloat() > -90. )
                        P[10] = 1;

                    if ( InputStr.mid( 22, 4 ).simplified().toInt() > -90 )
                        P[11] = 1;

                    if ( InputStr.mid( 27, 4 ).simplified().toInt() > -90 )
                        P[12] = 1;

                    if ( InputStr.mid( 34, 4 ).simplified().toInt() > -90 )
                        P[13] = 1;

                    if ( InputStr.mid( 39, 5 ).simplified().toFloat() > -90. )
                        P[14] = 1;

                    if ( InputStr.mid( 45, 4 ).simplified().toInt() > -90 )
                        P[15] = 1;

                    if ( InputStr.mid( 50, 4 ).simplified().toInt() > -90 )
                        P[16] = 1;

                    if ( InputStr.mid( 58, 5 ).simplified().toFloat() > -90. )
                        P[17] = 1;

                    if ( InputStr.mid( 64, 5 ).simplified().toFloat() > -90. )
                        P[18] = 1;

                    i_P_sum = 0;

                    for ( int i=1; i<=18 ; ++i )
                        i_P_sum += P[i];

                    if ( i_P_sum == 18 )
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

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-11-07

/*! @brief Konvertiert den LR 1100.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param i_Height Hoehe des Messortes ueber Grund
*   @param Parameter_0001 Pointer auf Array aller Parameter aus LR 0001
*   @param Parameter_0009 Pointer auf Array aller Parameter aus LR 0009
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::OtherMeasurementsAtXmConverter( const bool b_Import, const QString& s_FilenameIn, const int i_Height, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				err				= 0;

    int				i				= 0;
    int				j				= 1;
    int				k				= 0;
    int				n			= 0;

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
    QString			InputStr		= "";

    QString			SearchString1	= "*C0009";
    QString			SearchString2	= "*U0009";
    QString			SearchString3	= "*C3" + InputStr.sprintf( "%03d", i_Height );
    QString			SearchString4	= "*U3" + InputStr.sprintf( "%03d", i_Height );

    QString			d				= "";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";
    QString         s_DatasetID     = "";
    QString         s_Comment       = "";

    QStringList     sl_Parameter;

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;
    bool			b_Out			= false;

    int				P[MAX_NUM_OF_PARAMETER+1];

    bool			b_gr			= false;
    bool			b_swu			= false;
    bool			b_lwd			= false;
    bool			b_lwu			= false;

// ***********************************************************************************************************************

    err = OtherMeasurementsAtXmTest( s_FilenameIn, P, i_Height, i_NumOfFiles );

    if ( err == _NODATAFOUND_ )
        return( _NOERROR_ );

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

    QFile fin( s_FilenameIn );
    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, QString( "Other measurements at %1 m converter working (Building)..." ).arg( i_Height ), 100 );

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

    while ( ( tin.atEnd() == false ) && ( InputStr.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr = tin.readLine().simplified();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( InputStr.startsWith( "*" ) == false )
        {
            InputStr.replace( " ", "\t" );
            InputStr.replace( "-1", "" );

            for ( int i=0; i<8; ++i )
            {
                i_ParameterID = InputStr.section( "\t", i, i ).toInt();

                if ( i_ParameterID > 0 )
                    Parameter_0001[i+j].ParameterID = i_ParameterID;
            }

            j += 8;
        }
    }

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr.startsWith( "*C0002" ) == true ) || ( InputStr.startsWith( "*U0002" ) == true ) )
        {
            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            InputStr = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            i_PIID = findPiId( InputStr.left( 38 ).simplified(), Staff_ptr );

            b_Stop = true;
        }
        else
        {
            resetProgress( i_NumOfFiles );
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

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_" + d.sprintf( "3%03d", i_Height );

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
// LR 0009

    i = 0;
    j = 0;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr = tin.readLine();
                ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    if ( ++i <= MAX_NUM_OF_PARAMETER )
                    {
                        Parameter_0009[i].ParameterID	= InputStr.mid( 10, 9 ).simplified().toInt();
                        Parameter_0009[i].MethodID		= InputStr.mid( 20, 5 ).simplified().toInt();

                        i_Day		= InputStr.mid( 1, 2 ).toInt();
                        i_Hour		= InputStr.mid( 4, 2 ).toInt();
                        i_Minute	= InputStr.mid( 7, 2 ).toInt();

                        if ( i_Day == -1 ) i_Day = 1;
                        if ( i_Hour == -1 ) i_Hour = 0;
                        if ( i_Minute == -1 ) i_Minute = 0;

                        Parameter_0009[i].DateTime.setDate( QDate( i_Year, i_Month, i_Day ) );
                        Parameter_0009[i].DateTime.setTime( QTime( i_Hour, i_Minute, 0 ) );

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
// build data description header

    b_Stop = false;

    if ( b_Import == true )
    {
        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetId( QString( "%1_radiation_%2m_%3" ).arg( s_EventLabel ).arg( i_Height ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                s_DatasetID = DataSetID( num2str( i_DatasetID ) );
            else
                s_DatasetID = DataSetID( QString( "%1_radiation_%2m_%3" ).arg( s_EventLabel ).arg( i_Height ).arg( dt.toString( "yyyy-MM" ) ) );
        }

        sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );
        sl_Parameter.append( Parameter( num2str( 56349 ), num2str( i_PIID ), num2str( 43 ), tr( "###0" ) ) );

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == d.sprintf( "2%06d", i_Height*100 ).toInt() ) && ( b_gr == false ) ) // Global radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == d.sprintf( "2%06d", i_Height*100 ).toInt() ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + Parameter_0009[k].MethodID + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

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
            if ( ( Parameter_0009[i].ParameterID == d.sprintf( "131%06d", i_Height*100 ).toInt() ) && ( b_swu == false ) ) // short-wave upward radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == d.sprintf( "131%06d", i_Height*100 ).toInt() ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + Parameter_0009[k].MethodID + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[5] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55911 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[6] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55912 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[7] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55913 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[8] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55914 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_swu = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == d.sprintf( "5%06d", i_Height*100 ).toInt() ) && ( b_lwd == false ) ) // long-wave downward radiation
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == d.sprintf( "5%06d", i_Height*100 ).toInt() ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + Parameter_0009[k].MethodID + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[9] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45298 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[10] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55908 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[11] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55909 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[12] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55910 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_lwd = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == d.sprintf( "132%06d", i_Height*100 ).toInt() ) && ( b_lwu == false ) ) // long-wave upward
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == d.sprintf( "132%06d", i_Height*100 ).toInt() ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + Parameter_0009[k].MethodID + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[13] > 0 )
                    sl_Parameter.append( Parameter( num2str( 45299 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[14] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55915 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[15] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55916 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );
                if ( P[16] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55917 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0" ), s_Comment ) );

                b_lwu = true;
            }
        }

        if ( P[17] > 0 )
            sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), tr( "4722" ), tr( "###0.0" ) ) );

        if ( P[18] > 0 )
            sl_Parameter.append( Parameter( num2str( 2219 ), num2str( i_PIID ), tr( "5039" ), tr( "###0.0" ) ) );

        b_gr		= false;
        b_swu		= false;
        b_lwd		= false;
        b_lwu		= false;
    }

// ***********************************************************************************************************************
// write data description header

    if ( ( b_Import == true ) && ( sl_Parameter.count() > 2 ) )
    {
        tout << OpenDataDescriptionHeader();
        tout << s_DatasetID;
        tout << AuthorIDs( num2str( i_PIID ) );
        tout << SourceID( num2str( i_SourceID ) );
        tout << DatasetTitle( QString( "Other measurements at %1 m from" ).arg( i_Height ), s_StationName, dt );
        tout << ReferenceOtherVersion( s_EventLabel, dt );
        tout << ExportFilename( s_EventLabel, QString( "radiation_%1m" ).arg( i_Height ), dt );
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

    if ( b_Import == true )
        tout << "1599\t56349"; // Date/Time, Height
    else
        tout << "Station\tDate/Time\tLatitude\tLongitude\tHeight above ground [m]";

// ***********************************************************************************************************************
// write data

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == d.sprintf( "2%06d", i_Height*100 ).toInt() ) && ( b_gr == false ) ) // Global radiation
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
                if ( P[1] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation [W/m**2]";
                if ( P[2] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, standard deviation [W/m**2]";
                if ( P[3] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, minimum [W/m**2]";
                if ( P[4] > 0 ) tout << "\tShort-wave downward (GLOBAL) radiation, maximum [W/m**2]";
            }

            b_gr = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == d.sprintf( "131%06d", i_Height*100 ).toInt() ) && ( b_swu == false ) ) // Short-wave upward (REFLEX) radiation
        {
            if ( b_Import == true )
            {
                if ( P[5] > 0 ) tout << "\t55911";
                if ( P[6] > 0 ) tout << "\t55912";
                if ( P[7] > 0 ) tout << "\t55913";
                if ( P[8] > 0 ) tout << "\t55914";
            }
            else
            {
                if ( P[5] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation [W/m**2]";
                if ( P[6] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, standard deviation [W/m**2]";
                if ( P[7] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, minimum [W/m**2]";
                if ( P[8] > 0 ) tout << "\tShort-wave upward (REFLEX) radiation, maximum [W/m**2]";
            }

            b_swu = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == d.sprintf( "5%06d", i_Height*100 ).toInt() ) && ( b_lwd == false ) ) // Long-wave downward radiation
        {
            if ( b_Import == true )
            {
                if ( P[9]  > 0 ) tout << "\t45298";
                if ( P[10] > 0 ) tout << "\t55908";
                if ( P[11] > 0 ) tout << "\t55909";
                if ( P[12] > 0 ) tout << "\t55910";
            }
            else
            {
                if ( P[9] > 0 ) tout << "\tLong-wave downward radiation [W/m**2]";
                if ( P[10] > 0 ) tout << "\tLong-wave downward radiation, standard deviation [W/m**2]";
                if ( P[11] > 0 ) tout << "\tLong-wave downward radiation, minimum [W/m**2]";
                if ( P[12] > 0 ) tout << "\tLong-wave downward radiation, maximum [W/m**2]";
            }

            b_lwd = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == d.sprintf( "132%06d", i_Height*100 ).toInt() ) && ( b_lwu == false ) ) // Long-wave upward radiation
        {
            if ( b_Import == true )
            {
                if ( P[13] > 0 ) tout << "\t45299";
                if ( P[14] > 0 ) tout << "\t55915";
                if ( P[15] > 0 ) tout << "\t55916";
                if ( P[16] > 0 ) tout << "\t55917";
            }
            else
            {
                if ( P[13] > 0 ) tout << "\tLong-wave upward radiation [W/m**2]";
                if ( P[14] > 0 ) tout << "\tLong-wave upward radiation, standard deviation [W/m**2]";
                if ( P[15] > 0 ) tout << "\tLong-wave upward radiation, minimum [W/m**2]";
                if ( P[16] > 0 ) tout << "\tLong-wave upward radiation, maximum [W/m**2]";
            }

            b_lwu = true;
        }
    }

    if ( P[17] > 0 )
    {
        if ( b_Import == true )
            tout << "\t4610";
        else
            tout << "\tTemperature, air [deg C]";
    }


    if ( P[18] > 0 )
    {
        if ( b_Import == true )
            tout << "\t2219";
        else
            tout << "\tHumidity, relative [%]";
    }

    tout << eol;

// *************************************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        if ( ( InputStr.startsWith( SearchString3 ) == true ) || ( InputStr.startsWith( SearchString4 ) == true ) )
        {
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

                    b_Out = false;

                    if ( b_Import == false )
                        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ) + "\t" + num2str( i_Height );
                    else
                        OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( i_Height );

                    if ( b_gr == true )
                    {
                        if ( P[1] > 0 )
                        {
                            if ( InputStr.mid( 11, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 11, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[2] > 0 )
                        {
                            if ( InputStr.mid( 16, 5 ).simplified().toFloat() > -90. )
                            {
                                OutputStr += "\t" + InputStr.mid( 16, 5 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[3] > 0 )
                        {
                            if ( InputStr.mid( 22, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 12, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[4] > 0 )
                        {
                            if ( InputStr.mid( 27, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 27, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    if ( b_swu == true )
                    {
                        if ( P[5] > 0 )
                        {
                            if ( InputStr.mid( 34, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 34, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[6] > 0 )
                        {
                            if ( InputStr.mid( 39, 5 ).simplified().toFloat() > -90. )
                            {
                                OutputStr += "\t" + InputStr.mid( 39, 5 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[7] > 0 )
                        {
                            if ( InputStr.mid( 45, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 45, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[8] > 0 )
                        {
                            if ( InputStr.mid( 50, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 50, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( b_lwd == true )
                    {
                        if ( P[9] > 0 )
                        {
                            if ( InputStr.mid( 11, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 11, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[10] > 0 )
                        {
                            if ( InputStr.mid( 16, 5 ).simplified().toFloat() > -90. )
                            {
                                OutputStr += "\t" + InputStr.mid( 16, 5 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[11] > 0 )
                        {
                            if ( InputStr.mid( 22, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 12, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[12] > 0 )
                        {
                            if ( InputStr.mid( 27, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 27, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    if ( b_lwu == true )
                    {
                        if ( P[13] > 0 )
                        {
                            if ( InputStr.mid( 34, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 34, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[14] > 0 )
                        {
                            if ( InputStr.mid( 39, 5 ).simplified().toFloat() > -90. )
                            {
                                OutputStr += "\t" + InputStr.mid( 39, 5 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[15] > 0 )
                        {
                            if ( InputStr.mid( 45, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 45, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }

                        if ( P[16] > 0 )
                        {
                            if ( InputStr.mid( 50, 4 ).simplified().toInt() > -90 )
                            {
                                OutputStr += "\t" + InputStr.mid( 50, 4 ).simplified();
                                b_Out = true;
                            }
                            else
                                OutputStr += "\t";
                        }
                    }

                    if ( P[17] > 0 ) // Temperature, air [m]
                    {
                        if ( InputStr.mid( 58, 5 ).simplified().toFloat() > -90. )
                        {
                            OutputStr += "\t" + InputStr.mid( 58, 5 ).simplified();
                            b_Out = true;
                        }
                        else
                            OutputStr += "\t";
                    }

                    if ( P[18] > 0 ) // Humidity, relative [%]
                    {
                        if ( InputStr.mid( 64, 5 ).simplified().toFloat() > -90. )
                        {
                            OutputStr += "\t" + InputStr.mid( 64, 5 ).simplified();
                            b_Out = true;
                        }
                        else
                            OutputStr += "\t";
                    }

                    if ( b_Out == true )
                        tout << OutputStr << eol;

//					b_Stop = true;
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

/*! @brief Steuerung des Other Measurements at X m Converters, LR 3xxx */

void MainWindow::doOtherMeasurementsAtXmConverter( const bool b_Import, const int i_Height )
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
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Other measurements at X m converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = OtherMeasurementsAtXmConverter( b_Import, gsl_FilenameList.at( i ), i_Height, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Other measurements at X m converter was canceled" ), false, false );

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

/*! @brief Steuerung des Other Measurements at 10 m Converters, LR 3010 */

void MainWindow::doOtherMeasurementsAt10mConverter()
{
    doOtherMeasurementsAtXmConverter( false, 10 );
}

// **********************************************************************************************

/*! @brief Steuerung des Other Measurements at 10 m Converters im Import-Mode, LR 3010 */

void MainWindow::doOtherMeasurementsAt10mImportConverter()
{
    doOtherMeasurementsAtXmConverter( true, 10 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Other Measurements at 30 m Converters, LR 3030 */

void MainWindow::doOtherMeasurementsAt30mConverter()
{
    doOtherMeasurementsAtXmConverter( false, 30 );
}

// **********************************************************************************************

/*! @brief Steuerung des Other Measurements at 30 m Converters im Import-Mode, LR 3030 */

void MainWindow::doOtherMeasurementsAt30mImportConverter()
{
    doOtherMeasurementsAtXmConverter( true, 30 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Other Measurements at 300 m Converters, LR 3300 */

void MainWindow::doOtherMeasurementsAt300mConverter()
{
    doOtherMeasurementsAtXmConverter( false, 300 );
}

// **********************************************************************************************

/*! @brief Steuerung des Other Measurements at 300 m Converters im Import-Mode, LR 3300 */

void MainWindow::doOtherMeasurementsAt300mImportConverter()
{
    doOtherMeasurementsAtXmConverter( true, 300 );
}
