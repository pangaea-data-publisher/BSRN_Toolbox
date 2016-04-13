/* doAll						*/
/* 2007-06-08					*/
/* Dr. Rainer Sieger			*/

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2009-05-04

/*! @brief Setzt das Overwrite Dataset Flag */

void MainWindow::doSetOverwriteDatasetFlag()
{
    int err = _NOERROR_;

    if ( gb_OverwriteDataset == true )
      gb_OverwriteDataset = false;
    else
      gb_OverwriteDataset = true;

// **********************************************************************************************

    setOverwriteDatasetFlagAction->setChecked( gb_OverwriteDataset );

// **********************************************************************************************

    if ( gb_OverwriteDataset == true )
        err = readIDsDatasets();

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-06-08

/*! @brief Ruft alle Metadaten Converter nacheinander auf */

void MainWindow::doAllMetadataConverter()
{
    int			i				= 0;
    int			err				= 0;
    int			stopProgress	= 0;

    QStringList	sl_FilenameSave;
    QStringList sl_FilenameOut;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Metadata converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR 0001
            if ( err == _NOERROR_ )
                err = FileIDConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR 0002
            if ( err == _NOERROR_ )
                err = ScientistIDConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Staff_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR 0003
            if ( err == _NOERROR_ )
                err = MessagesConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR 0004
            if ( err == _NOERROR_ )
                err = StationDescriptionConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR 0005
            if ( err == _NOERROR_ )
                err = RadiosondeEquipmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR 0006
            if ( err == _NOERROR_ )
                err = OzoneEquipmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR 0007
            if ( err == _NOERROR_ )
                err = StationHistoryConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR 0008
            if ( err == _NOERROR_ )
                err = RadiationInstrumentsConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR 0009
            if ( err == _NOERROR_ )
                err = AssignmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// Reference import file
            if ( err == _NOERROR_ )
                err = CreateReferenceImportFile( gsl_FilenameList.at( i ), sl_FilenameOut, g_Staff_ptr, g_Station_ptr,  gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );

        if ( err == _NOERROR_ )
        {
            copyStringList( gsl_FilenameList, sl_FilenameSave );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0001" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0001.txt", gsl_FilenameList, tr( "Building LR0001 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0002" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0002.txt", gsl_FilenameList, tr( "Building LR0002 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0003" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0003.txt", gsl_FilenameList, tr( "Building LR0003 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0004" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0004.txt", gsl_FilenameList, tr( "Building LR0004 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0005" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0005.txt", gsl_FilenameList, tr( "Building LR0005 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0006" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0006.txt", gsl_FilenameList, tr( "Building LR0006 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0007" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0007.txt", gsl_FilenameList, tr( "Building LR0007 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0008" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0008.txt", gsl_FilenameList, tr( "Building LR0008 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_0009" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_LR0009.txt", gsl_FilenameList, tr( "Building LR0009 file..." ), 1, true );

            if ( copyStringList( sl_FilenameOut, gsl_FilenameList, "_refImp" ) > 0 )
                err = concatenateFiles( gs_Path + "/" + "BSRN_RefImp.txt", gsl_FilenameList, tr( "Building reference import file..." ), 1, true );

            copyStringList( sl_FilenameSave, gsl_FilenameList );
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Metadata converter was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-06-08

/*! @brief Ruf alle Data Converter nacheinander auf */

void MainWindow::doAllDataConverter()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    int		P[MAX_NUM_OF_PARAMETER+1];

    QStringList sl_FilenameOut;

// **********************************************************************************************

    structParameter	*Parameter_0001_ptr	= NULL;
    structParameter	*Parameter_0009_ptr	= NULL;

    Parameter_0001_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Data converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR 0100
            if ( err == _NOERROR_ )
                err = BasicMeasurementsConverter( false, gsl_FilenameList.at( i ), Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 0300
            if ( err == _NOERROR_ )
                err = OtherMinuteMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, P, gsl_FilenameList.count() );

// LR 0500
            if ( err == _NOERROR_ )
                err = UltraVioletMeasurementsConverter( false, gsl_FilenameList.at( i ), Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1000
            if ( err == _NOERROR_ )
                err = SYNOPConverter( false, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1100
            if ( err == _NOERROR_ )
                err = RadiosondeMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1200
            if ( err == _NOERROR_ )
                err = OzoneMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1300
            if ( err == _NOERROR_ )
                err = ExpandedMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3010
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 10, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3030
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 30, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3300
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 300, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Data converter was canceled" ), false, false );

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
// 2008-06-08

/*! @brief Ruf alle Import Converter nacheinander auf */

void MainWindow::doAllImportConverter()
{
    int		    i				= 0;
    int		    err				= 0;
    int		    stopProgress	= 0;

    QStringList sl_FilenameOut;  // not used

// **********************************************************************************************

    structParameter	*Parameter_0001_ptr	= NULL;
    structParameter	*Parameter_0009_ptr	= NULL;

    Parameter_0001_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Import converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR 0100
            if ( err == _NOERROR_ )
                err = BasicMeasurementsConverter( true, gsl_FilenameList.at( i ), Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 0500
            if ( err == _NOERROR_ )
                err = UltraVioletMeasurementsConverter( true, gsl_FilenameList.at( i ), Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1000
            if ( err == _NOERROR_ )
                err = SYNOPConverter( true, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1100
            if ( err == _NOERROR_ )
                err = RadiosondeMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1200
            if ( err == _NOERROR_ )
                err = OzoneMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 1300
            if ( err == _NOERROR_ )
                err = ExpandedMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3010
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 10, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3030
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 30, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR 3300
            if ( err == _NOERROR_ )
                err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 300, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Import converter was canceled" ), false, false );

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
// 2016-04-10

/*! @brief Laed BSRN IDs Datei */

void MainWindow::doRefreshIDsBSRN()
{
    setStatusBar( tr( "Reading BSRN IDs database - please wait" ) );
    setWaitCursor();

// **********************************************************************************************

    downloadFile( QLatin1String( "https://pangaea.de/PHP/bsrn/BSRN_IDs.txt" ), getDataLocation() + "/" + "BSRN_IDs.txt" );

// **********************************************************************************************

    setStatusBar( tr( "Ready" ), 2 );
    setNormalCursor();

// **********************************************************************************************

    readIDsBSRN();

// **********************************************************************************************

    onError( _NOERROR_ );
}
