/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Testet den LR0500.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param P Pointer auf ein Array von Integern
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::UltraVioletMeasurementsTest( const QString &s_FilenameIn, int *P, const int i_NumOfFiles )
{
    int				i_P_sum			= 0;

    QString			InputStr		= "";
    QString			SearchString1	= "*C0500";
    QString			SearchString2	= "*U0500";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool            b_hasRecord     = false;
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

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Ultra-violet measurements converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

// ***********************************************************************************************************************

    InputStr  = tin.readLine().simplified();
    InputStr  = tin.readLine().simplified();

    while ( ( tin.atEnd() == false ) && ( InputStr.startsWith( "*" ) == false ) )
    {
        // ID of quantity
        InputStr  = tin.readLine().simplified();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( InputStr.startsWith( "*" ) == false )
        {
            InputStr.append( " " );

            if ( InputStr.contains( " 121 " ) ) b_hasRecord = true;
            if ( InputStr.contains( " 122 " ) ) b_hasRecord = true;
            if ( InputStr.contains( " 123 " ) ) b_hasRecord = true;
            if ( InputStr.contains( " 124 " ) ) b_hasRecord = true;
            if ( InputStr.contains( " 125 " ) ) b_hasRecord = true;
        }
    }

    while ( ( b_hasRecord == true ) && ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
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
                    for ( int i=1; i<=8; ++i )
                    {
                        if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                            P[i] = 1;
                    }

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    for ( int i=1; i<=12; ++i )
                    {
                        if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                            P[i+8] = 1;
                    }

                    i_P_sum = 0;

                    for ( int i=1; i<=20 ; ++i )
                        i_P_sum += P[i];

                    if ( i_P_sum == 20 )
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

    if ( b_hasRecord == true )
    {
        i_P_sum = 0;

        for ( int i=1; i<=20 ; ++i )
            i_P_sum += P[i];

        if ( i_P_sum == 0 )
        {
            QFileInfo fin( s_FilenameIn );
            QString s_Message = tr( "No LR0500 data found. Something must be wrong. Please check the station-to-archive file:\n\n    " ) + fin.fileName();
            QMessageBox::warning( this, getApplicationName( true ), s_Message  );

            return( _APPBREAK_ );
        }
        else
        {
            return( _NOERROR_ );
        }
    }

    return( _APPBREAK_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-11-07

/*! @brief Konvertiert den LR0500.
*
*   @param b_showSelectParameterDialog soll der Select Parameter Dialog anzeigt werden?
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param Parameter_0001 Pointer auf Array aller Parameter aus LR0001
*   @param Parameter_0009 Pointer auf Array aller Parameter aus LR0009
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::UltraVioletMeasurementsConverter( const bool b_Import, const bool b_showSelectParameterDialog, const QString &s_FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				err				= 0;

    int				i				= 0;
    int				j				= 1;
    int				k				= 0;
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
    QString			InputStr		= "";

    QString			SearchString1	= "*C0009";
    QString			SearchString2	= "*U0009";
    QString			SearchString3	= "*C0500";
    QString			SearchString4	= "*U0500";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";
    QString         s_DatasetID     = "";
    QString         s_Comment       = "";

    QStringList     sl_Parameter;

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;
    bool			b_Out			= false;

    bool			b_UV_a_g		= false;
    bool			b_UV_b_d		= false;
    bool			b_UV_b_g		= false;
    bool			b_UV_b_diff		= false;
    bool			b_UV_up_r		= false;

    int				P[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    structParameter	*Parameter_0001	= NULL;
    structParameter	*Parameter_0009	= NULL;

    Parameter_0001	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// ***********************************************************************************************************************

    for ( int i=0; i<=MAX_NUM_OF_PARAMETER; ++i )
    {
        Parameter_0001[i].ParameterID = -1;
        Parameter_0001[i].MethodID    = -1;

        Parameter_0009[i].ParameterID = -1;
        Parameter_0009[i].MethodID    = -1;

        P[i] = 0;
    }


// ***********************************************************************************************************************

    err = UltraVioletMeasurementsTest( s_FilenameIn, P, i_NumOfFiles );

    if ( err != _NOERROR_ )
        return( setErr( err ) );

// ***********************************************************************************************************************

    if ( b_Import == false )
    {
        offset = 4;

        if ( b_showSelectParameterDialog == true )
        {
            if ( doSelectParametersDialog( LR0500, P ) != QDialog::Accepted )
                return( _CHOOSEABORTED_ );

            if ( ( err = checkSelectedParameter( offset, P ) ) != _NOERROR_ )
                 return( err );
        }
        else
        {
            for ( int i=0; i<=20+offset; i++ )
                P[i] = 0;

            P[1] = 1; P[2] = 1; P[3] = 1; // Date/Time, Latitutude, Longitude

            P[1+offset]  = 1;  // UV-a global [W/m**2]
            P[5+offset]  = 1;  // UV-b direct [W/m**2]
            P[9+offset]  = 1;  // UV-b global [W/m**2]
            P[13+offset] = 1;  // UV-b diffuse [W/m**2]
            P[17+offset] = 1;  // UV upward reflected [W/m**2]
        }
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

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Ultra-violet measurements converter working (Building)..." ), 100 );

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
        InputStr  = tin.readLine().simplified();
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
            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            i_PIID = findPiID( InputStr.left( 38 ).simplified(), Staff_ptr );

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

    QString s_FilenameOut = fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_0500";

    if ( b_Import == true )
        s_FilenameOut.append( "_imp.txt" );
    else
        s_FilenameOut.append( ".txt" );

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************
// LR0009

    i = 0;
    j = 0;

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr  = tin.readLine();
        ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr  = tin.readLine();
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
// 0500 - build data description header

    b_Stop = false;

    if ( b_Import == true )
    {
        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetID( QString( "%1_Ultra-violet_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                s_DatasetID = DataSetID( num2str( i_DatasetID ) );
            else
                s_DatasetID = DataSetID( QString( "@%1_Ultra-violet_%2@" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ) );
        }

        sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );
        sl_Parameter.append( Parameter( num2str( 56349 ), num2str( i_PIID ), num2str( 43 ), tr( "###0" ) ) );

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 121 ) && ( b_UV_a_g == false ) ) // UV-a global
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 121 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[1] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55922 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[2] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55923 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[3] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55924 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[4] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55925 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );

                b_UV_a_g = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 122 ) && ( b_UV_b_d == false ) ) // UV-b direct
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 122 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[5] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55926 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[6] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55927 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[7] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55928 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[8] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55929 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );

                b_UV_b_d = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 123 ) && ( b_UV_b_g == false ) ) // UV-b global
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 123 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[9] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55930 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[10] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55931 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[11] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55932 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[12] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55933 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );

                b_UV_b_g = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 124 ) && ( b_UV_b_diff == false ) ) // UV-b diffuse
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 124 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[13] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55934 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[14] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55935 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[15] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55936 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[16] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55937 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );

                b_UV_b_diff = true;
            }
        }

        for ( int i=1; i<=n; ++i )
        {
            if ( ( Parameter_0009[i].ParameterID == 125 ) && ( b_UV_up_r == false ) ) // UV upward reflected
            {
                k = 0; for ( j=i+1; j<=n; ++j ) if ( Parameter_0009[j].ParameterID == 125 ) k = j;

                i_MethodID = findMethodID( i_StationNumber, Parameter_0009[i].MethodID, Method_ptr );

                if ( k == 0 )
                    s_Comment.clear();
                else
                    s_Comment = tr( "Changed to WRMC No. " ) + num2str( Parameter_0009[k].MethodID ) + tr( " at " ) + Parameter_0009[k].DateTime.toString( "yyyy-MM-ddThh:mm" );

                if ( P[17] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55938 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[18] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55939 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[19] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55940 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );
                if ( P[20] > 0 )
                    sl_Parameter.append( Parameter( num2str( 55941 ), num2str( i_PIID ), num2str( i_MethodID ), tr( "###0.0" ), s_Comment ) );

                b_UV_up_r = true;
            }
        }

        b_UV_a_g	= false;
        b_UV_b_d	= false;
        b_UV_b_g	= false;
        b_UV_b_diff	= false;
        b_UV_up_r	= false;
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
        tout << DatasetTitle( QString( "Ultra-violet measurements from" ), s_StationName, dt );
        tout << ExportFilename( s_EventLabel, tr( "Ultra-violet" ), dt );
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

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == 121 ) && ( b_UV_a_g == false ) )
        {
            if ( b_Import == true )
            {
                if ( P[1] > 0 ) tout << "\t55922";
                if ( P[2] > 0 ) tout << "\t55923";
                if ( P[3] > 0 ) tout << "\t55924";
                if ( P[4] > 0 ) tout << "\t55925";			// UV-a global
            }
            else
            {
                if ( P[1+offset] > 0 ) tout << "\tUV-a global [W/m**2]";
                if ( P[2+offset] > 0 ) tout << "\tUV-a global, standard deviation [W/m**2]";
                if ( P[3+offset] > 0 ) tout << "\tUV-a global, minimum [W/m**2]";
                if ( P[4+offset] > 0 ) tout << "\tUV-a global, maximum [W/m**2]";
            }

            b_UV_a_g = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == 122 ) && ( b_UV_b_d == false ) )
        {
            if ( b_Import == true )
            {
                if ( P[5] > 0 ) tout << "\t55926";
                if ( P[6] > 0 ) tout << "\t55927";
                if ( P[7] > 0 ) tout << "\t55928";
                if ( P[8] > 0 ) tout << "\t55929";			// UV-b direct
            }
            else
            {
                if ( P[5+offset] > 0 ) tout << "\tUV-b direct [W/m**2]";
                if ( P[6+offset] > 0 ) tout << "\tUV-b direct, standard deviation [W/m**2]";
                if ( P[7+offset] > 0 ) tout << "\tUV-b direct, minimum [W/m**2]";
                if ( P[8+offset] > 0 ) tout << "\tUV-b direct, maximum [W/m**2]";
            }

            b_UV_b_d = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == 123 ) && ( b_UV_b_g == false ) )
        {
            if ( b_Import == true )
            {
                if ( P[9]  > 0 ) tout << "\t55930";
                if ( P[10] > 0 ) tout << "\t55931";
                if ( P[11] > 0 ) tout << "\t55932";
                if ( P[12] > 0 ) tout << "\t55933";			// UV-b global
            }
            else
            {
                if ( P[9+offset]  > 0 ) tout << "\tUV-b global [W/m**2]";
                if ( P[10+offset] > 0 ) tout << "\tUV-b global, standard deviation [W/m**2]";
                if ( P[11+offset] > 0 ) tout << "\tUV-b global, minimum [W/m**2]";
                if ( P[12+offset] > 0 ) tout << "\tUV-b global, maximum [W/m**2]";
            }

            b_UV_b_g = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == 124 ) && ( b_UV_b_diff == false ) )
        {
            if ( b_Import == true )
            {
                if ( P[13] > 0 ) tout << "\t55934";
                if ( P[14] > 0 ) tout << "\t55935";
                if ( P[15] > 0 ) tout << "\t55936";
                if ( P[16] > 0 ) tout << "\t55937";			// UV-b diffuse
            }
            else
            {
                if ( P[13+offset] > 0 ) tout << "\tUV-b diffuse [W/m**2]";
                if ( P[14+offset] > 0 ) tout << "\tUV-b diffuse, standard deviation [W/m**2]";
                if ( P[15+offset] > 0 ) tout << "\tUV-b diffuse, minimum [W/m**2]";
                if ( P[16+offset] > 0 ) tout << "\tUV-b diffuse, maximum [W/m**2]";
            }

            b_UV_b_diff = true;
        }
    }

    for ( int i=1; i<=n; ++i )
    {
        if ( ( Parameter_0009[i].ParameterID == 125 ) && ( b_UV_up_r == false ) )
        {
            if ( b_Import == true )
            {
                if ( P[17] > 0 ) tout << "\t55938";
                if ( P[18] > 0 ) tout << "\t55939";
                if ( P[19] > 0 ) tout << "\t55940";
                if ( P[20] > 0 ) tout << "\t55941";			// UV upward reflected
            }
            else
            {
                if ( P[17+offset] > 0 ) tout << "\tUV upward reflected [W/m**2]";
                if ( P[18+offset] > 0 ) tout << "\tUV upward reflected, standard deviation [W/m**2]";
                if ( P[19+offset] > 0 ) tout << "\tUV upward reflected, minimum [W/m**2]";
                if ( P[20+offset] > 0 ) tout << "\tUV upward reflected, maximum [W/m**2]";
            }

            b_UV_up_r = true;
        }
    }

    tout << eol;

    if ( ( b_UV_a_g == false ) && ( b_UV_b_d == false ) && ( b_UV_b_g == false ) && ( b_UV_b_diff == false ) && ( b_UV_up_r == false ) )
        b_Stop = true;

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

                    OutputStr = buildGeocodeEntries( b_Import, P, dt, s_EventLabel, f_Latitude, f_Longitude );

                    if ( b_UV_a_g == true )
                    {
                        for ( int i=1; i<=4; ++i )
                        {
                            if ( P[i+offset] > 0 )
                            {
                                if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                                {
                                    OutputStr.append( "\t" + InputStr.mid( i*6+3, 5 ).simplified() );
                                    b_Out = true;
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }
                    }

                    if ( b_UV_b_d == true )
                    {
                        for ( i=5; i<=8; ++i )
                        {
                            if ( P[i+offset] > 0 )
                            {
                                if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                                {
                                    OutputStr.append( "\t" + InputStr.mid( i*6+3, 5 ).simplified() );
                                    b_Out = true;
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }
                    }

                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( b_UV_b_g == true )
                    {
                        for ( int i=1; i<=4; ++i )
                        {
                            if ( P[i+8+offset] > 0 )
                            {
                                if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                                {
                                    OutputStr.append( "\t" + InputStr.mid( i*6+3, 5 ).simplified() );
                                    b_Out = true;
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }
                    }

                    if ( b_UV_b_diff == true )
                    {
                        for ( i=5; i<=8; ++i )
                        {
                            if ( P[i+8+offset] > 0 )
                            {
                                if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                                {
                                    OutputStr.append( "\t" + InputStr.mid( i*6+3, 5 ).simplified() );
                                    b_Out = true;
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
                        }
                    }

                    if ( b_UV_up_r == true )
                    {
                        for ( i=9; i<=12; ++i )
                        {
                            if ( P[i+8+offset] > 0 )
                            {
                                if ( InputStr.mid( i*6+3, 5 ).simplified().toFloat() > -90 )
                                {
                                    OutputStr.append( "\t" + InputStr.mid( i*6+3, 5 ).simplified() );
                                    b_Out = true;
                                }
                                else
                                    OutputStr.append( "\t" );
                            }
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
            InputStr	= tin.readLine();
            ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );
        }
    }

// ***********************************************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( ( b_UV_a_g == false ) && ( b_UV_b_d == false ) && ( b_UV_b_g == false ) && ( b_UV_b_diff == false ) && ( b_UV_up_r == false ) )
        fout.remove();

// **********************************************************************************************

    if ( Parameter_0001 != NULL )
    {
        delete []Parameter_0001;
        Parameter_0001 = NULL;
    }

    if ( Parameter_0009 != NULL )
    {
        delete []Parameter_0009;
        Parameter_0009 = NULL;
    }

// **********************************************************************************************

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    removeEmptyFile( s_FilenameIn, s_FilenameOut, 100 );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Expanded Measurements Converters, LR0500 */

void MainWindow::doUltraVioletMeasurementsConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        if ( b_Import == true )
            readBsrnReferenceIDs( false );

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Ultra-violet measurements converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = UltraVioletMeasurementsConverter( b_Import, true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Ultra-violet measurements converter was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Basic Measurements Converters im Import-Mode, LR0500 */

void MainWindow::doUltraVioletMeasurementsImportConverter()
{
    doUltraVioletMeasurementsConverter( true );
}
