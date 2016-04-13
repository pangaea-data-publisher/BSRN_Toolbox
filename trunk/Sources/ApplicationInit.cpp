/* ApplicationInit.cpp        */
/* 2009-08-09                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Initalisierung des CentralWidget.
*
*   Am Beginn der Methode wird der Cursor auf den WaitCursor des Betriebssystems
*   gesetzt. Dann folgt die Intialisierung einiger globaler Variablen (gi_ActionNumer, gsl_FilenameList).
*   Model und ListView werden erzeugt und das CentralWidget gesetzt. Nach dem Laden
*   der Programmeinstellungen werden die Menues erzeugt. Die Fensterposition und -groesse wird
*   gesetzt. Abschliessend wird das Fenster angezeigt und der Cursor zuruek auf den NormalCursor gesetzt.
*/

MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent )
{
// **********************************************************************************************
//  QObject::connect( this, SIGNAL( finishedBuildFilelist( bool ) ), this, SLOT( doIt( bool ) ) );
// **********************************************************************************************

    setWaitCursor();

// **********************************************************************************************
// init

    gi_ActionNumber     = 1;
    gi_Extension        = _TXT_;

    gs_Version          = "unknown";
    gs_FilenameFormat   = "zz_";

    gb_OverwriteDataset = false;

    gsl_FilenameList.clear();

// **********************************************************************************************

    #if defined(Q_OS_LINUX)
        gb_showProgressBar = true;
    #endif

    #if defined(Q_OS_MAC)
        gb_showProgressBar = false;
    #endif

    #if defined(Q_OS_WIN)
        gb_showProgressBar = true;
    #endif

// **********************************************************************************************

    Model               = new QStringListModel( this );
    ListView            = new QListView;

    ListView->setModel( Model );
    ListView->setEditTriggers( QAbstractItemView::NoEditTriggers );

    setCentralWidget( ListView );

    setAcceptDrops( true );

    g_Station_ptr = new structStation[MAX_NUM_OF_STATION+1];
    g_Staff_ptr	  = new structStaff[MAX_NUM_OF_STAFF+1];
    g_Method_ptr  = new structMethod[MAX_NUM_OF_METHOD+1];
    g_Dataset_ptr = new structDataset[MAX_NUM_OF_DATASET+1];

// **********************************************************************************************

    loadPreferences();

    if ( gi_NumOfProgramStarts++ < 1 )
        savePreferences();

    gs_Version = getVersion();

// **********************************************************************************************

    createActions();
    createMenus();
    createStatusBar( gb_showProgressBar );

// **********************************************************************************************

    this->resize( sizeWindow );
    this->move( posWindow );
    this->show();

// **********************************************************************************************

    gsl_FilenameList = expandCommandline();

// **********************************************************************************************

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );

// **********************************************************************************************

    QFile fID( getDataLocation() + "/" + "BSRN_IDs.txt" );

    if ( fID.exists() == true )
        fID.copy( getDataLocation() + "/" + "BSRN_IDs_old.txt" );

// **********************************************************************************************

    setStatusBar( tr( "Reading BSRN IDs database - please wait" ) );

    downloadFile( QLatin1String( "https://pangaea.de/PHP/bsrn/BSRN_IDs.txt" ), getDataLocation() + "/" + "BSRN_IDs.txt" );

// **********************************************************************************************

    setStatusBar( tr( "Ready" ), 2 );
    setNormalCursor();

    readIDsBSRN();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Öffnet URL.
*
*   Öffnet beliebige Links im Standard Browser
*/

void MainWindow::OpenExternalURL( const int i_URL )
{
    switch ( i_URL)
    {
    case _HELP_:
        QDesktopServices::openUrl( QUrl( tr( "https://wiki.pangaea.de/wiki/BSRN_Toolbox" ) ) );
        break;
    case _BSRNSTATUS_:
        QDesktopServices::openUrl( QUrl( tr( "https://pangaea.de/PHP/BSRN_Status.php" ) ) );
        break;
    case _GCOS_:
        QDesktopServices::openUrl( QUrl( tr( "http://hdl.handle.net/10013/epic.42596.d001" ) ) );
        break;
    default:
        QDesktopServices::openUrl( QUrl( tr( "https://pangaea.de" ) ) );
        break;
    }
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

MainWindow::~MainWindow()
{
    if ( g_Method_ptr != NULL )
    {
        delete []g_Method_ptr;
        g_Method_ptr = NULL;
    }

    if ( g_Staff_ptr != NULL )
    {
        delete []g_Staff_ptr;
        g_Staff_ptr = NULL;
    }

    if ( g_Station_ptr != NULL )
    {
        delete []g_Station_ptr;
        g_Station_ptr = NULL;
    }
}
