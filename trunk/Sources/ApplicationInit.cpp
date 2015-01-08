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

    bool    b_downloadIDs   = true;

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
    createStatusBar();

// **********************************************************************************************

    this->resize( sizeWindow );
    this->move( posWindow );
    this->show();

// **********************************************************************************************

    gsl_FilenameList = expandCommandline();

// **********************************************************************************************

    setWTitle( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList );

// **********************************************************************************************

    setStatusBar( tr( "Reading IDs - please wait." ) );

    QFileInfo fi( getDataLocation() );

// **********************************************************************************************

    if ( b_downloadIDs == true )
    {
        QFile fDatasetID( fi.absoluteFilePath() + "/" + "BSRN_Dataset_IDs.txt" );

        if ( fDatasetID.open( QIODevice::WriteOnly | QIODevice::Text ) == true )
        {
            webfile m_webfile;

            m_webfile.setUrl( QLatin1String( "http://www.pangaea.de/PHP/bsrn/BSRN_Dataset_IDs.txt" ) );

            if ( m_webfile.open() == true )
            {
                char    buffer[1024];
                qint64  nSize = 0;

                while ( ( nSize = m_webfile.read( buffer, sizeof( buffer ) ) ) > 0 )
                    fDatasetID.write( buffer, nSize );

                m_webfile.close();
            }

            fDatasetID.close();
        }

// **********************************************************************************************

        QFile fID( fi.absoluteFilePath() + "/" + "BSRN_IDs.txt" );

        if ( fID.exists() == true )
            fID.copy( fi.absoluteFilePath() + "/" + "BSRN_IDs_old.txt" );

        if ( fID.open( QIODevice::WriteOnly | QIODevice::Text ) == true )
        {
            webfile m_webfile;

            m_webfile.setUrl( QLatin1String( "http://www.pangaea.de/PHP/bsrn/BSRN_IDs.txt" ) );

            if ( m_webfile.open() == true )
            {
                char    buffer[1024];
                qint64  nSize = 0;

                while ( ( nSize = m_webfile.read( buffer, sizeof( buffer ) ) ) > 0 )
                    fID.write( buffer, nSize );

                m_webfile.close();
            }

            fID.close();
        }
    }

// **********************************************************************************************

    readIDs();

// **********************************************************************************************

    setStatusBar( tr( "Ready" ), 2 );
    setNormalCursor();
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
