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
        err = readBsrnDatasetIDs();

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

        while ( ( i < gsl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR0001
            err = FileIDConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR0002
            err = ScientistIDConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Staff_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR0003
            err = MessagesConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR0004
            err = StationDescriptionConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR0005
            err = RadiosondeEquipmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR0006
            err = OzoneEquipmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR0007
            err = StationHistoryConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Station_ptr, gsl_FilenameList.count() );

// LR0008
            err = RadiationInstrumentsConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// LR0009
            err = AssignmentConverter( gsl_FilenameList.at( i ), sl_FilenameOut, g_Method_ptr, g_Station_ptr, gsl_FilenameList.count() );

// Reference import file
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
    int		i				  = 0;
    int		err				  = 0;
    int		stopProgress	  = 0;

    QString     s_FilenameOut = "";

    QStringList sl_FilenameOut;

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Data converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR0100
            err = BasicMeasurementsConverter( false, false, LR0100, gsl_FilenameList.at( i ), s_FilenameOut, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR0300
            err = BasicMeasurementsConverter( false, false, LR0300, gsl_FilenameList.at( i ), s_FilenameOut, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR0500
            err = UltraVioletMeasurementsConverter( false, false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1000
            err = SYNOPConverter( false, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1100
            err = RadiosondeMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1200
            err = OzoneMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1300
            err = ExpandedMeasurementsConverter( false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3010
            err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 10, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3030
            err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 30, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3300
            err = OtherMeasurementsAtXmConverter( false, gsl_FilenameList.at( i ), 300, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

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

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-06-08

/*! @brief Ruf alle Import Converter nacheinander auf */

void MainWindow::doAllImportConverter()
{
    int		i				  = 0;
    int		err				  = 0;
    int		stopProgress	  = 0;

    QString     s_FilenameOut = "";

    QStringList sl_FilenameOut;  // not used

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        sl_FilenameOut.clear();

        readBsrnReferenceIDs( false );

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Import converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
        {

// LR0100
            err = BasicMeasurementsConverter( true, false, LR0100, gsl_FilenameList.at( i ), s_FilenameOut, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR0500
            err = UltraVioletMeasurementsConverter( true, false, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1000
            err = SYNOPConverter( true, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1100
            err = RadiosondeMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1200
            err = OzoneMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR1300
            err = ExpandedMeasurementsConverter( true, gsl_FilenameList.at( i ), g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3010
            err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 10, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3030
            err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 30, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

// LR3300
            err = OtherMeasurementsAtXmConverter( true, gsl_FilenameList.at( i ), 300, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

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

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-04-10

/*! @brief Laed BSRN dataset IDs Datei */

void MainWindow::doRefreshBsrnIDs()
{
    setWaitCursor();
    setStatusBar( tr( "Reading BSRN IDs database - please wait" ) );

// **********************************************************************************************

    downloadFile( findCurl(), QLatin1String( "https://pangaea.de/PHP/bsrn/BSRN_IDs.txt" ), getDataLocation() + "/" + "BSRN_IDs.txt" );

// **********************************************************************************************

    setNormalCursor();
    setStatusBar( tr( "Ready" ), 2 );

// **********************************************************************************************

    readBsrnIDs();

// **********************************************************************************************

    onError( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-04-10

/*! @brief Laed BSRN reference IDs Datei */

void MainWindow::doRefreshBsrnReferenceIDs()
{
    setWaitCursor();
    setStatusBar( tr( "Reading BSRN reference IDs database - please wait" ) );

// **********************************************************************************************

    downloadFile( findCurl(), QLatin1String( "https://pangaea.de/ddi?request=bsrn/BSRNReferences&format=textfile&charset=UTF-8" ), getDataLocation() + "/" + "BSRN_Reference_IDs.txt" );

// **********************************************************************************************

    readBsrnReferenceIDs( true );

// **********************************************************************************************

    setNormalCursor();
    setStatusBar( tr( "Ready" ), 2 );

// **********************************************************************************************

    onError( _NOERROR_ );
}
