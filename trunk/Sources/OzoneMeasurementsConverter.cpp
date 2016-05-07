/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Konvertiert den LR 1200.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param FilenameIn Dateiname der Inputdatei
*   @param Method_ptr Pointer auf Array aller Methoden
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::OzoneMeasurementsConverter( const bool b_Import, const QString& s_FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
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

    QString			SearchString1	= "*C0006";
    QString			SearchString2	= "*U0006";
    QString			SearchString3	= "*C0007";
    QString			SearchString4	= "*U0007";
    QString			SearchString5	= "*C1200";
    QString			SearchString6	= "*U1200";

    QString			s_StationName					= "";
    QString			s_EventLabel					= "";
    QString			s_Remarks						= "";
    QString			s_OzoneInstrumentIdentification	= "";
    QString			s_Location						= "";
    QString         s_DatasetComment                = "";
    QString         s_DatasetID                     = "";

    QStringList     sl_Parameter;

    unsigned int	ui_length					= 1;
    unsigned int	ui_filesize					= 1;

    bool			b_OzoneMeasurements			= false;
    bool			b_Stop						= false;
    bool			b_Comment					= false;

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    if ( ( fi.exists() == false ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
        return( -1 );

// ***********************************************************************************************************************

    QFile fin( s_FilenameIn );
    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "Ozone measurements converter working..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn );

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

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_1200";

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
// LR 0006

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
                b_OzoneMeasurements = true;

                InputStr = tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                s_OzoneInstrumentIdentification = InputStr.left( 30 ).simplified();

                if ( InputStr.mid( 61, 5 ).simplified().isEmpty() == false )
                    s_OzoneInstrumentIdentification += ", " + InputStr.mid( 61, 5 ).simplified();

                i_MethodID = findMethodID( s_OzoneInstrumentIdentification, Method_ptr );

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

        if ( ( b_OzoneMeasurements == false ) && ( ( InputStr.startsWith( SearchString3 ) == true ) || ( InputStr.startsWith( SearchString4 ) == true ) ) )
            b_Stop = true;
    }

// ***********************************************************************************************************************
// 1200

    if ( b_OzoneMeasurements == true )
    {
        b_Stop = false;

// ***********************************************************************************************************************
// build data description header

        if ( b_Import == true )
        {
            if ( b_overwriteDataset == true )
            {
                int i_DatasetID = findDatasetID( QString( "%1_Ozone_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

                if ( i_DatasetID > 0 )
                    s_DatasetID = DataSetID( num2str( i_DatasetID ) );
                else
                    s_DatasetID = DataSetID( QString( "@%1_Ozone_%2@" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ) );
            }

            if ( i_Distance > 0 )
            {
                b_Comment = true;

                s_Location.replace( "CH-7050 Arosa", "Arosa, Switzerland" );
                s_Location.replace( "Tamanrasset", "Tamanrasset, Algeria" );

                s_DatasetComment.append( tr( "Location: " ) + s_Location + "; ");
                s_DatasetComment.append( tr( "Distance from radiation site: " ) + QString( "%1 km" ).arg( i_Distance ) );
            }

            if ( s_Remarks.isEmpty() == false )
            {
                if ( b_Comment == true )
                    s_DatasetComment.append( "; " + s_Remarks );
                else
                    s_DatasetComment.append( s_Remarks );
            }

            sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );
            sl_Parameter.append( Parameter( num2str( 49377 ), num2str( i_PIID ), num2str( 43 ), tr( "###0" ) ) );
        }

// ***********************************************************************************************************************
// write data description header

        if ( ( b_Import == true ) && ( sl_Parameter.count() > 1 ) )
        {
            tout << OpenDataDescriptionHeader();
            tout << s_DatasetID;
            tout << ReferenceOtherVersion( s_EventLabel, Reference_ptr, dt );
            tout << AuthorIDs( num2str( i_PIID ) );
            tout << SourceID( num2str( i_SourceID ) );
            tout << DatasetTitle( tr( "Ozone measurements from" ), s_StationName, dt );
            tout << ExportFilename( s_EventLabel, tr( "Ozone" ), dt );
            tout << EventLabel( s_EventLabel );
            tout << Parameter( sl_Parameter );
            tout << DatasetComment( s_DatasetComment );
            tout << ProjectIDs( num2str( 4094 ) );
            tout << TopologicTypeID( num2str( 8 ) );
            tout << StatusID( num2str( 4 ) );
            tout << UserIDs( num2str( 1144 ) );
            tout << LoginID( num2str( 3 ) );
            tout << CloseDataDescriptionHeader();
        }

// ***********************************************************************************************************************
// write data

        while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
        {
            InputStr  = tin.readLine();
            ui_length = incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( ( InputStr.startsWith( SearchString5 ) == true ) || ( InputStr.startsWith( SearchString6 ) == true ) )
            {
                if ( b_Import == true )
                    tout << "1599\t49377" << eol;
                else
                    tout << "Station\tDate/Time\tLatitude\tLongitude\tOzone total [DU]" << eol;

                while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
                {
                    InputStr	= tin.readLine();
                    ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                    if ( InputStr.startsWith( "*" ) == false )
                    {
                        if ( InputStr.mid( 9, 6 ).simplified() != "-999" )
                        {
                            i_Day		= InputStr.mid( 1, 2 ).toInt();
                            i_Minute	= InputStr.mid( 4, 4 ).toInt();

                            dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                            dt.setTime( QTime( 0, 0, 0 ) );
                            dt = dt.addSecs( i_Minute*60 );

                            if ( b_Import == false )
                                tout << s_EventLabel << "\t" << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t" << num2str( f_Latitude ) << "\t" << num2str( f_Longitude ) + "\t";
                            else
                                tout << dt.toString( "yyyy-MM-ddThh:mm" ) << "\t";

                            tout << InputStr.mid( 9, 6 ).simplified(); // Ozone total [DU]
                            tout << eol;
                        }
                    }
                    else
                    {
                        b_Stop = true;
                    }
                }
            }
        }
    }

// ***********************************************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();
    fout.close();

    if ( b_OzoneMeasurements == false )
        fout.remove();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Ozone Measurements Converters, LR 1200 */

void MainWindow::doOzoneMeasurementsConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        if ( b_Import == true )
            readBsrnReferenceIDs( false );

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Ozone measurements converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = OzoneMeasurementsConverter( b_Import, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Ozone measurements converter was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Ozone Measurements Converters im Import-Mode, LR 1200 */

void MainWindow::doOzoneMeasurementsImportConverter()
{
    doOzoneMeasurementsConverter( true );
}

