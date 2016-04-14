// ***********************************************************************************************
// *                                                                                             *
// * createMenu.cpp - creates application menus                                                  *
// *                                                                                             *
// * Dr. Rainer Sieger - 2008-09-27                                                              *
// *                                                                                             *
// ***********************************************************************************************

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Erstellen der Menue-Aktionen. */

void MainWindow::createActions()
{
// File menu
    newWindowAction = new QAction(tr("&New window"), this);
    newWindowAction->setShortcut(tr("Ctrl+N"));
    connect(newWindowAction, SIGNAL(triggered()), this, SLOT(newWindow()));

    openFileAction = new QAction(tr("&Open..."), this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(chooseFiles()));

    openFolderAction = new QAction(tr("Select &Folder..."), this);
    openFolderAction->setShortcut(tr("Ctrl+F"));
    connect(openFolderAction, SIGNAL(triggered()), this, SLOT(chooseFolder()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveAsAction = new QAction(tr("Save &As..."), this);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    hideWindowAction = new QAction(tr("&Close window"), this);
    hideWindowAction->setShortcut(tr("Ctrl+W"));
    connect(hideWindowAction, SIGNAL(triggered()), this, SLOT(hideWindow()));

    exitAction = new QAction(tr("&Quit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exitApplication()));

// Station-to-archive menu
    downloadStationToArchiveFilesAction = new QAction(tr("&Download Station-to-archive files..."), this);
    connect(downloadStationToArchiveFilesAction, SIGNAL(triggered()), this, SLOT(doDownloadStationToArchiveFiles()));

    checkStationToArchiveFilesAction = new QAction(tr("&Check Station-to-archive files..."), this);
    connect(checkStationToArchiveFilesAction, SIGNAL(triggered()), this, SLOT(doCheckStationToArchiveFiles()));

// Metadata menu
    fileIDAction = new QAction(tr("&File ID, LR 0001"), this);
    connect(fileIDAction, SIGNAL(triggered()), this, SLOT(doFileIDConverter()));

    scientistIDAction = new QAction(tr("&Scientist description, LR 0002"), this);
    connect(scientistIDAction, SIGNAL(triggered()), this, SLOT(doScientistIDConverter()));

    messagesAction = new QAction(tr("&Messages, LR 0003"), this);
    connect(messagesAction, SIGNAL(triggered()), this, SLOT(doMessagesConverter()));

    stationDescriptionAction = new QAction(tr("Station &description, LR 0004"), this);
    connect(stationDescriptionAction, SIGNAL(triggered()), this, SLOT(doStationDescriptionConverter()));

    radiosondeEquipmentAction = new QAction(tr("&Radiosonde equipment, LR 0005"), this);
    connect(radiosondeEquipmentAction, SIGNAL(triggered()), this, SLOT(doRadiosondeEquipmentConverter()));

    ozoneEquipmentAction = new QAction(tr("&Ozone equipment, LR 0006"), this);
    connect(ozoneEquipmentAction, SIGNAL(triggered()), this, SLOT(doOzoneEquipmentConverter()));

    stationHistoryAction = new QAction(tr("Station &history, LR 0007"), this);
    connect(stationHistoryAction, SIGNAL(triggered()), this, SLOT(doStationHistoryConverter()));

    radiationInstrumentsAction = new QAction(tr("R&adiation instruments, LR 0008"), this);
    connect(radiationInstrumentsAction, SIGNAL(triggered()), this, SLOT(doRadiationInstrumentsConverter()));

    assignmentAction = new QAction(tr("Assig&nment of radiation quantities, LR 0009"), this);
    connect(assignmentAction, SIGNAL(triggered()), this, SLOT(doAssignmentConverter()));

    createRefFileAction = new QAction(tr("Create reference import file"), this);
    connect(createRefFileAction, SIGNAL(triggered()), this, SLOT(doCreateReferenceImportFile()));

    doAllMetadataAction = new QAction(tr("Create all metadata files"), this);
    connect(doAllMetadataAction, SIGNAL(triggered()), this, SLOT(doAllMetadataConverter()));

    doRefreshIDsBSRNAction = new QAction(tr("Refresh BSRN IDs database"), this);
    connect(doRefreshIDsBSRNAction, SIGNAL(triggered()), this, SLOT(doRefreshIDsBSRN()));

    // Data menu
    basicMeasurementsAction = new QAction(tr("&Basic and other measurements, LR 0100 + LR 0300"), this);
    connect(basicMeasurementsAction, SIGNAL(triggered()), this, SLOT(doBasicMeasurementsConverter()));

    otherMinuteMeasurementsAction = new QAction(tr("&Other measurements in minutes intervals, LR 0300"), this);
    connect(otherMinuteMeasurementsAction, SIGNAL(triggered()), this, SLOT(doOtherMinuteMeasurementsConverter()));

    uvMeasurementsAction = new QAction(tr("&Ultra-violet measurements, LR 0500"), this);
    connect(uvMeasurementsAction, SIGNAL(triggered()), this, SLOT(doUltraVioletMeasurementsConverter()));

    synopAction = new QAction(tr("&SYNOP, LR 1000"), this);
    connect(synopAction, SIGNAL(triggered()), this, SLOT(doSYNOPConverter()));

    radiosondeMeasurementsAction = new QAction(tr("&Radiosonde measurements, LR 1100"), this);
    connect(radiosondeMeasurementsAction, SIGNAL(triggered()), this, SLOT(doRadiosondeMeasurementsConverter()));

    ozoneMeasurementsAction = new QAction(tr("&Ozone measurements, LR 1200"), this);
    connect(ozoneMeasurementsAction, SIGNAL(triggered()), this, SLOT(doOzoneMeasurementsConverter()));

    expandedMeasurementsAction = new QAction(tr("&Expanded measurements in hours intervals, LR 1300"), this);
    connect(expandedMeasurementsAction, SIGNAL(triggered()), this, SLOT(doExpandedMeasurementsConverter()));

    otherMeasurementsAt10mAction = new QAction(tr("&Other measurements at 10 m, LR 3010"), this);
    connect(otherMeasurementsAt10mAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt10mConverter()));

    otherMeasurementsAt30mAction = new QAction(tr("&Other measurements at 30 m, LR 3030"), this);
    connect(otherMeasurementsAt30mAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt30mConverter()));

    otherMeasurementsAt300mAction = new QAction(tr("&Other measurements at 300 m, LR 3300"), this);
    connect(otherMeasurementsAt300mAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt300mConverter()));

    doAllDataAction = new QAction(tr("Create all data files"), this);
    connect(doAllDataAction, SIGNAL(triggered()), this, SLOT(doAllDataConverter()));

    // Import menu
    basicMeasurementsImportAction = new QAction(tr("&Basic and other measurements, LR 0100 + LR 0300"), this);
    connect(basicMeasurementsImportAction, SIGNAL(triggered()), this, SLOT(doBasicMeasurementsImportConverter()));

    uvMeasurementsImportAction = new QAction(tr("&Ultra-violet measurements, LR 0500"), this);
    connect(uvMeasurementsImportAction, SIGNAL(triggered()), this, SLOT(doUltraVioletMeasurementsImportConverter()));

    synopImportAction = new QAction(tr("&SYNOP, LR 1000"), this);
    connect(synopImportAction, SIGNAL(triggered()), this, SLOT(doSYNOPImportConverter()));

    radiosondeMeasurementsImportAction = new QAction(tr("&Radiosonde measurements, LR 1100"), this);
    connect(radiosondeMeasurementsImportAction, SIGNAL(triggered()), this, SLOT(doRadiosondeMeasurementsImportConverter()));

    ozoneMeasurementsImportAction = new QAction(tr("&Ozone measurements, LR 1200"), this);
    connect(ozoneMeasurementsImportAction, SIGNAL(triggered()), this, SLOT(doOzoneMeasurementsImportConverter()));

    expandedMeasurementsImportAction = new QAction(tr("&Expanded measurements in hours intervals, Part I, LR 1300"), this);
    connect(expandedMeasurementsImportAction, SIGNAL(triggered()), this, SLOT(doExpandedMeasurementsImportConverter()));

    otherMeasurementsAt10mImportAction = new QAction(tr("&Other measurements at 10 m, LR 3010"), this);
    connect(otherMeasurementsAt10mImportAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt10mImportConverter()));

    otherMeasurementsAt30mImportAction = new QAction(tr("&Other measurements at 30 m, LR 3030"), this);
    connect(otherMeasurementsAt30mImportAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt30mImportConverter()));

    otherMeasurementsAt300mImportAction = new QAction(tr("&Other measurements at 300 m, LR 3300"), this);
    connect(otherMeasurementsAt300mImportAction, SIGNAL(triggered()), this, SLOT(doOtherMeasurementsAt300mImportConverter()));

    doAllImportAction = new QAction(tr("Create all import files"), this);
    connect(doAllImportAction, SIGNAL(triggered()), this, SLOT(doAllImportConverter()));

    setOverwriteDatasetFlagAction = new QAction(tr("Overwrite dataset"), this);
    setOverwriteDatasetFlagAction->setCheckable( true );
    setOverwriteDatasetFlagAction->setChecked( gb_OverwriteDataset );
    connect(setOverwriteDatasetFlagAction, SIGNAL(triggered()), this, SLOT(doSetOverwriteDatasetFlag()));

// Tools menu

    concatenateFilesAction = new QAction(tr("Concatenate files..."), this);
    connect(concatenateFilesAction, SIGNAL(triggered()), this, SLOT(doConcatenateFiles()));

    WinConvertEOLAction = new QAction(tr("Convert Windows End-of-Line to UNIX"), this);
    connect(WinConvertEOLAction, SIGNAL(triggered()), this, SLOT(doWinConvertEOL()));

    MacOSConvertEOLAction = new QAction(tr("Convert MacOS 9 End-of-Line to UNIX"), this);
    connect(MacOSConvertEOLAction, SIGNAL(triggered()), this, SLOT(doMacOSConvertEOL()));

    compressFileAction = new QAction(tr("Compress files with gzip"), this);
    connect(compressFileAction, SIGNAL(triggered()), this, SLOT(doCompressFile()));

    convertUnformattedAction = new QAction(tr("Convert tab files to unformatted format..."), this);
    connect(convertUnformattedAction, SIGNAL(triggered()), this, SLOT(doConvertUnformatted()));

    convertFormattedAction = new QAction(tr("Convert tab files to formatted format..."), this);
    connect(convertFormattedAction, SIGNAL(triggered()), this, SLOT(doConvertFormatted()));

    createReplaceDatabaseAction = new QAction(tr("Create search and replace database..."), this);
    connect(createReplaceDatabaseAction, SIGNAL(triggered()), this, SLOT(doCreateReplaceDatabase()));

// Quality check menu

    QualityCheckRecommendedV20Action = new QAction(tr("BSRN &Recommended V2.0"), this);
    connect(QualityCheckRecommendedV20Action, SIGNAL(triggered()), this, SLOT(doQualityCheckRecommendedV20()));

// Help menu
    aboutAction = new QAction(tr("&About ") + getApplicationName( true ), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    openExternalURLHelpAction = new QAction(getApplicationName( true ) + tr(" &Manual"), this);
    openExternalURLHelpAction->setShortcut(tr("F1"));
    connect(openExternalURLHelpAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLHelp()));

    openExternalURLBSRNHomePageAction = new QAction(tr("BSRN Homepage"), this);
    connect(openExternalURLBSRNHomePageAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLBSRNHomepage()));

    openExternalURLBSRNAccountAction = new QAction(tr("How to get the BSRN account"), this);
    connect(openExternalURLBSRNAccountAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLBSRNAccount()));

    openExternalURLBSRNStatusAction = new QAction(tr("BSRN Status"), this);
    connect(openExternalURLBSRNStatusAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLBSRNStatus()));

    openExternalURLBSRNLatestSnapshotAction = new QAction(tr("BSRN snapshot 2015-09"), this);
    connect(openExternalURLBSRNLatestSnapshotAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLBSRNLatestSnapshot()));

    openExternalURLBSRNStationToArchiveFormatAction = new QAction(tr("Station-to-archiv file format description"), this);
    connect(openExternalURLBSRNStationToArchiveFormatAction, SIGNAL(triggered()), this, SLOT(doOpenExternalURLBSRNStationToArchiveFormat()));
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Verbindet Menues mit Aktionen. */

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( openFileAction );
    fileMenu->addAction( openFolderAction );

#if defined(Q_OS_MAC)
    fileMenu->addSeparator();
    fileMenu->addAction( newWindowAction );
    newWindowAction->setEnabled( false );
    fileMenu->addAction( hideWindowAction );
#endif

#if defined(Q_OS_WIN)
    fileMenu->addSeparator();
    fileMenu->addAction( hideWindowAction );
#endif

    fileMenu->addSeparator();
    fileMenu->addAction( exitAction );

// **********************************************************************************************

    stationMenu = menuBar()->addMenu( tr( "&Station-to-archive" ) );
    stationMenu->addAction( downloadStationToArchiveFilesAction );
    stationMenu->addAction( checkStationToArchiveFilesAction );

// **********************************************************************************************

    metadataMenu = menuBar()->addMenu( tr( "&Metadata" ) );
    metadataMenu->addAction( fileIDAction );
    metadataMenu->addAction( scientistIDAction );
    metadataMenu->addAction( messagesAction );
    metadataMenu->addAction( stationDescriptionAction );
    metadataMenu->addAction( radiosondeEquipmentAction );
    metadataMenu->addAction( ozoneEquipmentAction );
    metadataMenu->addAction( stationHistoryAction );
    metadataMenu->addAction( radiationInstrumentsAction );
    metadataMenu->addAction( assignmentAction );
    metadataMenu->addSeparator();
    metadataMenu->addAction( createRefFileAction );
    metadataMenu->addSeparator();
    metadataMenu->addAction( doAllMetadataAction );
    metadataMenu->addSeparator();
    metadataMenu->addAction( doRefreshIDsBSRNAction );

// **********************************************************************************************

    dataMenu = menuBar()->addMenu( tr( "&Data" ) );
    dataMenu->addAction( basicMeasurementsAction );
    dataMenu->addAction( otherMinuteMeasurementsAction );
    dataMenu->addAction( uvMeasurementsAction );
    dataMenu->addAction( synopAction );
    dataMenu->addAction( radiosondeMeasurementsAction );
    dataMenu->addAction( ozoneMeasurementsAction );
    dataMenu->addAction( expandedMeasurementsAction );
    dataMenu->addAction( otherMeasurementsAt10mAction );
    dataMenu->addAction( otherMeasurementsAt30mAction );
    dataMenu->addAction( otherMeasurementsAt300mAction );
    dataMenu->addSeparator();
    dataMenu->addAction( doAllDataAction );

// **********************************************************************************************

    importMenu = menuBar()->addMenu( tr( "&Import" ) );
    importMenu->addAction( setOverwriteDatasetFlagAction );
    importMenu->addSeparator();
    importMenu->addAction( basicMeasurementsImportAction );
    importMenu->addAction( uvMeasurementsImportAction );
    importMenu->addAction( synopImportAction );
    importMenu->addAction( radiosondeMeasurementsImportAction );
    importMenu->addAction( ozoneMeasurementsImportAction );
    importMenu->addAction( expandedMeasurementsImportAction );
    importMenu->addAction( otherMeasurementsAt10mImportAction );
    importMenu->addAction( otherMeasurementsAt30mImportAction );
    importMenu->addAction( otherMeasurementsAt300mImportAction );
    importMenu->addSeparator();
    importMenu->addAction( doAllImportAction );

 // **********************************************************************************************

    toolsMenu = menuBar()->addMenu( tr( "&Tools" ) );
    toolsMenu->addAction( concatenateFilesAction );
    toolsMenu->addSeparator();
    toolsMenu->addAction( WinConvertEOLAction );
    toolsMenu->addAction( MacOSConvertEOLAction );
    toolsMenu->addSeparator();
    toolsMenu->addAction( compressFileAction );
    toolsMenu->addSeparator();
    toolsMenu->addAction( convertUnformattedAction );
    toolsMenu->addAction( convertFormattedAction );
    toolsMenu->addSeparator();
    toolsMenu->addAction( createReplaceDatabaseAction );

// **********************************************************************************************

    if ( _VERSION_.startsWith( "2.") == true )
    {
        qualityMenu = menuBar()->addMenu( tr( "&Quality Check" ) );
        qualityMenu->addAction( QualityCheckRecommendedV20Action );
    }

// **********************************************************************************************

    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( aboutAction );
    helpMenu->addAction( aboutQtAction );
    helpMenu->addSeparator();
    helpMenu->addAction( openExternalURLHelpAction );
    helpMenu->addSeparator();
    helpMenu->addAction( openExternalURLBSRNHomePageAction );
    helpMenu->addAction( openExternalURLBSRNAccountAction );
    helpMenu->addAction( openExternalURLBSRNStatusAction );
    helpMenu->addAction( openExternalURLBSRNLatestSnapshotAction );
    helpMenu->addAction( openExternalURLBSRNStationToArchiveFormatAction );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

void MainWindow::enableMenuItems( const QStringList sl_FilenameList )
{
    bool b_containsBinaryFile = containsBinaryFile( sl_FilenameList );

// **********************************************************************************************

    QList<QAction*> metadataMenuActions = metadataMenu->actions();

    if ( b_containsBinaryFile == false )
    {
        for ( int i=0; i<metadataMenuActions.count(); ++i )
            metadataMenuActions.at( i )->setEnabled( true );
    }
    else
    {
        for ( int i=0; i<metadataMenuActions.count(); ++i )
            metadataMenuActions.at( i )->setEnabled( false );
    }

// **********************************************************************************************

    QList<QAction*> dataMenuActions = dataMenu->actions();

    if ( b_containsBinaryFile == false )
    {
        for ( int i=0; i<dataMenuActions.count(); ++i )
            dataMenuActions.at( i )->setEnabled( true );
    }
    else
    {
        for ( int i=0; i<dataMenuActions.count(); ++i )
            dataMenuActions.at( i )->setEnabled( false );
    }

// **********************************************************************************************

    QList<QAction*> importMenuAction = importMenu->actions();

    if ( b_containsBinaryFile == false )
    {
        for ( int i=0; i<importMenuAction.count(); ++i )
            importMenuAction.at( i )->setEnabled( true );
    }
    else
    {
        for ( int i=0; i<importMenuAction.count(); ++i )
            importMenuAction.at( i )->setEnabled( false );
    }

// **********************************************************************************************

    QList<QAction*> toolsMenuAction = toolsMenu->actions();

    if ( b_containsBinaryFile == false )
    {
        for ( int i=0; i<toolsMenuAction.count(); ++i )
            toolsMenuAction.at( i )->setEnabled( true );
    }
    else
    {
        for ( int i=0; i<toolsMenuAction.count(); ++i )
            toolsMenuAction.at( i )->setEnabled( false );
    }

// **********************************************************************************************

    QList<QAction*> qualityMenuAction = qualityMenu->actions();

    if ( b_containsBinaryFile == false )
    {
        for ( int i=0; i<qualityMenuAction.count(); ++i )
            qualityMenuAction.at( i )->setEnabled( true );
    }
    else
    {
        for ( int i=0; i<qualityMenuAction.count(); ++i )
            qualityMenuAction.at( i )->setEnabled( false );
    }
}

