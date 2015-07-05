/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2008-01-20

/*! @brief Testet den LR 1000.
*
*   @param FilenameIn Dateiname der Inputdatei
*   @param P Pointer auf ein Array von Integern
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/

int MainWindow::SYNOPTest( const QString& s_FilenameIn, int *P, const int i_NumOfFiles )
{
    int				i_Format		= 0;

    QString			InputStr		= "";
    QString			SearchString1	= "*C1000";
    QString			SearchString2	= "*U1000";

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

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

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "SYNOP converter working (Testing)..." ), 100 );

    setStatusBarFileInProgress( s_FilenameIn, tr( " - Testing" ) );

// ***********************************************************************************************************************

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr	= tin.readLine();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            InputStr	= tin.readLine();
            ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

            if ( fi.baseName().startsWith( "fua" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "gvn" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "ish" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "lin" ) == true ) i_Format = 1;
            if ( fi.baseName().startsWith( "mnm" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "nya" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "pay" ) == true ) i_Format = 4;
            if ( fi.baseName().startsWith( "sap" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "sbo" ) == true ) i_Format = 1;
            if ( fi.baseName().startsWith( "son" ) == true ) i_Format = 5;
            if ( fi.baseName().startsWith( "syo" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "tam" ) == true ) i_Format = 3;
            if ( fi.baseName().startsWith( "tat" ) == true ) i_Format = 2;

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    switch ( i_Format )
                    {
                    case 1: // LIN
                        InputStr = InputStr.simplified();

                        if ( InputStr.count( " " ) >= 2 )
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 )   != "/" ) P[1] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 1 ) != "/" ) P[2] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 1 ) != "/" ) P[3] = 1;
                        }

                        if ( InputStr.count( " " ) >= 3 )
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 1 ) != "/" ) P[4] = 1;

                        if ( InputStr.count( " " ) >= 4 )
                            if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ) != "/" ) P[5] = 1;

                        if ( InputStr.count( " " ) >= 5 )
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" ) P[6] = 1;

                        if ( InputStr.count( " " ) >= 6 )
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[7] = 1;

                        if ( InputStr.count( " " ) >= 7 )
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" ) P[8] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" ) P[9] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" ) P[10] = 1;
                        }

                        if ( InputStr.count( " " ) >= 8 )
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" ) P[11] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" ) P[12] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" ) P[13] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" ) P[14] = 1;
                        }

                        if ( InputStr.count( " ") >= 9 )
                        {
                            if ( InputStr.section( " ", 9, 9 ) != "333" )
                                b_Stop = true;
                        }

                        if ( InputStr.count( " " ) >= 10 )
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" ) P[15] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" ) P[16] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" ) P[17] = 1;
                        }

                        if ( InputStr.count( " " ) >= 11 )
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" ) P[18] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" ) P[19] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[20] = 1;
                        }

                        if ( InputStr.count( " " ) == 12 )
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" ) P[21] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" ) P[22] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[23] = 1;
                        }

                        break;

                    case 2: // GVN and NYA
                        if ( InputStr.mid( 9, 1 )  != "/" ) P[1] = 1;
                        if ( InputStr.mid( 16, 1 ) != "/" ) P[2] = 1;
                        if ( InputStr.mid( 21, 1 ) != "/" ) P[3] = 1;
                        if ( InputStr.mid( 27, 1 ) != "/" ) P[4] = 1;
                        if ( InputStr.mid( 31, 1 ) != "/" ) P[5] = 1;
                        if ( InputStr.mid( 35, 1 ) != "/" ) P[6] = 1;
                        if ( InputStr.mid( 37, 1 ) != "/" ) P[7] = 1;
                        if ( InputStr.mid( 40, 1 ) != "/" ) P[8] = 1;
                        if ( InputStr.mid( 42, 1 ) != "/" ) P[9] = 1;
                        if ( InputStr.mid( 44, 1 ) != "/" ) P[10] = 1;
                        if ( InputStr.mid( 46, 1 ) != "/" ) P[11] = 1;
                        if ( InputStr.mid( 48, 1 ) != "/" ) P[12] = 1;
                        if ( InputStr.mid( 50, 1 ) != "/" ) P[13] = 1;
                        if ( InputStr.mid( 52, 1 ) != "/" ) P[14] = 1;
                        if ( InputStr.mid( 53, 1 ) != "/" ) P[15] = 1;
                        if ( InputStr.mid( 55, 1 ) != "/" ) P[16] = 1;

                        break;

                    case 3: // TAM
                        InputStr = InputStr.simplified();

                        if ( InputStr.count( " " ) >= 2 )
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 2, 1 ) != "/" ) P[1] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" ) P[2] = 1;
                        }

                        if ( InputStr.count( " " ) >= 3 )
                        {
                            if ( InputStr.section( " ", 3, 3 ).left( 1 )   != "/" ) P[3] = 1;
                            if ( InputStr.section( " ", 3, 3 ).mid( 1, 1 ) != "/" ) P[4] = 1;
                            if ( InputStr.section( " ", 3, 3 ).mid( 3, 1 ) != "/" ) P[5] = 1;
                        }

                        if ( InputStr.count( " " ) >= 4 )
                            if ( InputStr.section( " ", 4, 4 ).mid( 2, 1 ) != "/" ) P[6] = 1;

                        if ( InputStr.count( " " ) >= 5 )
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 1 ) != "/" ) P[7] = 1;

                        if ( InputStr.count( " " ) >= 6 )
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[8] = 1;

                        if ( InputStr.count( " " ) >= 7 )
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 4 ) != "////" ) P[9] = 1;

                        P[9] = 0;  // Geopotential height will be not stored in BSRN datasets, GKL 2013-03-11

                        if ( InputStr.count( " " ) >= 8 )
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 2 ) != "//" ) P[10] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )  P[11] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )  P[12] = 1;
                        }

                        if ( InputStr.count( " " ) >= 9 )
                        {
                            if ( InputStr.section( " ", 9, 9 ).mid( 1, 1 ) != "/" ) P[13] = 1;
                            if ( InputStr.section( " ", 9, 9 ).mid( 2, 1 ) != "/" ) P[14] = 1;
                            if ( InputStr.section( " ", 9, 9 ).mid( 3, 1 ) != "/" ) P[15] = 1;
                            if ( InputStr.section( " ", 9, 9 ).mid( 4, 1 ) != "/" ) P[16] = 1;
                        }

                        if ( InputStr.count( " ") >= 10 )
                        {
                            if ( InputStr.section( " ", 10, 10 ) != "333" )
                                b_Stop = true;
                        }

                        if ( InputStr.count( " " ) >= 11 )
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" ) P[17] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" ) P[18] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[19] = 1;
                        }

                        if ( InputStr.count( " " ) >= 12 )
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" ) P[20] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" ) P[21] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[22] = 1;
                        }

                        if ( InputStr.count( " " ) == 13 )
                        {
                            if ( InputStr.section( " ", 13, 13 ).mid( 1, 1 ) != "/" ) P[23] = 1;
                            if ( InputStr.section( " ", 13, 13 ).mid( 2, 1 ) != "/" ) P[24] = 1;
                            if ( InputStr.section( " ", 13, 13 ).mid( 3, 2 ) != "//" ) P[25] = 1;
                        }
                        break;

                    case 4: // PAY
                        InputStr = InputStr.simplified();

                        if ( InputStr.count( " " ) >= 1 )
                        {
                            if ( InputStr.section( " ", 1, 1 ).mid( 2, 1 ) != "/" ) P[1] = 1;
                            if ( InputStr.section( " ", 1, 1 ).mid( 3, 2 ) != "//" ) P[2] = 1;
                        }

                        if ( InputStr.count( " " ) >= 2 )
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 )   != "/" ) P[3] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 1 ) != "/" ) P[4] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 1 ) != "/" ) P[5] = 1;
                        }

                        if ( InputStr.count( " " ) >= 3 )
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 1 ) != "/" ) P[6] = 1;

                        if ( InputStr.count( " " ) >= 4 )
                            if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ) != "/" ) P[7] = 1;

                        if ( InputStr.count( " " ) >= 5 )
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" ) P[8] = 1;

                        if ( InputStr.count( " " ) >= 6 )
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[9] = 1;

                        if ( InputStr.count( " " ) >= 7 )
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" ) P[10] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" ) P[11] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" ) P[12] = 1;
                        }

                        if ( InputStr.count( " " ) >= 8 )
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" ) P[13] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" ) P[14] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" ) P[15] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" ) P[16] = 1;
                        }

                        if ( InputStr.count( " ") >= 9 )
                        {
                            if ( InputStr.section( " ", 9, 9 ) != "333" )
                                b_Stop = true;
                        }

                        if ( InputStr.count( " " ) >= 10 )
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" ) P[17] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" ) P[18] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" ) P[19] = 1;
                        }

                        if ( InputStr.count( " " ) >= 11 )
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" ) P[20] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" ) P[21] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[22] = 1;
                        }

                        if ( InputStr.count( " " ) == 12 )
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" ) P[23] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" ) P[24] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[25] = 1;
                        }
                        break;

                    case 5: // SON
                        InputStr = InputStr.simplified();

                        if ( InputStr.count( " " ) >= 2 )
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 )   != "/" ) P[1] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 1 ) != "/" ) P[2] = 1;
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 1 ) != "/" ) P[3] = 1;
                        }

                        if ( InputStr.count( " " ) >= 3 )
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 1 ) != "/" ) P[4] = 1;

                        if ( InputStr.count( " " ) >= 4 )
                            if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ) != "/" ) P[5] = 1;

                        if ( InputStr.count( " " ) >= 5 )
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" ) P[6] = 1;

                        if ( InputStr.count( " " ) >= 6 )
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[7] = 1;

                        P[7] = 0;  // Geopotential height will be not stored in BSRN datasets, GKL 2013-03-11

                        if ( InputStr.count( " " ) >= 7 )
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" ) P[8] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" )  P[9] = 1;
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" )  P[10] = 1;
                        }

                        if ( InputStr.count( " " ) >= 8 )
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" ) P[11] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" ) P[12] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" ) P[13] = 1;
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" ) P[14] = 1;
                        }

                        if ( InputStr.count( " ") >= 9 )
                        {
                            if ( InputStr.section( " ", 9, 9 ) != "333" )
                                b_Stop = true;
                        }

                        if ( InputStr.count( " " ) >= 10 )
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" )  P[15] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" )  P[16] = 1;
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" ) P[17] = 1;
                        }

                        if ( InputStr.count( " " ) >= 11 )
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )  P[18] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )  P[19] = 1;
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[20] = 1;
                        }

                        if ( InputStr.count( " " ) == 12 )
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )  P[21] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )  P[22] = 1;
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[23] = 1;
                        }
                        break;

                    default:
                        b_Stop = true;
                        break;
                    }
                }
                else
                    b_Stop = true;
            }
        }
    }

// ***********************************************************************************************************************

    resetProgress( i_NumOfFiles );

    fin.close();

    return( i_Format );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

/*! @brief Konvertiert den LR 1000.
*
*   @param b_Import Erzeugt Import- oder Datendatei
*   @param s_FilenameIn Dateiname der Inputdatei
*   @param Staff_ptr Pointer auf Array aller Personen
*   @param Station_ptr Pointer auf Array aller Stationen
*   @param i_NumOfFiles Anzahl der Dateien
*
*   @return Fehlercode
*/
int MainWindow::SYNOPConverter( const bool b_Import, const QString& s_FilenameIn, structStaff *Staff_ptr, structStation *Station_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				i_Format		= 0;

    int				i_PIID			= 506;
    int				i_SourceID		= 17;
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

    QString			SearchString1	= "*C1000";
    QString			SearchString2	= "*U1000";

    QString			s_StationName	= "";
    QString			s_EventLabel	= "";
    QString         s_DatasetID     = "";

    QStringList     sl_Parameter;

    unsigned int	ui_length		= 1;
    unsigned int	ui_filesize		= 1;

    bool			b_Stop			= false;

    int				P[MAX_NUM_OF_PARAMETER+1];

// ***********************************************************************************************************************

    i_Format = SYNOPTest( s_FilenameIn, P, i_NumOfFiles );

    if ( i_Format == 0 )
        return( _NOERROR_ );

// ***********************************************************************************************************************

    QFileInfo fi( s_FilenameIn );

    QFile fin( s_FilenameIn );
    if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
        return( -10 );
    ui_filesize = fin.size();

    QTextStream tin( &fin );

// ***********************************************************************************************************************

    initProgress( i_NumOfFiles, s_FilenameIn, tr( "SYNOP converter working (Building)..." ), 100 );

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

            i_PIID = findPiId( InputStr.left( 38 ).simplified(), Staff_ptr );

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

    QString s_FilenameOut = s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_1000";

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
// LR1000 - build data description header

    b_Stop = false;

    if ( b_Import == true )
    {
        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetId( QString( "%1_SYNOP_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                s_DatasetID = DataSetID( num2str( i_DatasetID ) );
            else
                s_DatasetID = DataSetID( QString( "%1_SYNOP_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ) );
        }

        sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );

        switch ( i_Format )
        {
        case 1: // LIN
            if ( P[1]  == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Total cloud amount
            if ( P[2]  == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );    // Wind direction [deg]
            if ( P[3]  == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );     // Wind speed [m/sec]
            if ( P[4]  == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air [deg C]
            if ( P[5]  == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );  // Dew/frost point [deg C]
            if ( P[6]  == 1 ) sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );    // Station Pressure [hPa]
            if ( P[7]  == 1 ) sl_Parameter.append( Parameter( num2str( 2224 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );     // Pressure, atmospheric [hPa]
            if ( P[8]  == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present weather [code]
            if ( P[9]  == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather1 [code]
            if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather2 [code]
            if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low/middle cloud amount [code]
            if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
            if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
            if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
            if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 57649 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 1 [code]
            if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 57652 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 1 [code]
            if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 57655 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 1 [code]
            if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 57650 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 2 [code]
            if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 57653 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 2 [code]
            if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 57656 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 2 [code]
            if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 57651 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 3 [code]
            if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 57654 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 3 [code]
            if ( P[23] == 1 ) sl_Parameter.append( Parameter( num2str( 57657 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 3 [code]

            sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) ) );

            break;

        case 2: // GVN and NYA
            if ( P[1] == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );   // Temperature, air [deg C]
            if ( P[2] == 1 ) sl_Parameter.append( Parameter( num2str( 2224 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ), tr( "Station pressure reduced to sea level" ) ) );   // Pressure, atmospheric [hPa]
            if ( P[3] == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );   // Dew/frost point [deg C]
            if ( P[4] == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );     // Wind direction [deg]
            if ( P[5] == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );      // Wind speed [m/sec]
            if ( P[6] == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );       // Past weather1 [code]
            if ( P[7] == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );       // Present weather [code]
            if ( P[8] == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );       // Total cloud amount
            if ( P[9] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );       // Low/middle cloud amount [code]
            if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
            if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
            if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
            if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45259 ), num2str( i_PIID ), num2str( 5036 ), tr( "#0" ) ) );     // Cloud base height [code]
            if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45307 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present blowing snow [code]
            if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 45308 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past blowing snow [code]
            if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 45260 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // Horizontal visibility [code]

            break;

        case 3: // TAM
            if ( P[1]  == 1 ) sl_Parameter.append( Parameter( num2str( 45259 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Cloud base height
            if ( P[2]  == 1 ) sl_Parameter.append( Parameter( num2str( 45260 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Horizontal visibility
            if ( P[3]  == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Total cloud amount
            if ( P[4]  == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );    // Wind direction [deg]
            if ( P[5]  == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );     // Wind speed [m/sec]
            if ( P[6]  == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air [deg C]
            if ( P[7]  == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );  // Dew/frost point [deg C]
            if ( P[8]  == 1 ) sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );    // Station Pressure [hPa]
            if ( P[9]  == 1 ) sl_Parameter.append( Parameter( num2str( 123640 ), num2str( i_PIID ), num2str( 50 ), tr( "###0" ) ) );    // Geopotential height [m]
            if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present weather [code]
            if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather1 [code]
            if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather2 [code]
            if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low/middle cloud amount [code]
            if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
            if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
            if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
            if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 57649 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 1 [code]
            if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 57652 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 1 [code]
            if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 57655 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 1 [code]
            if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 57650 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 2 [code]
            if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 57653 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 2 [code]
            if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 57656 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 2 [code]
            if ( P[23] == 1 ) sl_Parameter.append( Parameter( num2str( 57651 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 3 [code]
            if ( P[24] == 1 ) sl_Parameter.append( Parameter( num2str( 57654 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 3 [code]
            if ( P[25] == 1 ) sl_Parameter.append( Parameter( num2str( 57657 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 3 [code]

            sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) ) );

            break;

        case 4: // PAY
            if ( P[1]  == 1 ) sl_Parameter.append( Parameter( num2str( 45259 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Cloud base height
            if ( P[2]  == 1 ) sl_Parameter.append( Parameter( num2str( 45260 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Horizontal visibility
            if ( P[3]  == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Total cloud amount
            if ( P[4]  == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );    // Wind direction [deg]
            if ( P[5]  == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );     // Wind speed [m/sec]
            if ( P[6]  == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air [deg C]
            if ( P[7]  == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );  // Dew/frost point [deg C]
            if ( P[8]  == 1 ) sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );    // Station Pressure [hPa]
            if ( P[9]  == 1 ) sl_Parameter.append( Parameter( num2str( 2224 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );     // Pressure, atmospheric [hPa]
            if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present weather [code]
            if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather1 [code]
            if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather2 [code]
            if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low/middle cloud amount [code]
            if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
            if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
            if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
            if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 57649 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 1 [code]
            if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 57652 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 1 [code]
            if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 57655 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 1 [code]
            if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 57650 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 2 [code]
            if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 57653 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 2 [code]
            if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 57656 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 2 [code]
            if ( P[23] == 1 ) sl_Parameter.append( Parameter( num2str( 57651 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 3 [code]
            if ( P[24] == 1 ) sl_Parameter.append( Parameter( num2str( 57654 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 3 [code]
            if ( P[25] == 1 ) sl_Parameter.append( Parameter( num2str( 57657 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 3 [code]

           sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) ) );

            break;

        case 5: // SON
            if ( P[1]  == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Total cloud amount
            if ( P[2]  == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );    // Wind direction [deg]
            if ( P[3]  == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );     // Wind speed [m/sec]
            if ( P[4]  == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air [deg C]
            if ( P[5]  == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );  // Dew/frost point [deg C]
            if ( P[6]  == 1 ) sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );    // Station Pressure [hPa]
            if ( P[7]  == 1 ) sl_Parameter.append( Parameter( num2str( 123640 ), num2str( i_PIID ), num2str( 50 ), tr( "###0" ) ) );    // Geopotential height [m]
            if ( P[8]  == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present weather [code]
            if ( P[9]  == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather1 [code]
            if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather2 [code]
            if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low/middle cloud amount [code]
            if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
            if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
            if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
            if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 57649 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 1 [code]
            if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 57652 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 1 [code]
            if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 57655 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 1 [code]
            if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 57650 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 2 [code]
            if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 57653 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 2 [code]
            if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 57656 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 2 [code]
            if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 57651 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ns 3 [code]
            if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 57654 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C 3 [code]
            if ( P[23] == 1 ) sl_Parameter.append( Parameter( num2str( 57657 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs 3 [code]

           sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) ) );

            break;
        }
    }

// ***********************************************************************************************************************
// write data description header

    if ( ( b_Import == true ) && ( sl_Parameter.count() > 1 ) )
    {
        tout << OpenDataDescriptionHeader();
        tout << s_DatasetID;
        tout << ReferenceOtherVersion( s_EventLabel, dt );
        tout << AuthorIDs( num2str( i_PIID ) );
        tout << SourceID( num2str( i_SourceID ) );
        tout << DatasetTitle( QString( "Meteorological synoptical observations from" ), s_StationName, dt );
        tout << ExportFilename( s_EventLabel, tr( "SYNOP" ), dt );
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

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr	= tin.readLine();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            switch ( i_Format )
            {
            case 1: // LIN
                if ( b_Import == true )
                {
                    tout << "1599"; // Date/Time
                    if ( P[1]  == 1 ) tout << "\t45267";
                    if ( P[2]  == 1 ) tout << "\t2221";
                    if ( P[3]  == 1 ) tout << "\t18906";
                    if ( P[4]  == 1 ) tout << "\t4610";
                    if ( P[5]  == 1 ) tout << "\t4611";
                    if ( P[6]  == 1 ) tout << "\t48823";
                    if ( P[7]  == 1 ) tout << "\t2224";
                    if ( P[8]  == 1 ) tout << "\t45261";
                    if ( P[9]  == 1 ) tout << "\t45262";
                    if ( P[10] == 1 ) tout << "\t45263";
                    if ( P[11] == 1 ) tout << "\t45268";
                    if ( P[12] == 1 ) tout << "\t45264";
                    if ( P[13] == 1 ) tout << "\t45265";
                    if ( P[14] == 1 ) tout << "\t45266";
                    if ( P[15] == 1 ) tout << "\t57649";
                    if ( P[16] == 1 ) tout << "\t57652";
                    if ( P[17] == 1 ) tout << "\t57655";
                    if ( P[18] == 1 ) tout << "\t57650";
                    if ( P[19] == 1 ) tout << "\t57653";
                    if ( P[20] == 1 ) tout << "\t57656";
                    if ( P[21] == 1 ) tout << "\t57651";
                    if ( P[22] == 1 ) tout << "\t57654";
                    if ( P[23] == 1 ) tout << "\t57657";
                    tout << "\t50007" << eol;
                }
                else
                {
                    tout << "Station\tDate/Time\tLatitude\tLongitude\tIIiii";
                    if ( P[1]  == 1 ) tout << "\tTotal cloud amount [code]";
                    if ( P[2]  == 1 ) tout << "\tWind direction [deg]";
                    if ( P[3]  == 1 ) tout << "\tWind speed [m/sec]";
                    if ( P[4]  == 1 ) tout << "\tTemperature, air [deg C]";
                    if ( P[5]  == 1 ) tout << "\tDew/frost point [deg C]";
                    if ( P[6]  == 1 ) tout << "\tStation Pressure [hPa]";
                    if ( P[7]  == 1 ) tout << "\tPressure, atmospheric [hPa]";
                    if ( P[8]  == 1 ) tout << "\tPresent weather [code]";
                    if ( P[9]  == 1 ) tout << "\tPast weather1 [code]";
                    if ( P[10] == 1 ) tout << "\tPast weather2 [code]";
                    if ( P[11] == 1 ) tout << "\tLow/middle cloud amount [code]";
                    if ( P[12] == 1 ) tout << "\tLow cloud [code]";
                    if ( P[13] == 1 ) tout << "\tMiddle cloud [code]";
                    if ( P[14] == 1 ) tout << "\tHigh cloud [code]";
                    if ( P[15] == 1 ) tout << "\tNs 1 [code]";
                    if ( P[16] == 1 ) tout << "\tC 1 [code]";
                    if ( P[17] == 1 ) tout << "\thshs 1 [code]";
                    if ( P[18] == 1 ) tout << "\tNs 2 [code]";
                    if ( P[19] == 1 ) tout << "\tC 2 [code]";
                    if ( P[20] == 1 ) tout << "\thshs 2 [code]";
                    if ( P[21] == 1 ) tout << "\tNs 3 [code]";
                    if ( P[22] == 1 ) tout << "\tC 3 [code]";
                    if ( P[23] == 1 ) tout << "\thshs 3 [code]";
                    tout << "\t" << tr( "YYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) << eol;
                }
                break;

            case 2: // GVN and NYA
                if ( b_Import == true )
                {
                    tout << "1599"; // Date/Time
                    if ( P[1]  == 1 ) tout << "\t4610";
                    if ( P[2]  == 1 ) tout << "\t2224";
                    if ( P[3]  == 1 ) tout << "\t4611";
                    if ( P[4]  == 1 ) tout << "\t2221";
                    if ( P[5]  == 1 ) tout << "\t18906";
                    if ( P[6]  == 1 ) tout << "\t45262";
                    if ( P[7]  == 1 ) tout << "\t45261";
                    if ( P[8]  == 1 ) tout << "\t45267";
                    if ( P[9]  == 1 ) tout << "\t45268";
                    if ( P[10] == 1 ) tout << "\t45264";
                    if ( P[11] == 1 ) tout << "\t45265";
                    if ( P[12] == 1 ) tout << "\t45266";
                    if ( P[13] == 1 ) tout << "\t45259";
                    if ( P[14] == 1 ) tout << "\t45307";
                    if ( P[15] == 1 ) tout << "\t45308";
                    if ( P[16] == 1 ) tout << "\t45260";
                    tout << eol;
                }
                else
                {
                    tout << "Station\tDate/Time\tLatitude\tLongitude";
                    if ( P[1]  == 1 ) tout << "\tTemperature, air [deg C]";
                    if ( P[2]  == 1 ) tout << "\tPressure, atmospheric [hPa]";
                    if ( P[3]  == 1 ) tout << "\tDew/frost point [deg C]";
                    if ( P[4]  == 1 ) tout << "\tWind direction [deg]";
                    if ( P[5]  == 1 ) tout << "\tWind speed [m/sec]";
                    if ( P[6]  == 1 ) tout << "\tPast weather1 [code]";
                    if ( P[7]  == 1 ) tout << "\tPresent weather [code]";
                    if ( P[8]  == 1 ) tout << "\tTotal cloud amount [code]";
                    if ( P[9]  == 1 ) tout << "\tLow/middle cloud amount [code]";
                    if ( P[10] == 1 ) tout << "\tLow cloud [code]";
                    if ( P[11] == 1 ) tout << "\tMiddle cloud [code]";
                    if ( P[12] == 1 ) tout << "\tHigh cloud [code]";
                    if ( P[13] == 1 ) tout << "\tCloud base height [code]";
                    if ( P[14] == 1 ) tout << "\tPresent blowing snow [code]";
                    if ( P[15] == 1 ) tout << "\tPast blowing snow [code]";
                    if ( P[16] == 1 ) tout << "\tHorizontal visibility [code]";
                    tout << eol;
                }
                break;

            case 3: // TAM
                if ( b_Import == true )
                {
                    tout << "1599"; // Date/Time
                    if ( P[1]  == 1 ) tout << "\t45259";
                    if ( P[2]  == 1 ) tout << "\t45260";
                    if ( P[3]  == 1 ) tout << "\t45267";
                    if ( P[4]  == 1 ) tout << "\t2221";
                    if ( P[5]  == 1 ) tout << "\t18906";
                    if ( P[6]  == 1 ) tout << "\t4610";
                    if ( P[7]  == 1 ) tout << "\t4611";
                    if ( P[8]  == 1 ) tout << "\t48823";
                    if ( P[9]  == 1 ) tout << "\t123640";
                    if ( P[10] == 1 ) tout << "\t45261";
                    if ( P[11] == 1 ) tout << "\t45262";
                    if ( P[12] == 1 ) tout << "\t45263";
                    if ( P[13] == 1 ) tout << "\t45268";
                    if ( P[14] == 1 ) tout << "\t45264";
                    if ( P[15] == 1 ) tout << "\t45265";
                    if ( P[16] == 1 ) tout << "\t45266";
                    if ( P[17] == 1 ) tout << "\t57649";
                    if ( P[18] == 1 ) tout << "\t57652";
                    if ( P[19] == 1 ) tout << "\t57655";
                    if ( P[20] == 1 ) tout << "\t57650";
                    if ( P[21] == 1 ) tout << "\t57653";
                    if ( P[22] == 1 ) tout << "\t57656";
                    if ( P[23] == 1 ) tout << "\t57651";
                    if ( P[24] == 1 ) tout << "\t57654";
                    if ( P[25] == 1 ) tout << "\t57657";
                    tout << "\t50007" << eol;
                }
                else
                {
                    tout << "Station\tDate/Time\tLatitude\tLongitude\tIIiii";
                    if ( P[1]  == 1 ) tout << "\tCloud base height [code]";
                    if ( P[2]  == 1 ) tout << "\tHorizontal visibility [code]";
                    if ( P[3]  == 1 ) tout << "\tTotal cloud amount [code]";
                    if ( P[4]  == 1 ) tout << "\tWind direction [deg]";
                    if ( P[5]  == 1 ) tout << "\tWind speed [m/sec]";
                    if ( P[6]  == 1 ) tout << "\tTemperature, air [deg C]";
                    if ( P[7]  == 1 ) tout << "\tDew/frost point [deg C]";
                    if ( P[8]  == 1 ) tout << "\tStation Pressure [hPa]";
                    if ( P[9]  == 1 ) tout << "\tGeopotential height [m]";
                    if ( P[10] == 1 ) tout << "\tPresent weather [code]";
                    if ( P[11]  == 1 ) tout << "\tPast weather1 [code]";
                    if ( P[12] == 1 ) tout << "\tPast weather2 [code]";
                    if ( P[13] == 1 ) tout << "\tLow/middle cloud amount [code]";
                    if ( P[14] == 1 ) tout << "\tLow cloud [code]";
                    if ( P[15] == 1 ) tout << "\tMiddle cloud [code]";
                    if ( P[16] == 1 ) tout << "\tHigh cloud [code]";
                    if ( P[17] == 1 ) tout << "\tNs 1 [code]";
                    if ( P[18] == 1 ) tout << "\tC 1 [code]";
                    if ( P[19] == 1 ) tout << "\thshs 1 [code]";
                    if ( P[20] == 1 ) tout << "\tNs 2 [code]";
                    if ( P[21] == 1 ) tout << "\tC 2 [code]";
                    if ( P[22] == 1 ) tout << "\thshs 2 [code]";
                    if ( P[23] == 1 ) tout << "\tNs 3 [code]";
                    if ( P[24] == 1 ) tout << "\tC 3 [code]";
                    if ( P[25] == 1 ) tout << "\thshs 3 [code]";
                    tout << "\t" << tr( "YYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) << eol;
                }
                break;

            case 4: // PAY
                if ( b_Import == true )
                {
                    tout << "1599"; // Date/Time
                    if ( P[1]  == 1 ) tout << "\t45259";
                    if ( P[2]  == 1 ) tout << "\t45260";
                    if ( P[3]  == 1 ) tout << "\t45267";
                    if ( P[4]  == 1 ) tout << "\t2221";
                    if ( P[5]  == 1 ) tout << "\t18906";
                    if ( P[6]  == 1 ) tout << "\t4610";
                    if ( P[7]  == 1 ) tout << "\t4611";
                    if ( P[8]  == 1 ) tout << "\t48823";
                    if ( P[9]  == 1 ) tout << "\t2224";
                    if ( P[10] == 1 ) tout << "\t45261";
                    if ( P[11] == 1 ) tout << "\t45262";
                    if ( P[12] == 1 ) tout << "\t45263";
                    if ( P[13] == 1 ) tout << "\t45268";
                    if ( P[14] == 1 ) tout << "\t45264";
                    if ( P[15] == 1 ) tout << "\t45265";
                    if ( P[16] == 1 ) tout << "\t45266";
                    if ( P[17] == 1 ) tout << "\t57649";
                    if ( P[18] == 1 ) tout << "\t57652";
                    if ( P[19] == 1 ) tout << "\t57655";
                    if ( P[20] == 1 ) tout << "\t57650";
                    if ( P[21] == 1 ) tout << "\t57653";
                    if ( P[22] == 1 ) tout << "\t57656";
                    if ( P[23] == 1 ) tout << "\t57651";
                    if ( P[24] == 1 ) tout << "\t57654";
                    if ( P[25] == 1 ) tout << "\t57657";
                    tout << "\t50007" << eol;
                }
                else
                {
                    tout << "Station\tDate/Time\tLatitude\tLongitude\tIIiii";
                    if ( P[1]  == 1 ) tout << "\tCloud base height [code]";
                    if ( P[2]  == 1 ) tout << "\tHorizontal visibility [code]";
                    if ( P[3]  == 1 ) tout << "\tTotal cloud amount [code]";
                    if ( P[4]  == 1 ) tout << "\tWind direction [deg]";
                    if ( P[5]  == 1 ) tout << "\tWind speed [m/sec]";
                    if ( P[6]  == 1 ) tout << "\tTemperature, air [deg C]";
                    if ( P[7]  == 1 ) tout << "\tDew/frost point [deg C]";
                    if ( P[8]  == 1 ) tout << "\tStation Pressure [hPa]";
                    if ( P[9]  == 1 ) tout << "\tPressure, atmospheric [hPa]";
                    if ( P[10] == 1 ) tout << "\tPresent weather [code]";
                    if ( P[11]  == 1 ) tout << "\tPast weather1 [code]";
                    if ( P[12] == 1 ) tout << "\tPast weather2 [code]";
                    if ( P[13] == 1 ) tout << "\tLow/middle cloud amount [code]";
                    if ( P[14] == 1 ) tout << "\tLow cloud [code]";
                    if ( P[15] == 1 ) tout << "\tMiddle cloud [code]";
                    if ( P[16] == 1 ) tout << "\tHigh cloud [code]";
                    if ( P[17] == 1 ) tout << "\tNs 1 [code]";
                    if ( P[18] == 1 ) tout << "\tC 1 [code]";
                    if ( P[19] == 1 ) tout << "\thshs 1 [code]";
                    if ( P[20] == 1 ) tout << "\tNs 2 [code]";
                    if ( P[21] == 1 ) tout << "\tC 2 [code]";
                    if ( P[22] == 1 ) tout << "\thshs 2 [code]";
                    if ( P[23] == 1 ) tout << "\tNs 3 [code]";
                    if ( P[24] == 1 ) tout << "\tC 3 [code]";
                    if ( P[25] == 1 ) tout << "\thshs 3 [code]";
                    tout << "\t" << tr( "YYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) << eol;
                }
                break;

            case 5: // SON
                if ( b_Import == true )
                {
                    tout << "1599"; // Date/Time
                    if ( P[1]  == 1 ) tout << "\t45267";
                    if ( P[2]  == 1 ) tout << "\t2221";
                    if ( P[3]  == 1 ) tout << "\t18906";
                    if ( P[4]  == 1 ) tout << "\t4610";
                    if ( P[5]  == 1 ) tout << "\t4611";
                    if ( P[6]  == 1 ) tout << "\t48823";
                    if ( P[7]  == 1 ) tout << "\t123640";
                    if ( P[8]  == 1 ) tout << "\t45261";
                    if ( P[9]  == 1 ) tout << "\t45262";
                    if ( P[10] == 1 ) tout << "\t45263";
                    if ( P[11] == 1 ) tout << "\t45268";
                    if ( P[12] == 1 ) tout << "\t45264";
                    if ( P[13] == 1 ) tout << "\t45265";
                    if ( P[14] == 1 ) tout << "\t45266";
                    if ( P[15] == 1 ) tout << "\t57649";
                    if ( P[16] == 1 ) tout << "\t57652";
                    if ( P[17] == 1 ) tout << "\t57655";
                    if ( P[18] == 1 ) tout << "\t57650";
                    if ( P[19] == 1 ) tout << "\t57653";
                    if ( P[20] == 1 ) tout << "\t57656";
                    if ( P[21] == 1 ) tout << "\t57651";
                    if ( P[22] == 1 ) tout << "\t57654";
                    if ( P[23] == 1 ) tout << "\t57657";
                    tout << "\t50007" << eol;
                }
                else
                {
                    tout << "Station\tDate/Time\tLatitude\tLongitude\tIIiii";
                    if ( P[1]  == 1 ) tout << "\tTotal cloud amount [code]";
                    if ( P[2]  == 1 ) tout << "\tWind direction [deg]";
                    if ( P[3]  == 1 ) tout << "\tWind speed [m/sec]";
                    if ( P[4]  == 1 ) tout << "\tTemperature, air [deg C]";
                    if ( P[5]  == 1 ) tout << "\tDew/frost point [deg C]";
                    if ( P[6]  == 1 ) tout << "\tStation Pressure [hPa]";
                    if ( P[7]  == 1 ) tout << "\tGeopotential height [m]";
                    if ( P[8]  == 1 ) tout << "\tPresent weather [code]";
                    if ( P[9]  == 1 ) tout << "\tPast weather1 [code]";
                    if ( P[10] == 1 ) tout << "\tPast weather2 [code]";
                    if ( P[11] == 1 ) tout << "\tLow/middle cloud amount [code]";
                    if ( P[12] == 1 ) tout << "\tLow cloud [code]";
                    if ( P[13] == 1 ) tout << "\tMiddle cloud [code]";
                    if ( P[14] == 1 ) tout << "\tHigh cloud [code]";
                    if ( P[15] == 1 ) tout << "\tNs 1 [code]";
                    if ( P[16] == 1 ) tout << "\tC 1 [code]";
                    if ( P[17] == 1 ) tout << "\thshs 1 [code]";
                    if ( P[18] == 1 ) tout << "\tNs 2 [code]";
                    if ( P[19] == 1 ) tout << "\tC 2 [code]";
                    if ( P[20] == 1 ) tout << "\thshs 2 [code]";
                    if ( P[21] == 1 ) tout << "\tNs 3 [code]";
                    if ( P[22] == 1 ) tout << "\tC 3 [code]";
                    if ( P[23] == 1 ) tout << "\thshs 3 [code]";
                    tout << "\t" << tr( "YYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) << eol;
                }
                break;
            }

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    switch ( i_Format )
                    {
                    case 1: // LIN
                        InputStr = InputStr.simplified();

                        i_Day	= InputStr.mid( 0, 2 ).toInt();
                        i_Hour	= InputStr.mid( 2, 2 ).toInt();

                        dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                        dt.setTime( QTime( 0, 0, 0 ) );
                        dt = dt.addSecs( i_Hour*3600 );

                        if ( b_Import == false )
                            OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
                        else
                            OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time


                        if ( b_Import == false )
                            OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ) );            // IIiii

                        if ( P[1] == 1 )  // Total cloud amount [code]
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 2, 2 ).left( 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2] == 1 )  // Wind direction [deg]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 1, 2 ).toInt()*10 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3] == 1 )  // Wind speed [m/sec]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 3, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4] == 1 )  // Temperature, air [deg C]
                        {
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 3 ) != "///" )
                            {
                                float TTT = InputStr.section( " ", 3, 3 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 3, 3 ).mid( 1, 1 ).toInt() == 1 )
                                    TTT = -1*TTT;

                                if ( TTT > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TTT ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[5] == 1 )  // Dew/frost point [deg C]
                        {
                            if ( InputStr.section( " ", 4, 4 ).mid( 2, 3 ) != "///" )
                            {
                                float TdTdTd = InputStr.section( " ", 4, 4 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ).toInt() == 1 )
                                    TdTdTd = -1*TdTdTd;

                                if ( TdTdTd > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TdTdTd ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6] == 1 )  // Station Pressure [hPa]
                        {
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7] == 1 ) // Pressure, atmospheric [hPa]
                        {
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8] == 1 )  // Present weather [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 7, 7 ).mid( 1, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[9] == 1 )  // Past weather1 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[10] == 1 )  // Past weather2 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[11] == 1 )  // Low/middle cloud amount [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[12] == 1 )  // Low cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[13] == 1 )  // Middle cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[14] == 1 )  // High cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[15] == 1 )  // Ns 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[16] == 1 )  // C 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[17] == 1 )  // hshs 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[18] == 1 )  // Ns 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[19] == 1 )  // C 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[20] == 1 )  // hshs 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[21] == 1 )  // Ns 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[22] == 1 )  // C 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[23] == 1 )  // hshs 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        OutputStr.replace( "/", "" );

                        tout << OutputStr << "\t" << InputStr << eol;

                        break;

                    case 2: // GVN and NYA
                        i_Day		= InputStr.mid( 1, 2 ).toInt();
                        i_Minute	= InputStr.mid( 4, 4 ).toInt();

                        dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                        dt.setTime( QTime( 0, 0, 0 ) );
                        dt = dt.addSecs( i_Minute*60 );

                        if ( b_Import == false )
                            OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
                        else
                            OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

                        if ( P[1] == 1 )  // Temperature, air [deg C]
                        {
                            if ( ( InputStr.mid( 9, 5 ).contains( "/" ) == false ) && ( InputStr.mid( 9, 5 ).toFloat() > -90 ) )
                                OutputStr.append( "\t" + InputStr.mid( 9, 5 ).simplified() );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2] == 1 ) // Pressure, atmospheric [hPa]
                        {
                            if ( ( InputStr.mid( 16, 4 ).contains( "/" ) == false ) && ( InputStr.mid( 16, 4 ).toFloat() > -90 ) )
                                OutputStr.append( "\t" + InputStr.mid( 16, 4 ).simplified() );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3] == 1 ) // Dew/frost point [deg C]
                        {
                            if ( ( InputStr.mid( 21, 5 ).contains( "/" ) == false ) && ( InputStr.mid( 21, 5 ).toFloat() > -90 ) )
                                OutputStr.append( "\t" + InputStr.mid( 21, 5 ).simplified() );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4] == 1 ) // Wind direction [deg]
                        {
                            if ( ( InputStr.mid( 27, 3 ).contains( "/" ) == false ) && ( InputStr.mid( 27, 3 ).toFloat() > -90 ) )
                                OutputStr.append( "\t" + InputStr.mid( 27, 3 ).simplified() );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[5] == 1 ) // Wind speed [m/sec]
                        {
                            if ( ( InputStr.mid( 31, 3 ).contains( "/" ) == false ) && ( InputStr.mid( 31, 3 ).toFloat() > -90 ) )
                                OutputStr.append( "\t" + InputStr.mid( 31, 3 ).simplified() );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6] == 1 )
                        {
                            if ( InputStr.mid( 35, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 35, 1 ) );	// Past weather1 [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7] == 1 )
                        {
                            if ( InputStr.mid( 37, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 37, 2 ) );	// Present weather [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8] == 1 )
                        {
                            if ( InputStr.mid( 40, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 40, 1 ) );	// Total cloud amount [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[9] == 1 )
                        {
                            if ( InputStr.mid( 42, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 42, 1 ) );	// Low/middle cloud amount [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[10] == 1 )
                        {
                            if ( InputStr.mid( 44, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 44, 1 ) );	// Low cloud [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[11] == 1 )
                        {
                            if ( InputStr.mid( 46, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 46, 1 ) );	// Middle cloud [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[12] == 1 )
                        {
                            if ( InputStr.mid( 48, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 48, 1 ) );	// High cloud [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[13] == 1 )
                        {
                            if ( InputStr.mid( 50, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 50, 1 ) );	// Cloud base height [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[14] == 1 )
                        {
                            if ( InputStr.mid( 52, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 52, 1 ) );	// Present blowing snow [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[15] == 1 )
                        {
                            if ( InputStr.mid( 53, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 53, 1 ) );	// Past blowing snow [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[16] == 1 )
                        {
                            if ( InputStr.mid( 55, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.mid( 55, 2 ) );	// Horizontal visibility [code]
                            else
                                OutputStr.append( "\t" );
                        }

                        OutputStr.replace( "/", "" );

                        tout << OutputStr << "\t" << InputStr << eol;

                        break;

                    case 3: // TAM
                        InputStr = InputStr.simplified();

                        i_Day	= InputStr.mid( 0, 2 ).toInt();
                        i_Hour	= InputStr.mid( 2, 2 ).toInt();

                        dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                        dt.setTime( QTime( 0, 0, 0 ) );
                        dt = dt.addSecs( i_Hour*3600 );

                        if ( b_Import == false )
                            OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
                        else
                            OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

                        if ( b_Import == false ) // IIiii
                            OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ) );

                        if ( P[1] == 1 ) // Cloud base height [code]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 2, 2 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2] == 1 ) // Horizontal visibility [code]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 2, 2 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3] == 1 ) // Total cloud amount [code]
                        {
                            if ( InputStr.section( " ", 3, 3 ).left( 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 3, 3 ).left( 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4] == 1 ) // Wind direction [deg]
                        {
                            if ( InputStr.section( " ", 3, 3 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 3, 3 ).mid( 1, 2 ).toInt()*10 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[5] == 1 ) // Wind speed [m/sec]
                        {
                            if ( InputStr.section( " ", 3, 3 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 3, 3 ).mid( 3, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6] == 1 ) // Temperature, air [deg C]
                        {
                            if ( InputStr.section( " ", 4, 4 ).mid( 2, 3 ) != "///" )
                            {
                                float TTT = InputStr.section( " ", 4, 4 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ).toInt() == 1 )
                                    TTT = -1*TTT;

                                if ( TTT > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TTT ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7] == 1 ) // Dew/frost point [deg C]
                        {
                            if ( InputStr.section( " ", 5, 5 ).mid( 2, 3 ) != "///" )
                            {
                                float TdTdTd = InputStr.section( " ", 5, 5 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 5, 5 ).mid( 1, 1 ).toInt() == 1 )
                                    TdTdTd = -1*TdTdTd;

                                if ( TdTdTd > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TdTdTd ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8] == 1 ) // Station Pressure [hPa]
                        {
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[9] == 1 ) // Geopotential height [m]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 4 ) != "////" )
                                OutputStr.append( QString( "\t%1" ).arg( calcGeopotentialHeight( InputStr.section( " ", 7, 7 ) ) ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[10] == 1 ) // Present weather [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 8, 8 ).mid( 1, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[11] == 1 ) // Past weather1 [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[12] == 1 ) // Past weather2 [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[13] == 1 ) // Low/middle cloud amount [code]
                        {
                            if ( InputStr.section( " ", 9, 9 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 9, 9 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[14] == 1 ) // Low cloud [code]
                        {
                            if ( InputStr.section( " ", 9, 9 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 9, 9 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[15] == 1 ) // Middle cloud [code]
                        {
                            if ( InputStr.section( " ", 9, 9 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 9, 9 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[16] == 1 ) // High cloud [code]
                        {
                            if ( InputStr.section( " ", 9, 9 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 9, 9 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

//                      InputStr.section( " ", 10, 10 ) = "333";

                        if ( P[17] == 1 ) // Ns 1 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[18] == 1 ) // C 1 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[19] == 1 ) // hshs 1 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[20] == 1 ) // Ns 2 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[21] == 1 ) // C 2 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[22] == 1 ) // hshs 2 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[23] == 1 ) // Ns 3 [code]
                        {
                            if ( InputStr.section( " ", 13, 13 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 13, 13 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[24] == 1 ) // C 3 [code]
                        {
                            if ( InputStr.section( " ", 13, 13 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 13, 13 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[25] == 1 ) // hshs 3 [code]
                        {
                            if ( InputStr.section( " ", 13, 13 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 13, 13 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        OutputStr.replace( "/", "" );

                        tout << OutputStr << "\t" << InputStr << eol;

                        break;

                    case 4: // PAY
                        InputStr = InputStr.simplified();

                        i_Day	= InputStr.mid( 0, 2 ).toInt();
                        i_Hour	= InputStr.mid( 2, 2 ).toInt();

                        dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                        dt.setTime( QTime( 0, 0, 0 ) );
                        dt = dt.addSecs( i_Hour*3600 );

                        if ( b_Import == false )
                            OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
                        else
                            OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

                        if ( b_Import == false ) // IIiii
                            OutputStr.append( "\tnot given" );

                        if ( P[1] == 1 ) // Cloud base height [code]
                        {
                            if ( InputStr.section( " ", 1, 1 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2] == 1 ) // Horizontal visibility [code]
                        {
                            if ( InputStr.section( " ", 1, 1 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3] == 1 ) // Total cloud amount [code]
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 2, 2 ).left( 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4] == 1 ) // Wind direction [deg]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 1, 2 ).toInt()*10 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[5] == 1 ) // Wind speed [m/sec]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 3, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6] == 1 )  // Temperature, air [deg C]
                        {
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 3 ) != "///" )
                            {
                                float TTT = InputStr.section( " ", 3, 3 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 3, 3 ).mid( 1, 1 ).toInt() == 1 )
                                    TTT = -1*TTT;

                                if ( TTT > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TTT ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7] == 1 )  // Dew/frost point [deg C]
                        {
                            if ( InputStr.section( " ", 4, 4 ).mid( 2, 3 ) != "///" )
                            {
                                float TdTdTd = InputStr.section( " ", 4, 4 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ).toInt() == 1 )
                                    TdTdTd = -1*TdTdTd;

                                if ( TdTdTd > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TdTdTd ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8] == 1 ) // Station Pressure [hPa]
                        {
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[9] == 1 ) // Pressure, atmospheric [hPa]
                        {
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 6, 6 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[10] == 1 ) // Present weather [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 7, 7 ).mid( 1, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[11] == 1 ) // Past weather1 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[12] == 1 ) // Past weather2 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[13] == 1 ) // Low/middle cloud amount [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[14] == 1 ) // Low cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[15] == 1 ) // Middle cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[16] == 1 ) // High cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

//                      InputStr.section( " ", 9, 9 ) = "333";

                        if ( P[17] == 1 ) // Ns 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[18] == 1 ) // C 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[19] == 1 ) // hshs 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[20] == 1 ) // Ns 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[21] == 1 ) // C 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[22] == 1 ) // hshs 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[23] == 1 ) // Ns 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[24] == 1 ) // C 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[25] == 1 ) // hshs 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        OutputStr.replace( "/", "" );

                        tout << OutputStr << "\t" << InputStr << eol;

                        break;

                    case 5: // SON
                        InputStr = InputStr.simplified();

                        i_Day	= InputStr.mid( 0, 2 ).toInt();
                        i_Hour	= InputStr.mid( 2, 2 ).toInt();

                        dt.setDate( QDate( i_Year, i_Month, i_Day ) );
                        dt.setTime( QTime( 0, 0, 0 ) );
                        dt = dt.addSecs( i_Hour*3600 );

                        if ( b_Import == false )
                            OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
                        else
                            OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

                        if ( b_Import == false )
                            OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ) );            // IIiii

                        if ( P[1] == 1 )  // Total cloud amount [code]
                        {
                            if ( InputStr.section( " ", 2, 2 ).left( 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 2, 2 ).left( 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[2] == 1 )  // Wind direction [deg]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 1, 2 ).toInt()*10 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[3] == 1 )  // Wind speed [m/sec]
                        {
                            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 2, 2 ).mid( 3, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[4] == 1 )  // Temperature, air [deg C]
                        {
                            if ( InputStr.section( " ", 3, 3 ).mid( 2, 3 ) != "///" )
                            {
                                float TTT = InputStr.section( " ", 3, 3 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 3, 3 ).mid( 1, 1 ).toInt() == 1 )
                                    TTT = -1*TTT;

                                if ( TTT > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TTT ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[5] == 1 )  // Dew/frost point [deg C]
                        {
                            if ( InputStr.section( " ", 4, 4 ).mid( 2, 3 ) != "///" )
                            {
                                float TdTdTd = InputStr.section( " ", 4, 4 ).mid( 2, 3 ).toFloat()/10.;

                                if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ).toInt() == 1 )
                                    TdTdTd = -1*TdTdTd;

                                if ( TdTdTd > -90 )
                                    OutputStr.append( QString( "\t%1" ).arg( TdTdTd ) );
                                else
                                    OutputStr.append( "\t" );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[6] == 1 )  // Station Pressure [hPa]
                        {
                            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" )
                            {
                                if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. <= 200. )
                                    OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                                else
                                    OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 5, 5 ).mid( 1, 4 ).toFloat()/10. ) );
                            }
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[7] == 1 ) // Geopotential height [m]
                        {
                            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" )
                                OutputStr.append( QString( "\t%1" ).arg( calcGeopotentialHeight( InputStr.section( " ", 6, 6 ) ) ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[8] == 1 )  // Present weather [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" )
                                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", 7, 7 ).mid( 1, 2 ).toInt() ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[9] == 1 )  // Past weather1 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[10] == 1 )  // Past weather2 [code]
                        {
                            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 7, 7 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[11] == 1 )  // Low/middle cloud amount [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[12] == 1 )  // Low cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[13] == 1 )  // Middle cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 3, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[14] == 1 )  // High cloud [code]
                        {
                            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 8, 8 ).mid( 4, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[15] == 1 )  // Ns 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[16] == 1 )  // C 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[17] == 1 )  // hshs 1 [code]
                        {
                            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 10, 10 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[18] == 1 )  // Ns 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[19] == 1 )  // C 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[20] == 1 )  // hshs 2 [code]
                        {
                            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 11, 11 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[21] == 1 )  // Ns 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 1, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[22] == 1 )  // C 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 2, 1 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        if ( P[23] == 1 )  // hshs 3 [code]
                        {
                            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" )
                                OutputStr.append( "\t" + InputStr.section( " ", 12, 12 ).mid( 3, 2 ) );
                            else
                                OutputStr.append( "\t" );
                        }

                        OutputStr.replace( "/", "" );

                        tout << OutputStr << "\t" << InputStr << eol;

                        break;
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

    resetProgress( i_NumOfFiles );

    fin.close();

    fout.close();

    if ( ui_length == (unsigned int) _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}


// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2013-03-11

float MainWindow::calcGeopotentialHeight( const QString& InputStr )
{
    // http://weather.unisys.com/wxp/Appendices/Formats/SYNOP.html
    // http://weather.unisys.com/wxp/Appendices/Formats/TEMP.html

    int   MPL = InputStr.mid( 1, 1 ).toInt();   // mandatory pressure level
    float HHH = InputStr.mid( 2, 3 ).toFloat(); // Geopotential height (m)

    switch( MPL )
    {
    case 1: // 1000 hPa
        if ( HHH >= 500. )
            HHH = 500. - HHH;
        break;

    case 2: // 925 hPa
        // HHH = HHH;
        break;

    case 5: // 500 hPa
        HHH *= 10;
        break;

    case 7: // 700 hPa
        if ( HHH < 500. )
            HHH += 3000.;
        else
            HHH += 2000.;
        break;

    case 8: // 850 hPa
        HHH += 1000;
        break;
    }

    return( HHH );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des Expanded Measurements Converters, LR 1000 */

void MainWindow::doSYNOPConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "SYNOP converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = SYNOPConverter( b_Import, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

            stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
        }

        resetFileProgress( gsl_FilenameList.count() );
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "SYNOP converter was canceled" ), false, false );

    onError( err );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung des SYNOP Converters im Import-Mode, LR 1000 */

void MainWindow::doSYNOPImportConverter()
{
    doSYNOPConverter( true );
}

