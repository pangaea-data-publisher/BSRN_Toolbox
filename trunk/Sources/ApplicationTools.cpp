﻿/* BSRN_ToolboxTools.cpp          */
/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

const QString PrefDate = "BSRN Toolbox, 2008-05-20";

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ReferenceImportFile( const QString &s_EventLabel, const QDateTime DateOfData, const int i_PIID, const QString &s_StationName )
{
    QDate   date;

    QString s_ReferenceStr = "";
    QString s_Authors      = QString( "%1" ).arg( i_PIID );
    QString s_Year         = QString( "%1" ).arg( date.currentDate().toString( "yyyy") );
    QString s_Title        = "BSRN Station-to-archive file for ";
    QString s_URI          = "ftp://ftp.bsrn.awi.de/" + s_EventLabel.toLower() + "/" + s_EventLabel.toLower() + DateOfData.toString( "MMyy" ) + ".dat.gz";
    QString s_RefStatus    = "published";
    QString s_RefType      = "data set";

// **********************************************************************************************

    if ( s_StationName.endsWith( "Station" ) == false )
        s_Title.append( tr( "station " ) );

    s_Title.append( s_StationName + tr( " (" ) + DateOfData.toString( "yyyy-MM" ) + tr( ")" ) );

    s_ReferenceStr.append( s_Authors + tr( "\t" ) );
    s_ReferenceStr.append( s_Year + tr( "\t" ) );
    s_ReferenceStr.append( s_Title + tr( "\t" ) );
    s_ReferenceStr.append( s_URI + tr( "\t" ) );
    s_ReferenceStr.append( s_RefStatus + tr( "\t" ) );
    s_ReferenceStr.append( s_RefType );

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

bool  MainWindow::checkFilename( const QString &s_Filename, const QString &s_EventLabel, const QString &s_Month, const QString &s_Year )
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

int MainWindow::findDatasetID( const QString &s_ExportFilename, structDataset *Dataset_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_DATASETS; ++i )
    {
        if ( Dataset_ptr[i].ExportFilename == s_ExportFilename )
            return( Dataset_ptr[i].DatasetID );
    }

    return ( -999 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermittelt die PANGAEA ID der Station-to-archive File Referenz.
*
*   @param s_ReferenceURI URI des Station-to-archive Files (ftp://ftp.bsrn.awi.de/...)
*   @param Reference_ptr Pointer auf Array aller Referenzen
*
*	@return ID der Referenz. -999 wird zurueckgegeben wenn keine ID gefunden wurde
*/

int MainWindow::findReferenceID( const QString &s_ReferenceURI, structReference *Reference_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_REFERENCES; ++i )
    {
        if ( Reference_ptr[i].ReferenceURI == s_ReferenceURI )
            return( Reference_ptr[i].ReferenceID );
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

int MainWindow::findPiID( const QString &s_StationScientist, structStaff *Staff_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_STAFFS; ++i )
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
    for ( int i=1; i<=MAX_NUM_OF_METHODS; ++i )
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

int MainWindow::findMethodID( const QString &s_InstrumentIdentificationNumber, structMethod *Method_ptr )
{
    for ( int i=1; i<=MAX_NUM_OF_METHODS; ++i )
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
    for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
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

    for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
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
    for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
    {
        if ( Station_ptr[i].StationID == i_StationNumber )
            return( Station_ptr[i].InstituteID );
    }

    return ( -999 );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

/*! @brief Liest alle BSRN IDs aus einer Datei (BSRN IDs database)
*
*   @return Fehlercode, 0 = kein Fehler, -1 = keine Einstellungsdatei gefunden
*/

int MainWindow::readBsrnIDs()
{
    int		i					= 0;

    QString InputStr			= "";
    QString IDsFilename			= "";
    QString IDsFilenameOld		= "";
    QString CurrentDate			= "";

// **********************************************************************************************

    QFileInfo fi( getDataLocation() );

    IDsFilename     = fi.absoluteFilePath() + "/" + "BSRN_IDs.txt";   //+ "ohne_ZVE_BSRN_IDs.txt";
    IDsFilenameOld  = fi.absoluteFilePath() + "/" + "BSRN_IDs_old.txt";

    QFile fIDs( IDsFilename );
    QFile fIDs_old( IDsFilenameOld );


    if ( ( fIDs.exists() == false ) || ( fIDs.size() == 0 ) )
    {
        setStatusBar( tr( "Reading of BSRN IDs database fails" ) );

        if ( fIDs.exists() == true )
            fIDs.remove();

        if ( fIDs_old.exists() == true )
        {
            QMessageBox::warning( this, getApplicationName( true ), tr( "Cannot download BSRN IDs database.\nThe old BSRN IDs database will be used." ) );
            fIDs_old.copy( IDsFilename );
        }
        else
        {
            QMessageBox::warning( this, getApplicationName( true ), tr( "Cannot download BSRN IDs database.\nThe default BSRN IDs database will be used." ) );
            writeDefaultIDsBSRN( IDsFilename );
        }
    }

    if ( fIDs.open( QIODevice::ReadOnly ) == false )
        return( -11 ); // Can't open BSRN IDs database.

    QTextStream tinIDs( &fIDs );


    InputStr = tinIDs.readLine();
    InputStr = tinIDs.readLine();

    CurrentDate = InputStr.section( "\t", 0, 0 ).section( ", ", 1, 1 );

    if ( ( InputStr.startsWith( "BSRN Toolbox" ) == false ) || ( CurrentDate < PrefDate.section( ", ", 1, 1 ) ) )
    {
        fIDs.close();

        writeDefaultIDsBSRN( IDsFilename );
        readBsrnIDs();

        return( -42 ); // The format of the BSRN IDs database is wrong. Please contact roland.koppe@awi.de
    }

    InputStr = tinIDs.readLine();

    setWaitCursor();

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

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHODS ) )
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

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHODS ) )
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

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHODS ) )
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

            while ( ( tinIDs.atEnd() == false ) && ( InputStr.startsWith( "[" ) == false ) && ( i < MAX_NUM_OF_METHODS ) )
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

    setNormalCursor();


    if ( InputStr.section( "\t", 0, 0 ) != "[END]" )
    {
        fIDs_old.copy( IDsFilename );
        return( -60 ); // Download of BSRN IDs database fails. Please check your connection to the internet and refresh the database again.
    }


    fIDs_old.remove();

    if ( i == MAX_NUM_OF_METHODS )
        return( -70 ); // Maximum number of methods was reached. Please contact Roland Koppe (roland.koppe@awi.de)

    return( _NOERROR_ );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

/*! @brief Liest alle Reference IDs aus einer Datei
*
*   @return Fehlercode, 0 = kein Fehler, -30 = keine Datensatz ID Datenbank gefunden
*/

int MainWindow::readBsrnReferenceIDs( const bool b_updateReferenceIDs )
{
    int         i               = 0;
    int         n               = 0;

    QString     IDsFilename		= "";

    QStringList sl_Input;

// ***********************************************************************************************************************

    if ( b_updateReferenceIDs == true )
        g_Reference_ptr[0].ReferenceID = -999;

    if ( g_Reference_ptr[0].ReferenceID > 0 )
        return( _NOERROR_ );

// ***********************************************************************************************************************

    IDsFilename = getDataLocation()  + "/" + "BSRN_Reference_IDs.txt";

// ***********************************************************************************************************************

    setWaitCursor();
    setStatusBar( tr( "Reading BSRN reference IDs database - please wait" ) );

// ***********************************************************************************************************************

    QFileInfo fIDs( IDsFilename );

// ***********************************************************************************************************************

    if ( fIDs.size() == 0 )
    {
        setNormalCursor();
        setStatusBar( tr( "Download of BSRN reference IDs database fails" ), 2 );

        return( -62 ); // Download of BSRN reference IDs database fails. Please check your connection to the internet and refresh the database again.
    }

// ***********************************************************************************************************************

    n = readFile( IDsFilename, sl_Input, _UTF8_ ) - 1; // Number of lines - Header line

    if ( n == MAX_NUM_OF_REFERENCES )
    {
        setNormalCursor();
        setStatusBar( tr( "Maximum number of datasets was reached" ), 2 );

        return( -72 ); // Maximum number of datasets was reached. Please contact Roland Koppe (roland.koppe@awi.de)
    }

    for ( int j=n; j>0; --j )
    {
        i++;

        g_Reference_ptr[i].ReferenceID  = sl_Input.at( j ).section( "\t", 0, 0 ).toInt();
        g_Reference_ptr[i].ReferenceURI = sl_Input.at( j ).section( "\t", 1, 1 );
    }

    g_Reference_ptr[0].ReferenceID  = n;

    setNormalCursor();
    setStatusBar( tr( "Ready" ), 2 );

// **********************************************************************************************

    return( _NOERROR_ );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

/*! @brief Liest alle Datensatz IDs aus einer Datei
*
*   @return Fehlercode, 0 = kein Fehler, -30 = keine Datensatz ID Datenbank gefunden
*/

int MainWindow::readBsrnDatasetIDs()
{
    int		i					= 0;

    QString InputStr			= "";
    QString IDsFilename			= "";

// ***********************************************************************************************************************

    IDsFilename = getDataLocation()  + "/" + "BSRN_Dataset_IDs.txt";

// ***********************************************************************************************************************

    setWaitCursor();
    setStatusBar( tr( "Reading BSRN dataset IDs database - please wait" ) );

// ***********************************************************************************************************************

    downloadFile( findCurl(), QLatin1String( "https://store.pangaea.de/config/bsrn/BSRN_Dataset_IDs.txt" ), IDsFilename );

// ***********************************************************************************************************************

    QFile fIDs( IDsFilename );

// ***********************************************************************************************************************

    if ( ( fIDs.open( QIODevice::ReadOnly ) == false ) || ( fIDs.size() == 0 ) )
    {
        doSetOverwriteDatasetFlag();

        setNormalCursor();
        setStatusBar( tr( "Download of BSRN dataset IDs database fails" ), 2 );

        return( -61 ); // Download of BSRN dataset IDs database fails. Please check your connection to the internet and refresh the database again.
    }

// ***********************************************************************************************************************

    QTextStream tinIDs( &fIDs );

    InputStr = tinIDs.readLine(); // Date
    InputStr = tinIDs.readLine(); // Header

    i = 0;

    while ( ( tinIDs.atEnd() == false ) && ( i < MAX_NUM_OF_DATASETS ) )
    {
        ++i;

        InputStr = tinIDs.readLine();

        g_Dataset_ptr[i].ExportFilename = InputStr.section( "\t", 0, 0 );
        g_Dataset_ptr[i].DatasetID      = InputStr.section( "\t", 1, 1 ).toInt();
    }

    fIDs.close();

    setNormalCursor();
    setStatusBar( tr( "Ready" ), 2 );

// **********************************************************************************************

    if ( i == MAX_NUM_OF_DATASETS )
        return( -71 ); // Maximum number of datasets was reached. Please contact Roland Koppe (roland.koppe@awi.de)

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

int MainWindow::writeDefaultIDsBSRN( const QString &s_Filename )
{
    QFile fout( QDir::toNativeSeparators( s_Filename ) );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
        return( -20 );

//-----------------------------------------------------------------------------------------------------------------------

    QTextStream tout( &fout );

    tout << "[Version]" << endl;
    tout << PrefDate << "\t" << "of https://store.pangaea.de/config/bsrn/BSRN_IDs.txt" << endl;

    tout << "[Station]" << endl;
    tout << "BSRN Station ID\tPANGAEA Event label\tFull name\tPANGAEA Institute ID" << endl;
    tout << "1\tASP\tAlice Springs\t2894" << endl;
    tout << "2\tDAR\tDarwin\t2888" << endl;
    tout << "3\tFLO\tFlorianopolis\t2889" << endl;
    tout << "4\tSAP\tSapporo\t1671" << endl;
    tout << "5\tREG\tRegina\t2895" << endl;
    tout << "6\tFUA\tFukuoka\t1671" << endl;
    tout << "7\tISH\tIshigakijima\t1671" << endl;
    tout << "8\tMNM\tMinamitorishima\t1671" << endl;
    tout << "9\tTOR\tToravere\t2892" << endl;
    tout << "10\tCAR\tCarpentras\t2879" << endl;
    tout << "11\tNYA\tNy-&Aring%lesund\t33" << endl;
    tout << "12\tLIN\tLindenberg\t2885" << endl;
    tout << "13\tGVN\tNeumayer Station\t32" << endl;
    tout << "14\tBUD\tBudapest\t3032" << endl;
    tout << "16\tTAT\tTateno\t2877" << endl;
    tout << "17\tSYO\tSyowa\t1673" << endl;
    tout << "18\tALE\tAlert\t2895" << endl;
    tout << "19\tEUR\tEureka\t2895" << endl;
    tout << "21\tPAY\tPayerne\t2878" << endl;
    tout << "22\tBAR\tBarrow\t1809" << endl;
    tout << "23\tBOU\tBoulder\t2887" << endl;
    tout << "24\tBER\tBermuda\t1809" << endl;
    tout << "25\tKWA\tKwajalein\t1809" << endl;
    tout << "26\tSPO\tSouth Pole\t1809" << endl;
    tout << "27\tE13\tS. Great Plains\t2876" << endl;
    tout << "28\tBIL\tBillings\t2876" << endl;
    tout << "29\tMAN\tMomote\t2888" << endl;
    tout << "30\tNAU\tNauru Island\t2888" << endl;
    tout << "31\tFPE\tFort Peck\t2887" << endl;
    tout << "32\tBON\tBondville\t2887" << endl;
    tout << "33\tGCR\tGoodwin Creek\t2887" << endl;
    tout << "34\tBOS\tBoulder, SURFRAD\t2887" << endl;
    tout << "35\tDRA\tDesert Rock\t2887" << endl;
    tout << "36\tPSU\tRock Springs\t2887" << endl;
    tout << "37\tSXF\tSioux Falls\t2887" << endl;
    tout << "38\tILO\tIlorin\t2880" << endl;
    tout << "39\tCLH\tChesapeake Light\t2891" << endl;
    tout << "40\tDAA\tDe Aar\t2897" << endl;
    tout << "41\tSOV\tSolar Village\t2882" << endl;
    tout << "42\tTAM\tTamanrasset\t2893" << endl;
    tout << "43\tSBO\tSede Boqer\t2881" << endl;
    tout << "44\tXIA\tXianghe\t2875" << endl;
    tout << "45\tCNR\tCener\t3047" << endl;
    //tout << "46\tZVE\tZvenigrod\t2998" << endl;
    tout << "47\tCOC\tCocos Island\t2894" << endl;
    tout << "49\tLRC\tLangley Research Center\t2891" << endl;
    tout << "50\tCAM\tCamborne\t2886" << endl;
    tout << "51\tLER\tLerwick\t2886" << endl;
    tout << "53\tCAB\tCabauw\t1057" << endl;
    tout << "60\tLAU\tLauder\t2894" << endl;
    tout << "61\tIZA\tIzana\t2965" << endl;
    tout << "63\tPAL\tPalaiseau Cedex\t2884" << endl;
    tout << "65\tDWN\tDarwin Met Office\t2894" << endl;
    tout << "70\tSMS\tS&atilde%o Martinho da Serra\t2901" << endl;
    tout << "71\tBRB\tBrazilia\t2901" << endl;
    tout << "72\tPTR\tPetrolina\t2901" << endl;
    tout << "73\tRLM\tRolim de Moura\t2901" << endl;
    tout << "74\tDOM\tConcordia Station\t2963" << endl;
    tout << "76\tPSA\tPlataforma Solar de Almeria\t20296" << endl;
    tout << "77\tENA\tEastern North Atlantic\t99999" << endl;

    tout << "85\tCYL\tCyI PROTEAS\t2999" << endl;
    tout << "86\tABS\tAbashiri\t2999" << endl;
    tout << "87\tNPT\tNakhon Pathom\t2999" << endl;
    tout << "88\tINO\tMagurele\t2999" << endl;
    tout << "89\tTNB\tTerra Nova Bay\t2999" << endl;

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

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::OpenDataDescriptionHeader()
{
    QString s_OutputStr = "";

    s_OutputStr.append( tr( "// METAHEADER - BSRN data import at " ) );
    s_OutputStr.append( QDateTime::currentDateTime().toString( "yyyy-MM-ddThh:mm" ) );
    s_OutputStr.append( eol );
    s_OutputStr.append( "{" );
    s_OutputStr.append( eol );

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::CloseDataDescriptionHeader()
{
    QString s_OutputStr = "";

    s_OutputStr.append( "}" );
    s_OutputStr.append( eol );
    s_OutputStr.append( "// METAHEADER END" );
    s_OutputStr.append( eol );
//  s_OutputStr.append( eol ); // am 2016-05-19 eingefügt; am 2016-06-23 wieder raus.

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ParentID( const QString &s_ParentID )
{
    QString s_OutputStr = "";

    if ( s_ParentID.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"ParentID\": " );
        s_OutputStr.append( s_ParentID);
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::DataSetID( const QString &s_DatasetID )
{
    QString s_OutputStr = "";

    if ( s_DatasetID.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"DataSetID\": " );
        s_OutputStr.append( s_DatasetID );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::AuthorIDs( const QString &s_AuthorIDs )
{
    QString s_OutputStr = "";

    if ( s_AuthorIDs.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"AuthorIDs\": " );
        s_OutputStr.append( "[ "+ s_AuthorIDs + " ]" );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::SourceID( const QString &s_SourceID )
{
    QString s_OutputStr = "";

    if ( s_SourceID.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"SourceID\": " );
        s_OutputStr.append( s_SourceID );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::DatasetTitle( const QString &s_Text, const QString &s_StationName, const QDateTime dt )
{
    QString s_OutputStr = "  ";
    QString sd_Text     = s_Text;

    if ( ( s_Text.isEmpty() == false ) && ( s_StationName.isEmpty() == false ) )
    {
        sd_Text.replace( "\"", "\\\"" );
        sd_Text.replace( "\\\\", "\\" );

        s_OutputStr.append( "\"Title\": \"" );
        s_OutputStr.append( sd_Text + " " );

        if ( s_StationName.endsWith( "Station" ) == true )
            s_OutputStr.append( s_StationName);
        else
            s_OutputStr.append( tr( "station " ) + s_StationName);

        s_OutputStr.append( " (" + dt.toString( "yyyy-MM" ) + ")\"," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::Reference( const QString &s_ReferenceID, const int i_RelationTypeID, const QString &s_ReferenceType, const QString &s_EventLabel )
{
    QString s_OutputStr = "";

    if ( s_ReferenceID.isEmpty() == false )
    {
        s_OutputStr = "    ";

        if ( s_ReferenceID == "999999" )
            s_OutputStr.append( "{ \"ID\": @" + s_ReferenceType + "@" + s_EventLabel + "@" + ", \"RelationTypeID\": " + num2str( i_RelationTypeID ) + " }" );
        else
            s_OutputStr.append( "{ \"ID\": " + s_ReferenceID + ", \"RelationTypeID\": " + num2str( i_RelationTypeID ) + " }" );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ExportFilename( const QString &s_EventLabel, const QString &s_Text, const QDateTime dt )
{
    QString s_OutputStr = "";

    if ( ( s_EventLabel.isEmpty() == false ) && ( s_Text.isEmpty() == false ) )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"ExportFilename\": \"" );
        s_OutputStr.append( s_EventLabel + "_" );
        s_OutputStr.append( s_Text + "_" );
        s_OutputStr.append( dt.toString( "yyyy-MM" ) + "\"," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::EventLabel( const QString &s_EventLabel )
{
    QString s_OutputStr = "";

    if ( s_EventLabel.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"EventLabel\": \"" );
        s_OutputStr.append( s_EventLabel );
        s_OutputStr.append( "\"," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::Parameter( const QString &s_ParameterID, const QString &s_PIID, const QString &s_MethodID, const QString &s_Format, const QString &s_Comment )
{
    QString s_OutputStr = "";

    if ( s_ParameterID.isEmpty() == false )
    {
        s_OutputStr = "    ";
        s_OutputStr.append( "{ \"ID\": " + s_ParameterID + ", " );
        s_OutputStr.append( "\"PI_ID\": "  + s_PIID + ", " );
        s_OutputStr.append( "\"MethodID\": "  + s_MethodID );

        if ( s_Format.isEmpty() == false )
            s_OutputStr.append( ", \"Format\": \""  + s_Format + "\"" );

        if ( s_Comment.isEmpty() == false )
            s_OutputStr.append( ", \"Comment\": \""  + s_Comment + "\"" );

        s_OutputStr.append( " },");
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::Parameter( const QStringList &sl_Parameter )
{
    int     NumOfParameters = sl_Parameter.count() - 1;

    QString s_OutputStr     = "";

// **********************************************************************************************

    if ( NumOfParameters + 1 > 1 )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"ParameterIDs\": [ " );
        s_OutputStr.append( eol );
        s_OutputStr.append( sl_Parameter.first() );

        for ( int i=1; i<NumOfParameters; i++ )
            s_OutputStr.append( sl_Parameter.at( i ) );

        s_OutputStr.append( sl_Parameter.last().section( "},", 0, 0 ) );
        s_OutputStr.append( "} ],");
        s_OutputStr.append( eol );
    }
    else
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"ParameterIDs\": [ " );
        s_OutputStr.append( sl_Parameter.first().section( "    ", 1, 1 ).section( "},", 0, 0 ) );
        s_OutputStr.append( "} ],");
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::DatasetComment( const QString &s_DatasetComment )
{
    QString s_OutputStr = "";
    QString sd_DatasetComment = s_DatasetComment;

    if ( s_DatasetComment.isEmpty() == false )
    {
        sd_DatasetComment.replace( "\"", "\\\"" );
        sd_DatasetComment.replace( "\\\\", "\\" );

        s_OutputStr = "  ";
        s_OutputStr.append( "\"DataSetComment\": \"" );
        s_OutputStr.append( sd_DatasetComment );
        s_OutputStr.append( "\"," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ProjectIDs( const QString &s_ProjectIDs )
{
    QString s_OutputStr = "";

    if ( s_ProjectIDs.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"ProjectIDs\": " );
        s_OutputStr.append( "[ "+ s_ProjectIDs + " ]" );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::TopologicTypeID( const QString &s_TopologicTypeID )
{
    QString s_OutputStr = "";

    if ( s_TopologicTypeID.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"TopologicTypeID\": " );
        s_OutputStr.append( s_TopologicTypeID );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::StatusID( const QString &s_StatusID )
{
    QString s_OutputStr = "";

    if ( s_StatusID.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"StatusID\": " );
        s_OutputStr.append( s_StatusID );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::UserIDs( const QString &s_UserIDs )
{
    QString s_OutputStr = "";

    if ( s_UserIDs.isEmpty() == false )
    {
        s_OutputStr = "  ";
        s_OutputStr.append( "\"UserIDs\": " );
        s_OutputStr.append( "[ "+ s_UserIDs + " ]" );
        s_OutputStr.append( "," );
        s_OutputStr.append( eol );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::LoginID( const QString &s_LoginID )
{
    QString s_OutputStr = "  ";

    s_OutputStr.append( "\"LoginID\": " );
    s_OutputStr.append( s_LoginID ); // no comma at the end!
    s_OutputStr.append( eol );

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

QString MainWindow::ReferenceOtherVersion( const QString &s_EventLabel, structReference *Reference_ptr, const QDateTime dt )
{
    QString s_OutputStr = "  ";
    QString s_URI       = "ftp://ftp.bsrn.awi.de/" + s_EventLabel.toLower() + "/" + s_EventLabel.toLower() + dt.toString( "MMyy" ) + ".dat.gz";

// **********************************************************************************************

    if ( Reference_ptr[0].ReferenceID > 0 )
    {
        for ( int i=1; i <= Reference_ptr[0].ReferenceID; i++ )
        {
            if ( s_URI == Reference_ptr[i].ReferenceURI )
            {
                s_URI = QString( "%1" ).arg( Reference_ptr[i].ReferenceID );
                break;
            }
        }
    }

    s_OutputStr.append( "\"ReferenceIDs\": [" );
    s_OutputStr.append( eol );
    s_OutputStr.append( "    { \"ID\": " + s_URI + ", \"RelationTypeID\": 13 } ]," );
    s_OutputStr.append( eol );

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-06-06

QString MainWindow::writeGeocodeHeader( const bool b_Import, int *P )
{
    QString s_OutputStr = "";

    if ( b_Import == true )
    {
        s_OutputStr.append( "1599" );   // Date/Time
        s_OutputStr.append( "\t" );     // tab
        s_OutputStr.append( "56349" );  //  Height above ground = 2 m
    }
    else
    {
        if ( P[0] > 0 )
            s_OutputStr.append( "Station" );

        if ( P[1] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( "Date/Time" );
        }

        if ( P[2] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( "Latitude" );
        }

        if ( P[3] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( "Longitude" );
        }

        if ( P[4] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( "Height above ground [m]" );
        }

        if ( s_OutputStr.isEmpty() == true )
            s_OutputStr.append( "Date/Time" );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-06-06

QString MainWindow::buildGeocodeEntries( const bool b_Import, int *P, const QDateTime dt, const QString s_EventLabel, const float f_Latitude, const float f_Longitude )
{
    QString s_OutputStr = "";

    if ( b_Import == true )
    {
        s_OutputStr.append( dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" );
        s_OutputStr.append( tr( "2" ) );
    }
    else
    {
        if ( P[0] > 0 )
            s_OutputStr.append( s_EventLabel );

        if ( P[1] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( dt.toString( "yyyy-MM-ddThh:mm" ) );
        }

        if ( P[2] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( num2str( f_Latitude ) );
        }

        if ( P[3] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( num2str( f_Longitude ) );
        }

        if ( P[4] > 0 )
        {
            if ( s_OutputStr.isEmpty() == false )
                s_OutputStr.append( "\t" );

            s_OutputStr.append( tr( "2" ) );
        }

        if ( s_OutputStr.isEmpty() == true )
            s_OutputStr.append( dt.toString( "yyyy-MM-ddThh:mm" ) );
    }

    return( s_OutputStr );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-06-07

int MainWindow::checkSelectedParameter( const int offset, int *P, int *PoM )
{
    int P_sum = 0;

    for ( int i=offset+1; i<MAX_NUM_OF_PARAMETER; i++ )
        P_sum += P[i] + PoM[i];

    if ( P_sum < 1 )
        return( -51 );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-06-07

int MainWindow::checkSelectedParameter( const int offset, int *P )
{
    int P_sum = 0;

    for ( int i=offset+1; i<MAX_NUM_OF_PARAMETER; i++ )
        P_sum += P[i];

    if ( P_sum < 1 )
        return( -51 );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-07-21

bool MainWindow::removeEmptyFile( const QString &s_FilenameIn, const QString &s_FilenameOut, const int i_minFilesize )
{
    QFileInfo fin( s_FilenameIn );
    QFileInfo fout( s_FilenameOut );

    if ( ( fout.exists() == true ) && ( fout.isFile() == true ) && ( fout.size() < i_minFilesize ) )
    {
        QFile f( s_FilenameOut );
        f.remove();

        QString s_Message = tr( "Output file\n\n    ") + fout.fileName() + tr( "\n\nwas empty and has been deleted.\n\nSomething must be wrong. Please\ncheck the station-to-archive file\n\n    " ) + fin.fileName();
        QMessageBox::warning( this, getApplicationName( true ), s_Message  );

        return( true );
    }

    return( false );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-07-22

int MainWindow::setErr( const int err )
{
    switch ( err )
    {
    case _RECORDNOTFOUND_:
        return( _NOERROR_ );
        break;
    case _APPBREAK_:
        return( _NOERROR_ );
        break;
    }

    return( err );
}

