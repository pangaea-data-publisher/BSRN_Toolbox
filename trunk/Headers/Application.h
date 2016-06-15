/* Application.h              */
/* 2013-05-15                 */
/* Dr. Rainer Sieger          */

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <QTextEdit>
#include <QTextCodec>
#include <QTextStream>
#include <QStringListModel>
#include <QListView>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QCloseEvent>
#include <QProcess>
#include <QTimer>
#include <QMimeData>
#include <QDesktopServices>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QStatusBar>
#include <QSettings>
#include <QPointer>
#include <QErrorMessage>

#include "Globals.h"
#include "Webfile.h"

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class QAction;
class QLabel;
class QProgressDialog;
class QProgressBar;
class QTimer;
class QDateTime;
class QUrl;
class QTextEdit;
class QStringListModel;
class QListView;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_CLASSINFO( "author", "Rainer Sieger" )
    Q_CLASSINFO( "url", "https://pangaea.de" )

/* @brief Array fuer die Verwaltung des Instrumentwechseldatums */

    struct structParameter
    {
        int ParameterID;			//!< Parameter ID
        int MethodID;				//!< Method ID
        QDateTime DateTime;			//!< Date/Time des letzten Instrumentwechsels
    };

/* @brief Array fuer die Verwaltung der Methoden */

    struct structMethod
    {
        int StationID;				//!< Station ID
        QString SerialNumber;		//!< Geraete Seriennummer
        int WRMCnumber;				//!< WRMC Nummer
        int MethodID;				//!< Method ID
    };

/* @brief Array fuer die Verwaltung der Stationsleiter */

    struct structStaff
    {
        QString StationScientist;	//!< Name des Stationsleiter
        int StaffID;				//!< Staff ID
    };

/* @brief Array fuer die Verwaltung der Stationen */

    struct structStation
    {
        int StationID;				//!< Station ID
        QString EventLabel;			//!< Kurzname der Station wird als Event label benutzt
        QString StationName;		//!< Stationsname
        int InstituteID;			//!< Institut ID
    };

/* @brief Array fuer die Verwaltung der Datensaetze */

    struct structDataset
    {
        QString ExportFilename;		//!< Export filename
        int DatasetID;	    		//!< Dataset ID
    };

/* @brief Array fuer die Verwaltung der Referenzen */

    struct structReference
    {
        QString ReferenceURI;		//!< URI der Referenz
        int ReferenceID;            //!< Referenz ID
    };


public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    int         gi_EOL;                 //!< End-of-Line Zeichen fuer die Outputdateien
    int         gi_Codec;               //!< Zeichenkodierung
    int         gi_Extension;           //! Dateierweiterung fuer Ausgabedateien
    int         gi_NumOfProgramStarts;  //!< Anzahl der Programmstarts

    bool        gb_showProgressBar;     //!< Soll der ProgressBar im Fenster unten rechts angezeigt werden?

    int			gi_ActionNumber;        //!< Die ActionNumber wird bei jedem angewandenten Tool um 1 erhoeht.
    int			gi_SkipNFirstLines;     //!< Anzahl der Zeilen, die beim Verbinden von Dateien am Anfang uebersprungen werden sollen.
    int         gi_FieldDelimiter;      //!< Felddelimiter fr den Format Converter
    int         gi_FieldWidth;          //!< Feldbreite fr den Format Converter
    int         gi_FieldAlignment;      //!< Textausrichtung fr den Format Converter
    int         gi_lastReferenceID;     //!< letzte gesetzte Reference ID

    QString		gs_DownloadPath;        //!< Pfad fuer die Ablage der Station-to-archive Dateien
    QString		gs_User;                //!< Accountname
    QString		gs_Password;            //!< Password
    QString		gs_FTPServer;           //!< Adresse des Servers

    bool		gb_DecompressFiles;		//!< Wenn true werden die Dateien mit GZip entpackt
    bool		gb_CheckFiles;			//!< Wenn true werden die Dateien mit BSRN_fcheck geprueft
    bool		gb_CheckAvailability;	//!< Wenn true wird nur die Verfuegbarkeit der Dateien geprueft
    bool		gb_DeleteOriginalFiles;	//!< Wenn true werden die Original-Dateien nach dem Verbinden geloescht.
    bool        gb_OverwriteDataset;    //!< Wenn true wird der Datensatz berschrieben.

    bool		gb_Station[MAX_NUM_OF_STATIONS+1];	//!< Array von Flags, wenn Flag[i] = true werden die Datei der ausgewaehlten Station geladen
    bool		gb_Month[MAX_NUM_OF_MONTHS+1];		//!< Array von Monaten
    bool		gb_Year[MAX_NUM_OF_YEARS+1];		//!< Array von Jahren

    QString     gs_MissingValue;    //!< Missing value fr den Format Converter
    QString		gs_FilenameFormat;	//!< Bestimmt in die Form des automatisch erzeugten neuen Dateinamens. \%a = aktuelle Actionnumber, \%N = Dateiname.
    QString		gs_Path;			//!< Der zuletzt verwendete Pfad.
    QString     gs_Version;         //!< Programmversion

    QStringList gsl_FilenameList;	//!< Liste aller InputDateinamen incl. Pfad.

    bool        gb_CheckPhysicallyPossibleLimits; //!< If true perform quality check "Physically Possible Limits"
    bool        gb_CheckExtremelyRareLimits;      //!< If true perform quality check "Extremely Rare Limits"
    bool        gb_CheckComparisons;              //!< If true perform quality check "Comparisons"
    bool        gb_OutputCodes;                   //!< If true write quality codes to file after quality check
    bool        gb_OutputCleanedValues;           //!< If true write cleaend values to file after quality check
    bool        gb_OutputOriginalValues;          //!< If true write original values to file after quality check
    bool        gb_OutputAuxiliaryData;           //!< If true write calculated auxiliary data to file after quality check
    bool        gb_OutputOneFile;                 //!< If true write all data from quality check to one file

    QString     gs_AuxiliaryDataAlgorithm;        //!< Algorithm to use for determining auxiliary data during quality check

protected:
    void dragEnterEvent( QDragEnterEvent *event );
    void dropEvent( QDropEvent *event );
    void closeEvent( QCloseEvent *event );
    void onError( const int err );

signals:
    void finishedBuildFilelist( bool );

private slots:
    int  chooseFile();
    int  chooseFiles();
    int  chooseFolder( const QString& Folder = "" );
    int  saveFile();
    int  saveFileAs();

    void exitApplication();
    void about();
    void hideWindow();
    void newWindow();
    void displayHelp();
    void clearStatusMessage();
    int  incProgress( const int Step );

// Station-to-arvive
    void doDownloadStationToArchiveFiles();
    void doCheckStationToArchiveFiles();

// Metadata
    void doFileIDConverter();
    void doScientistIDConverter();
    void doMessagesConverter();
    void doStationDescriptionConverter();
    void doRadiosondeEquipmentConverter();
    void doOzoneEquipmentConverter();
    void doStationHistoryConverter();
    void doRadiationInstrumentsConverter();
    void doAssignmentConverter();
    void doCreateReferenceImportFile();
    void doAllMetadataConverter();
    void doRefreshBsrnIDs();
    void doRefreshBsrnReferenceIDs();

//  Data
    void doBasicMeasurementsConverter( const bool Import = false );
    void doOtherMinuteMeasurementsConverter();
    void doUltraVioletMeasurementsConverter( const bool Import = false );
    void doSYNOPConverter( const bool Import = false );
    void doRadiosondeMeasurementsConverter( const bool Import = false );
    void doOzoneMeasurementsConverter( const bool Import = false );
    void doExpandedMeasurementsConverter( const bool Import = false );
    void doOtherMeasurementsAtXmConverter( const bool Import = false, const int Height = 10 );
    void doOtherMeasurementsAt10mConverter();
    void doOtherMeasurementsAt30mConverter();
    void doOtherMeasurementsAt300mConverter();
    void doAllDataConverter();

//  Import
    void doBasicMeasurementsImportConverter();
    void doUltraVioletMeasurementsImportConverter();
    void doSYNOPImportConverter();
    void doRadiosondeMeasurementsImportConverter();
    void doOzoneMeasurementsImportConverter();
    void doExpandedMeasurementsImportConverter();
    void doOtherMeasurementsAt10mImportConverter();
    void doOtherMeasurementsAt30mImportConverter();
    void doOtherMeasurementsAt300mImportConverter();
    void doAllImportConverter();
    void doSetOverwriteDatasetFlag();

//  Tools
    void doConcatenateFiles();
    void doWinConvertEOL();
    void doMacOSConvertEOL();
    void doCompressFile();
    void doConvertUnformatted();
    void doConvertFormatted();
    void doCreateReplaceDatabase();

//  Quality management
    void doQualityCheckRecommendedV20();
    //void doQualityCheckTechnicalReport1();
    //void doQualityCheckUserDefined();

//  External URLs
    void doOpenExternalURLHelp() { OpenExternalURL( _HELP_ ); }
    void doOpenExternalURLBSRNStationToArchiveFormat() { OpenExternalURL( _BSRNSTATIONTOARCHIVEFORMAT_ ); }
    void doOpenExternalURLBSRNStatus() { OpenExternalURL( _BSRNSTATUS_ ); }
    void doOpenExternalURLBSRNHomepage() { OpenExternalURL( _BSRNHOMEPAGE_ ); }
    void doOpenExternalURLBSRNLatestSnapshot() { OpenExternalURL( _BSRNLATESTSNAPSHOT_ ); }
    void doOpenExternalURLBSRNAccount() { OpenExternalURL( _BSRNACCOUNT_ ); }

private:
    QStringList expandCommandline();
    QStringList renameFiles( const QStringList Filename, const QString &searchStr, const QString &replaceStr );
    bool buildFilename( const int ActionNumber, const QString &FilenameFormat, const int Extension, const QString &Filename, QString &FilenameIn, QString &FilenameOut );
    bool containsBinaryFile( const QStringList FilenameList );
    bool existsFirstFile( const int ActionNumber, const QString &FilenameFormat, const int Extension, QStringList &FilenameList );
    bool isEmptyLine( const QString &String );
    bool check7z();
    int NumOfSections( const QString &String );
    int addToFilenameList( QStringList &FilenameList, const QString &Filename, const bool showAllFiles = true, const bool decompressFile = false );
    int calcFileSizeClass( const QString &FilenameIn, const int NumOfFiles, const int lowerLimit = 100, const int upperLimit = 1000 );
    int copyStringList( const QStringList InStringList, QStringList &OutStringList, const QString &s_Pattern = "*" );
    int emptyDir( const QString &Dir );
    int decompressFile( const QString &Filename, const bool delZipFile = false );
    int incFileProgress( const int NumOfFiles, const int FileNumber );
    int incProgress( const int NumOfFiles, const int Step );
    int readFile( const QString &FilenameIn, QStringList &Input, const int Codec = -1, const int NumOfFiles = 0, const qint64 Bytes = 0 );
    int removeFile( const QString &Filename );
    int downloadFile( const QString &Url, const QString &absoluteFilePath );
    unsigned int incProgress( const int NumOfFiles, const unsigned int filesize, const unsigned int length, const QString &InputStr );
    void appendItem( QStringList &List, const QString &Item, const QString &SS = "", const QString &RS = "" );
    void compressFile( const QString &FilenameIn );
    void clearFilenameList( int &ActionNumber, QStringList &FilenameList );
    void clearList( QStringList& List );
    void clearMessage();
    void createActions();
    void createMenus();
    void createStatusBar( const bool showProgressBar );
    void enableMenuItems( const QStringList FilenameList );
    void endTool( const int err, const int stopProgress, int &ActionNumber, const QString &FilenameFormat, const int Extension, QStringList &FilenameList, const QString &doneMessage = "Done", const QString &canceledMessage = "Converter was canceled", const bool clearList = false, const bool incActionNumber = true );
    void initFileProgress( const int NumOfFiles, const QString &FilenameIn, const QString &MessageText );
    void initProgress( const int NumOfFiles, const QString &Filename, const QString &MessageText, const int totalNumberOfSteps );
    void initProgress( const int totalNumberOfSteps );
    void listDir( QStringList &Filename, const QString &Directory, const bool showAllFiles = true, const bool decompressFile = false );
    void loadPreferences();
    void resetFileProgress( const int NumOfFiles );
    void resetProgress( const int NumOfFiles = 0 );
    void savePreferences();
    void setNormalCursor();
    void setStatusBar( const QString &Message = "", const int seconds = 0 );
    void setStatusBarFileInProgress( const QString &Filename, const QString &Message = "", const int i_seconds = 0 );
    void setWTitle( const int ActionNumber, const QString &FilenameFormat, const int Extension, const QStringList sl_FilenameList );
    void setWaitCursor();
    void showFilenameList( const int ActionNumber, const QString &FilenameFormat, const int Extension, const QStringList FilenameList );
    void showList( const QStringList List );
    void showMessage( const QString &Message, QStringList &MessageList );
    void wait( const int msec );

    QString getApplicationName( const bool replaceUnderline = false );
    QString getDataLocation();
    QString getDocumentDir();
    QString getPreferenceFilename();
    QString getVersion();
    QString setEOLChar( const int EOL );
    QString setExtension( const int Extension );
    QString getFileChecksum( const QString &FilenameIn );

    bool warning( const QString & Message, const QString & Title = tr("Warning"));

// Station-to-archive
    int downloadStationToArchiveFiles( structStation *Station_ptr, const QString& FilenameOut, const QString& FTPServer, const QString& User, const QString& Password, const bool DecompressFiles, const bool CheckFiles, const bool CheckAvailability, bool Station[MAX_NUM_OF_STATIONS+1], bool Month[MAX_NUM_OF_MONTHS+1], bool Year[MAX_NUM_OF_YEARS+1] );

// Metadata
    int FileIDConverter( const QString& FilenameIn, QStringList& FilenameOut, structStation *Station_ptr, const int NumOfFiles );
    int ScientistIDConverter( const QString& FilenameIn, QStringList& FilenameOut, structStaff *Staff_ptr, structStation *Station_ptr, const int NumOfFiles );
    int MessagesConverter( const QString& FilenameIn, QStringList& FilenameOut, structStation *Station_ptr, const int NumOfFiles );
    int StationDescriptionConverter( const QString& FilenameIn, QStringList& FilenameOut, structStation *Station_ptr, const int NumOfFiles );
    int RadiosondeEquipmentConverter( const QString& FilenameIn, QStringList& FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int NumOfFiles );
    int OzoneEquipmentConverter( const QString& FilenameIn, QStringList& FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int NumOfFiles );
    int StationHistoryConverter( const QString& FilenameIn, QStringList& FilenameOut, structStation *Station_ptr, const int NumOfFiles );
    int RadiationInstrumentsConverter( const QString& FilenameIn, QStringList& FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int NumOfFiles );
    int AssignmentConverter( const QString& FilenameIn, QStringList& FilenameOut, structMethod *Method_ptr, structStation *Station_ptr, const int NumOfFiles );
    int CreateReferenceImportFile( const QString& FilenameIn, QStringList& FilenameOut, structStaff *Staff_ptr, structStation *Station_ptr, const int NumOfFiles );

// Data
    bool SYNOPTest1( const QString &InputStr, int *P );
    bool SYNOPTest2( const QString &InputStr, int *P );
    bool SYNOPTest3( const QString &InputStr, const int Year, int *P );
    bool SYNOPTest4( const QString &InputStr, int *P );
    bool SYNOPTest5( const QString &InputStr, int *P );
    bool SYNOPTest6( const QString &InputStr, int *P );

    QStringList buildSYNOPDataDescriptionHeader1( int *P, const int PIID );
    QStringList buildSYNOPDataDescriptionHeader2( int *P, const int PIID );
    QStringList buildSYNOPDataDescriptionHeader3( int *P, const int PIID, const int Year );
    QStringList buildSYNOPDataDescriptionHeader4( int *P, const int PIID );
    QStringList buildSYNOPDataDescriptionHeader5( int *P, const int PIID );
    QStringList buildSYNOPDataDescriptionHeader6( int *P, const int PIID );

    QString buildSYNOPDataHeader1( int *P, const bool Import );
    QString buildSYNOPDataHeader2( int *P, const bool Import );
    QString buildSYNOPDataHeader3( int *P, const int Year, const bool Import );
    QString buildSYNOPDataHeader4( int *P, const bool Import );
    QString buildSYNOPDataHeader5( int *P, const bool Import );
    QString buildSYNOPDataHeader6( int *P, const bool Import );

    QString buildSYNOPDataOutputStr1( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );
    QString buildSYNOPDataOutputStr2( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );
    QString buildSYNOPDataOutputStr3( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );
    QString buildSYNOPDataOutputStr4( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );
    QString buildSYNOPDataOutputStr5( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );
    QString buildSYNOPDataOutputStr6( const QString EventLabel, const QString DataStr, int *P, const int Year, const int Month, const float Latitude, const float Longitude, const bool Import );

    int BasicMeasurementsTest( const QString& FilenameIn, int *P, const int NumOfFiles );
    int UltraVioletMeasurementsTest( const QString& FilenameIn, int *P, const int NumOfFiles );
    int SYNOPTest( const QString& FilenameIn, int *P, const int NumOfFiles );
    int RadiosondeMeasurementsTest( const QString& FilenameIn, int *P, const int NumOfFiles );
    int ExpandedMeasurementsTest( const QString& FilenameIn, int *P, const int NumOfFiles );
    int OtherMeasurementsAtXmTest( const QString& FilenameIn, int *P, const int Height, const int NumOfFiles );

    int BasicMeasurementsConverter( const bool Import, const bool showSelectParameterDialog, const int Mode, const QString& FilenameIn, QString& FilenameOut, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int OtherMinuteMeasurementsConverter( const bool Import, const QString& FilenameIn, structStaff *Staff_ptr, structStation *Station_ptr, int *P, const int NumOfFiles );
    int UltraVioletMeasurementsConverter( const bool Import, const bool showSelectParameterDialog, const QString& FilenameIn, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int SYNOPConverter( const bool Import, const QString& FilenameIn, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int RadiosondeMeasurementsConverter( const bool Import, const QString& FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int OzoneMeasurementsConverter( const bool Import, const QString& FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int ExpandedMeasurementsConverter( const bool Import, const QString& FilenameIn, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int NumOfFiles );
    int OtherMeasurementsAtXmConverter( const bool Import, const QString& FilenameIn, const int Height, structParameter *Parameter_0001, structParameter *Parameter_0009, structMethod *Method_ptr, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles );

// Dialogs
    int doConcatenateOptionsDialog( int &SkipNFirstLines, bool &deleteOriginalFiles );
    int doDownloadManagerDialog( QString &DownloadPath, QString &FTPServer, QString &User, QString &Password, bool &DecompressFiles, bool &CheckFiles, bool &CheckAvailability, bool Station[MAX_NUM_OF_STATIONS+1], bool Month[MAX_NUM_OF_MONTHS+1], bool Year[MAX_NUM_OF_YEARS+1] );
    int doQualityCheckRecommendedV20OptionsDialog( bool & b_CheckPhysicallyPossibleLimits, bool & b_CheckExtremelyRareLimits, bool & b_CheckComparisons, QString & s_AuxiliaryDataAlgorithm, bool & b_OutputCodes, bool & b_OutputCleanedValues, bool & b_OutputOriginalValues, bool & b_OutputAuxiliaryData, bool & b_OutputOneFile);
    int doFormatUnformattedOptionsDialog( QString &MissingValue, int &FieldDelimiter );
    int doFormatFormattedOptionsDialog( int &FieldAlignment, int &FieldWidth, QString &MissingValue );
    int doSelectParametersDialog( const int mode, int *P );

// Tools
    QString ReferenceOtherVersion( const QString& EventLabel, structReference *Reference_ptr, const QDateTime dt );
    QString ReferenceImportFile( const QString& EventLabel, const QDateTime DateOfData, const int PIID, const QString &StationName );

    QString OpenDataDescriptionHeader();
    QString CloseDataDescriptionHeader();
    QString ParentID( const QString& ParentID = "-999" );
    QString DataSetID( const QString& DatasetID = "-999" );
    QString AuthorIDs( const QString& AuthorIDs = "-999" );
    QString SourceID( const QString& SourceID = "-999" );
    QString DatasetTitle( const QString& Text, const QString& StationName, const QDateTime dt );
    QString Reference( const QString &ReferenceID = "-999", const int RelationTypeID = -999, const QString &ReferenceType = "-999", const QString& EventLabel = "" );
    QString ExportFilename( const QString& EventLabel, const QString& Text, const QDateTime dt );
    QString EventLabel( const QString& EventLabel = "" );
    QString Parameter( const QString& ParameterID = "-999", const QString& PIID = "506", const QString& MethodID = "43", const QString& Format = "", const QString& Comment = "" );
    QString Parameter( const QStringList Parameter );
    QString DatasetComment( const QString& DatasetComment = "" );
    QString ProjectIDs( const QString& ProjectID = "-999" );
    QString TopologicTypeID( const QString& TopologicTypeID = "-999" );
    QString StatusID( const QString& StatusID = "-999" );
    QString UserIDs( const QString& UserIDs = "-999" );
    QString LoginID( const QString& LoginID = "-999" );

    QString num2str( const int num ) { return( QString::number( num ) ); }
    QString num2str( const float num ) { return( QString::number( num ) ); }
    QString num2str( const double num ) { return( QString::number( num ) ); }

    QString findEventLabel( const int StationNumber, structStation *Station_ptr );
    QString findStationName( const int StationNumber, structStation *Station_ptr );
    bool checkFilename( const QString& s_Filename, const QString& s_EventLabel, const QString& s_Month, const QString& s_Year );
    int checkSelectedParameter( const int offset, int *P, int *PoM );
    int checkSelectedParameter( const int offset, int *P );

    int findInstituteID( const int StationNumber, structStation *Station_ptr );
    int findPiID( const QString& Name, structStaff *Staff_ptr );
    int findMethodID( const int StationNumber, const int WRMCnumber, structMethod *Method_ptr );
    int findMethodID( const QString& RadiosondeIdentification, structMethod *Method_ptr );
    int findDatasetID( const QString& ExportFilename, structDataset *Dataset_ptr );
    int findReferenceID( const QString& ReferenceURI, structReference *Reference_ptr );

    int readBsrnIDs();
    int readBsrnDatasetIDs();
    int readBsrnReferenceIDs( const bool updateReferenceIDs );

    int writeDefaultIDsBSRN( const QString& Filename );

    int concatenateFiles( const QString& FilenameOut, const QStringList Filenames, const QString& ProgressMessage, const int SkipLines = 0, const bool RemoveFile = false );
    int convertEOL( const QString& FilenameIn, const bool convertEOL, const int OS );
    int convertFile( const QString& FilenameIn, const int NumOfFiles );
    int convertFile( const QString& FilenameIn, const QString& FilenameOut, const QString& MissingValue, const int FieldDelimiter, const int NumOfFiles );
    int convertFile( const QString& FilenameIn, const QString& FilenameOut, const int FieldAlignment, const int FieldWidth, const QString& MissingValue, const int NumOfFiles );
    int createReplaceDatabase( const QString& FilenameIn, const int firstReferenceID, const int NumOfFiles );

    float calcGeopotentialHeight( const QString& ahhh );

    void OpenExternalURL( const int URL = 1 );

    QString writeGeocodeHeader( const bool Import, int *P );
    QString buildGeocodeEntries( const bool Import, int *P, const QDateTime dt, const QString EventLabel, const float Latitude, const float Longitude );


//  Quality check
    int QualityCheckRecommendedV20( const QString & FileNameIn, QString & FileNameOut, const bool & b_CheckPhysicallyPossibleLimits, const bool & b_CheckExtremelyRareLimits, const bool & b_CheckComparisons, const QString & s_AuxiliaryDataAlgorithm, const bool & b_OutputCodes, const bool & b_OutputCleanedValues, const bool & b_OutputOriginalValues, const bool & b_OutputAuxiliaryData, const bool & b_OutputOneFile);
    //int QualityCheckTechnicalReport1( const QString& FilenameIn );
    //int QualityCheckUserDefined( const QString& FilenameIn );

    QMenu	*fileMenu;
    QMenu	*stationMenu;
    QMenu	*metadataMenu;
    QMenu	*dataMenu;
    QMenu	*importMenu;
    QMenu	*toolsMenu;
    QMenu   *qualityMenu;
    QMenu	*helpMenu;

    QAction *newWindowAction;
    QAction *openFileAction;
    QAction *openFolderAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *hideWindowAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *helpAction;

    QAction *openExternalURLHelpAction;
    QAction *openExternalURLBSRNStationToArchiveFormatAction;
    QAction *openExternalURLBSRNStatusAction;
    QAction *openExternalURLBSRNHomePageAction;
    QAction *openExternalURLBSRNLatestSnapshotAction;
    QAction *openExternalURLBSRNAccountAction;

    QAction *downloadStationToArchiveFilesAction;
    QAction *checkStationToArchiveFilesAction;

    QAction *allMetadataAction;
    QAction *refreshBsrnIDsAction;
    QAction *refreshBsrnReferenceIDsAction;
    QAction *fileIDAction;
    QAction *scientistIDAction;
    QAction *messagesAction;
    QAction *stationDescriptionAction;
    QAction *radiosondeEquipmentAction;
    QAction *ozoneEquipmentAction;
    QAction *stationHistoryAction;
    QAction *radiationInstrumentsAction;
    QAction *assignmentAction;
    QAction *createRefFileAction;

    QAction *allDataAction;
    QAction *basicMeasurementsAction;
    QAction *otherMinuteMeasurementsAction;
    QAction *uvMeasurementsAction;
    QAction *synopAction;
    QAction *radiosondeMeasurementsAction;
    QAction *ozoneMeasurementsAction;
    QAction *expandedMeasurementsAction;
    QAction *otherMeasurementsAt10mAction;
    QAction *otherMeasurementsAt30mAction;
    QAction *otherMeasurementsAt300mAction;

    QAction *allImportAction;
    QAction *basicMeasurementsImportAction;
    QAction *uvMeasurementsImportAction;
    QAction *synopImportAction;
    QAction *radiosondeMeasurementsImportAction;
    QAction *ozoneMeasurementsImportAction;
    QAction *expandedMeasurementsImportAction;
    QAction *otherMeasurementsAt10mImportAction;
    QAction *otherMeasurementsAt30mImportAction;
    QAction *otherMeasurementsAt300mImportAction;
    QAction *setOverwriteDatasetFlagAction;

    QAction *convertUnformattedAction;
    QAction *convertFormattedAction;
    QAction *concatenateFilesAction;
    QAction *WinConvertEOLAction;
    QAction *MacOSConvertEOLAction;
    QAction *compressFileAction;
    QAction *createReplaceDatabaseAction;

    QAction *QualityCheckRecommendedV20Action;

    QPointer<QProgressDialog> ProgressDialog;
    QPointer<QProgressDialog> FileProgressDialog;
    QPointer<QProgressBar>    ProgressBar;

    QErrorMessage       *ErrorMessage;
    QLabel              *StatusMessage;

    QStringListModel	*Model;
    QListView			*ListView;

    QTextEdit			*textViewer;

    QPoint	posWindow;	             //!< Position des Programmfensters.
    QSize	sizeWindow;	             //!< Groesse des Programmfensters.

    QPoint  posDialog;               //!< Position der Dialoge
    QSize	sizeParameterDialog;	 //!< Groesse der Parameterdialogs.

    structStation	*g_Station_ptr;
    structStaff		*g_Staff_ptr;
    structMethod	*g_Method_ptr;
    structDataset	*g_Dataset_ptr;
    structReference	*g_Reference_ptr;
};

#endif
