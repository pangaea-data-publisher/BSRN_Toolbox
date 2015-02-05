/* BSRN_ToolboxTools.cpp          */
/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

const QString PrefDate = "BSRN Toolbox, 2008-05-20";

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ReferenceOtherVersion( const QString& s_EventLabel, const QDateTime dt, const int i_PIID, const QString& s_StationName )
{
    QString s_ReferenceStr = "Reference:\t";
    QString s_URL          = "ftp://ftp.bsrn.awi.de/" + s_EventLabel.toLower() + "/" + s_EventLabel.toLower() + dt.toString( "MMyy" ) + ".dat.gz";
    QString s_Relationtype = QString( " * RELATIONTYPE: %1" ).arg( _RELATIONTYPE_ );
    QString s_Authors      = QString( " * AUTHORS: %1" ).arg( i_PIID );
    QString s_Year         = QString( " * YEAR: %1" ).arg( dt.toString( "yyyy") );
    QString s_Title        = " * TITLE: BSRN Station-to-archive file for ";

    if ( s_StationName.endsWith( "Station" ) == false )
        s_Title.append( "station " );

    s_Title.append( s_StationName + " (" + dt.toString( "yyyy-MM" ) + ")" );

    s_ReferenceStr.append( s_URL );
    s_ReferenceStr.append( s_Relationtype );
    s_ReferenceStr.append( s_Authors );
    s_ReferenceStr.append( s_Year );
    s_ReferenceStr.append( s_Title );

    return( s_ReferenceStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Testet ob der Dateiname mit der Stationnummer und den Datumsangaben uebereinstimmt.
*
*   @param s_Filename Name der Datei
*   @param s_EventLabel Stationsname
*   @param s_Month Monat
*   @param s_Year Jahr in Kurzform
*
*	@return false, wenn Fehler
*/

bool  MainWindow::checkFilename( const QString& s_Filename, const QString& s_EventLabel, const QString& s_Month, const QString& s_Year )
{
    QString s_testFilename = s_EventLabel.toLower();

    s_testFilename.append( s_Month );
    s_testFilename.append( s_Year );
    s_testFilename.append( ".dat" );
    s_testFilename.replace( " ", "0" );

    if ( s_Filename != s_testFilename )
        return( false );

    return( true );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die PANGAEA ID des Datasatzes.
*
*   @param s_Exportfilename Name des Exportfiles
*   @param Dataset_ptr Pointer auf Array aller Datensaetze
*
*	@return ID des Datensatzes. -999 wird zurueckgegeben wenn keine ID gefunden wurde
*/

int MainWindow::findDatasetId( const QString& s_ExportFilename, structDataset *Dataset_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_DATASET; ++i )
    {
        if ( Dataset_ptr[i].ExportFilename == s_ExportFilename )
            return( Dataset_ptr[i].DatasetID );
    }

    return ( -999 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die PANGAEA ID des Stationsleiters.
*
*   @param s_StationScientist Name des Stationsleiters
*   @param Staff_ptr Pointer auf Array aller Personen
*
*	@return ID der Person. -999 wird zurueckgegeben wenn keine ID gefunden wurde
*/

int MainWindow::findPiId( const QString& s_StationScientist, structStaff *Staff_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_STAFF; ++i )
    {
        if ( Staff_ptr[i].StationScientist == s_StationScientist )
            return( Staff_ptr[i].StaffID );
    }

    return ( -999 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die PANGAEA ID der Methode.
*
*   @param i_StationNumber Nummer der Station
*   @param i_WRMCnumber WRMC Code des Geraetes = PANGAEA Methode
*   @param Method_ptr Pointer auf Array aller Methoden
*
*	@return ID der Methode. -999 wird zurueckgegeben wenn keine ID gefunden wurde
*/

int MainWindow::findMethodID( const int i_StationNumber, const int i_WRMCnumber, structMethod *Method_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_METHOD; ++i )
    {
        if ( ( Method_ptr[i].StationID == i_StationNumber ) && ( Method_ptr[i].WRMCnumber == i_WRMCnumber ) )
            return( Method_ptr[i].MethodID );
    }

    return( -999 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die PANGAEA ID der Methode.
*
*   @param s_InstrumentIdentificationNumber Identifikationsnummer des Geraetes = PANGAEA Methode
*   @param Method_ptr Pointer auf Array aller Methoden
*
*	@return ID der Methode. -999 wird zurueckgegeben wenn keine ID gefunden wurde
*/

int MainWindow::findMethodID( const QString& s_InstrumentIdentificationNumber, structMethod *Method_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_METHOD; ++i )
    {
        if ( Method_ptr[i].SerialNumber == s_InstrumentIdentificationNumber )
            return( Method_ptr[i].MethodID );
    }

    return( -999 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt den Kurznamen der Station.
*
*   @param i_StationNumber Nummer der Station
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*
*	@return Kurzname der Station. "unknown" wird zurueckgegeben wenn keine Station gefunden wurde
*/

QString MainWindow::findEventLabel( const int i_StationNumber, structStation *Station_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_STATION; ++i )
    {
        if ( Station_ptr[i].StationID == i_StationNumber )
            return( Station_ptr[i].EventLabel );
    }

    return ( "unknown" );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt den Namen der Station.
*
*   @param i_StationNumber Nummer der Station
*   @param Station_ptr Pointer auf Array aller Stationsnamen
*
*	@return Name der Station. "unknown" wird zurueckgegeben, wenn keine Station gefunden wurde
*/

QString MainWindow::findStationName( const int i_StationNumber, structStation *Station_ptr )
{
    QString	s_StationName	= "";

    for ( int i=1; i<=MAX_NUM_OF_STATION; ++i )
    {
        if ( Station_ptr[i].StationID == i_StationNumber )
        {
            s_StationName = Station_ptr[i].StationName;
            s_StationName.replace( "Ny-&Aring%lesund", "Ny-Alesund" );
            s_StationName.replace( "S&atilde%o", "Sao" );
            return( s_StationName );
        }
    }

    return ( "unknown" );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt das Heimatinstitut der Station.
*
*   @param i_StationNumber Nummer der Station
*   @param Station_ptr Pointer auf Array aller Institute
*
*	@return ID des Instituts. -999 wird zurueckgegeben wenn kein Institut gefunden wurde
*/

int MainWindow::findInstituteID( const int i_StationNumber, structStation *Station_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_STATION; ++i )
    {
        if ( Station_ptr[i].StationID == i_StationNumber )
            return( Station_ptr[i].InstituteID );
    }

    return ( -999 );
}

// ***********************************************************************************************************************

/*! @brief Liest alle IDs aus einer Datei
*
*   @return Fehlercode, 0 = kein Fehler, -1 = keine Einstellungsdatei gefunden
*/

int MainWindow::readIDs()
{
    int		i					= 0;

    QString InputStr			= "";
    QString IDsFilename			= "";
    QString IDsFilenameOld		= "";
    QString CurrentDate			= "";

// **********************************************************************************************

    QFileInfo fi( getDataLocation() );

    IDsFilename     = fi.absoluteFilePath() + "/" + "BSRN_IDs.txt";
    IDsFilenameOld  = fi.absoluteFilePath() + "/" + "BSRN_IDs_old.txt";

    QFile fIDs( IDsFilename );
    QFile fIDs_old( IDsFilenameOld );

    setNormalCursor();

    if ( ( fIDs.exists() == false ) || ( fIDs.size() == 0 ) )
    {
        if ( fIDs.exists() == true )
            fIDs.remove();

        if ( fIDs_old.exists() == true )
        {
            fIDs_old.copy( IDsFilename );
            QMessageBox::warning( this, getApplicationName( true ), tr( "Cannot download BSRN ID file.\nAn old BSRN ID file will be used." ) );
        }
        else
        {
            writeDefaultIDs( IDsFilename );
            QMessageBox::warning( this, getApplicationName( true ), tr( "Cannot download BSRN ID file.\nThe default BSRN ID file will be used." ) );
        }
    }

    if ( fIDs.open( QIODevice::ReadOnly ) == false )
        return( -30 );

    setWaitCursor();

    QTextStream tinIDs( &fIDs );

    InputStr = tinIDs.readLine();
    InputStr = tinIDs.readLine();

    CurrentDate = InputStr.section( "\t", 0, 0 ).section( ", ", 1, 1 );

    if ( ( InputStr.startsWith( "BSRN Toolbox" ) == false ) || ( CurrentDate < PrefDate.section( ", ", 1, 1 ) ) )
    {
        fIDs.close();

        writeDefaultIDs( IDsFilename );
        readIDs();

        QMessageBox::warning( this, getApplicationName( true ), tr( "Wrong format of BSRN ID file.\nThe default BSRN ID file will be used." ) );

        return( _NOERROR_ );
    }

    InputStr = tinIDs.readLine();

    while ( tinIDs.atEnd() == false )
    {
        if ( InputStr.startsWith( "[Station]" ) == true )
        {
            InputStr = tinIDs.readLine(); i = 0;

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) )
            {
                // 1 \t ASP \t Alice Springs \t 2894

                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) & ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    g_Station_ptr[i].StationID	 = InputStr.section( "\t", 0, 0 ).toInt();
                    g_Station_ptr[i].EventLabel	 = InputStr.section( "\t", 1, 1 );
                    g_Station_ptr[i].StationName = InputStr.section( "\t", 2, 2 );
                    g_Station_ptr[i].InstituteID = InputStr.section( "\t", 3, 3 ).toInt();
                }
            }
        }

        if ( InputStr.startsWith( "[Staff]" ) == true )
        {
            InputStr = tinIDs.readLine(); i = 0;

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) )
            {
                // Ain Kallis \t 31684
                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) & ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    g_Staff_ptr[i].StationScientist	= InputStr.section( "\t", 0, 0 );
                    g_Staff_ptr[i].StaffID			= InputStr.section( "\t", 1, 1 ).toInt();
                }
            }
        }

        if ( InputStr.startsWith( "[Expanded]" ) == true )
        {
            InputStr = tinIDs.readLine(); i = 0;

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHOD ) )
            {
                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) & ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    g_Method_ptr[i].StationID	 = -999;
                    g_Method_ptr[i].SerialNumber = InputStr.section( "\t", 0, 0 ).simplified();
                    g_Method_ptr[i].WRMCnumber	 = -999;
                    g_Method_ptr[i].MethodID	 = InputStr.section( "\t", 1, 1 ).toInt();
                }
            }
        }

        if ( InputStr.startsWith( "[Ozonesonde]" ) == true )
        {
            InputStr = tinIDs.readLine(); // Header

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHOD ) )
            {
                // Beck Instruments, 91 \t 111111

                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) & ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    g_Method_ptr[i].StationID	 = -999;
                    g_Method_ptr[i].SerialNumber = InputStr.section( "\t", 0, 0 ).simplified();
                    g_Method_ptr[i].WRMCnumber	 = -999;
                    g_Method_ptr[i].MethodID	 = InputStr.section( "\t", 1, 1 ).toInt();
                }
            }
        }

        if ( InputStr.startsWith( "[Radiosonde]" ) == true )
        {
            InputStr = tinIDs.readLine();  // Header

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHOD ) )
            {
                // Vaisala DigiCora, RS80 \t 5435

                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) & ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    g_Method_ptr[i].StationID	 = -999;
                    g_Method_ptr[i].SerialNumber = InputStr.section( "\t", 0, 0 ).simplified();
                    g_Method_ptr[i].WRMCnumber	 = -999;
                    g_Method_ptr[i].MethodID	 = InputStr.section( "\t", 1, 1 ).toInt();
                }
            }
        }

        if ( InputStr.startsWith( "[Methods]" ) == true )
        {
            InputStr = tinIDs.readLine(); // Header

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHOD ) )
            {
                // 1, SN 28987F3, WRMC No. 1004 \t 5435
                InputStr = tinIDs.readLine();

                if ( ( InputStr.isEmpty() == false ) && ( InputStr.startsWith( "[" ) == false ) )
                {
                    ++i;

                    InputStr.replace( "\t", "," );
                    InputStr.replace( " WRMC No. ", "" );

                    g_Method_ptr[i].StationID	 = InputStr.section( ",", 0, 0 ).toInt();
                    g_Method_ptr[i].SerialNumber = InputStr.section( ",", 1, 1 ).simplified();
                    g_Method_ptr[i].WRMCnumber	 = InputStr.section( ",", 2, 2 ).toInt();
                    g_Method_ptr[i].MethodID	 = InputStr.section( ",", 3, 3 ).toInt();
                }
            }
        }

        if ( ( tinIDs.atEnd() == false ) && ( i > 0 ) )
            InputStr = tinIDs.readLine();
    }

    fIDs.close();
    fIDs_old.remove();

    if ( i == MAX_NUM_OF_METHOD )
        QMessageBox::information( this, getApplicationName( true ), tr( "Maximum number of methods was reached.\nPlease contact Rainer Sieger (rsieger@pangaea.de)" ) );

// **********************************************************************************************

    IDsFilename = fi.absolutePath() + "/" + "BSRN_Dataset_IDs.txt";

    fIDs.setFileName( IDsFilename );

    if ( fIDs.open( QIODevice::ReadOnly ) == false )
        return( -30 );

    InputStr = tinIDs.readLine(); // Date
    InputStr = tinIDs.readLine(); // Header

    i = 0;

    while ( ( tinIDs.atEnd() == false ) && ( i < MAX_NUM_OF_DATASET ) )
    {
        ++i;

        InputStr = tinIDs.readLine();

        g_Dataset_ptr[i].ExportFilename = InputStr.section( "\t", 0, 0 );
        g_Dataset_ptr[i].DatasetID      = InputStr.section( "\t", 1, 1 ).toInt();
    }

    fIDs.close();

    if ( i == MAX_NUM_OF_DATASET )
        QMessageBox::information( this, getApplicationName( true ), tr( "Maximum number of datasets was reached.\nPlease contact Rainer Sieger (rsieger@pangaea.de)" ) );

    return( _NOERROR_ );
}
// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Erzeugt die ID Datenbank
*
*   @param Filename Dateiname der Datenbank
*
*   @return Fehlercode
*/

int MainWindow::writeDefaultIDs( const QString& Filename )
{
    QFile fout( QDir::toNativeSeparators( Filename ) );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
        return( -20 );

//-----------------------------------------------------------------------------------------------------------------------

    QTextStream tout( &fout );

    tout << "[Version]" << endl;
    tout << PrefDate << "\t" << "from http://www.pangaea.de/PHP/bsrn/BSRN_IDs.txt" << endl;

    tout << "[Station]" << endl;
    tout << "BSRN Station ID\tPANGAEA Event label\tFull name\tPANGAEA Institute ID" << endl;
    tout << "1\tASP\tAlice Springs\t2894" << endl;
    tout << "18\tALE\tAlert\t2895" << endl;
    tout << "22\tBAR\tBarrow\t1809" << endl;
    tout << "24\tBER\tBermuda\t1809" << endl;
    tout << "28\tBIL\tBillings\t2876" << endl;
    tout << "32\tBON\tBondville\t2887" << endl;
    tout << "34\tBOS\tBoulder, SURFRAD\t2887" << endl;
    tout << "23\tBOU\tBoulder\t2887" << endl;
    tout << "71\tBRB\tBrazilia\t2901" << endl;
    tout << "14\tBUD\tBudapest\t3032" << endl;
    tout << "53\tCAB\tCabauw\t1057" << endl;
    tout << "50\tCAM\tCamborne\t2886" << endl;
    tout << "10\tCAR\tCarpentras\t2879" << endl;
    tout << "39\tCLH\tChesapeake Light\t2891" << endl;
    tout << "47\tCOC\tCocos Island\t2894" << endl;
    tout << "45\tCNR\tCener\t3047" << endl;
    tout << "19\tEUR\tEureka\t2895" << endl;
    tout << "40\tDAA\tDe Aar\t2897" << endl;
    tout << "2\tDAR\tDarwin\t2888" << endl;
    tout << "74\tDOM\tConcordia Station\t2963" << endl;
    tout << "35\tDRA\tDesert Rock\t2887" << endl;
    tout << "27\tE13\tS. Great Plains\t2876" << endl;
    tout << "3\tFLO\tFlorianopolis\t2889" << endl;
    tout << "31\tFPE\tFort Peck\t2887" << endl;
    tout << "6\tFUA\tFukuoka\t1671" << endl;
    tout << "33\tGCR\tGoodwin Creek\t2887" << endl;
    tout << "13\tGVN\tNeumayer Station\t32" << endl;
    tout << "38\tILO\tIlorin\t2880" << endl;
    tout << "7\tISH\tIshigakijima\t1671" << endl;
    tout << "61\tIZA\tIzana\t2965" << endl;
    tout << "25\tKWA\tKwajalein\t1809" << endl;
    tout << "60\tLAU\tLauder\t2894" << endl;
    tout << "51\tLER\tLerwick\t2886" << endl;
    tout << "12\tLIN\tLindenberg\t2885" << endl;
    tout << "49\tLRC\tLangley Research Center\t2891" << endl;
    tout << "29\tMAN\tMomote\t2888" << endl;
    tout << "8\tMNM\tMinamitorishima\t1671" << endl;
    tout << "30\tNAU\tNauru Island\t2888" << endl;
    tout << "11\tNYA\tNy-&Aring%lesund\t33" << endl;
    tout << "63\tPAL\tPalaiseau Cedex\t2884" << endl;
    tout << "21\tPAY\tPayerne\t2878" << endl;
    tout << "76\tPSA\tPlataforma Solar de Almeria\t20296" << endl;
    tout << "36\tPSU\tRock Springs\t2887" << endl;
    tout << "72\tPTR\tPetrolina\t2901" << endl;
    tout << "5\tREG\tRegina\t2895" << endl;
    tout << "73\tRLM\tRolim de Moura\t2901" << endl;
    tout << "4\tSAP\tSapporo\t1671" << endl;
    tout << "43\tSBO\tSede Boqer\t2881" << endl;
    tout << "70\tSMS\tS&atilde%o Martinho da Serra\t2901" << endl;
    tout << "41\tSOV\tSolar Village\t2882" << endl;
    tout << "26\tSPO\tSouth Pole\t1809" << endl;
    tout << "17\tSYO\tSyowa\t1673" << endl;
    tout << "37\tSXF\tSioux Falls\t2887" << endl;
    tout << "42\tTAM\tTamanrasset\t2893" << endl;
    tout << "16\tTAT\tTateno\t2877" << endl;
    tout << "9\tTOR\tToravere\t2892" << endl;
    tout << "44\tXIA\tXianghe\t2875" << endl;
    tout << "46\tZVE\tZvenigrod\t2998" << endl;
    tout << "65\tDWN\tDarwin Met Office\t2894" << endl;

    tout << "[Staff]" << endl;
    tout << "Station Scientist\tPANGAEA Staff ID\t\t" << endl;
    tout << "Dr. Gert Koenig-Langlo\t10396" << endl;
    tout << "Ellsworth Dutton\t31646" << endl;
    tout << "[Expanded]" << endl;
    tout << "Method text" << endl;
    tout << "Cloud base height is measured using the cloud ceilometer LD-WHX 137.1819.00/002\t6122" << endl;
    tout << "Cloud base height is measured using the laser ceilometer LD-40 137.2005.00\t6123" << endl;
    tout << "[Ozonesonde]" << endl;
    tout << "Manufacturer, Identification\tPANGAEA Method ID" << endl;
    tout << "Vaisala DigiCora, 01004\t6093" << endl;
    tout << "Vaisala DigiCora, 89002\t6093" << endl;
    tout << "[Radiosonde]" << endl;
    tout << "Manufacturer, Identification\tPANGAEA Method ID" << endl;
    tout << "Unknown manufacturer\t5041" << endl;
    tout << "Vaisala, RS90 \t6092" << endl;
    tout << "[Methods]" << endl;
    tout << "Station ID, Serial No., WRMC No.\tPANGAEA Method ID" << endl;
    tout << "1, SN 28987F3, WRMC No. 1004\t5435" << endl;
    tout << "1, SN 29075F3, WRMC No. 1005\t5436" << endl;
    tout << "1, SN 29076F3, WRMC No. 1006\t5437" << endl;
    tout << "1, SN 924021, WRMC No. 1002\t5432" << endl;
    tout << "1, SN 924048, WRMC No. 1001\t5433" << endl;
    tout << "1, SN 940042, WRMC No. 1003\t5434" << endl;
    tout << "[END]" << endl;

    fout.close();

    return( _NOERROR_ );
}
