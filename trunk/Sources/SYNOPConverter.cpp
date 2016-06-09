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
    int             i_Year          = 2000;

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

            if ( fi.baseName().startsWith( "lin" ) == true ) i_Format = 1;
            if ( fi.baseName().startsWith( "sbo" ) == true ) i_Format = 1;

            if ( fi.baseName().startsWith( "fua" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "ish" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "mnm" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "sap" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "syo" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "tat" ) == true ) i_Format = 2;
            if ( fi.baseName().startsWith( "nya" ) == true ) i_Format = 2;

            if ( fi.baseName().startsWith( "tam" ) == true ) i_Format = 3;

            if ( fi.baseName().startsWith( "pay" ) == true ) i_Format = 4;

            if ( fi.baseName().startsWith( "son" ) == true ) i_Format = 5;

            if ( fi.baseName().startsWith( "gvn" ) == true ) i_Format = 6; // 2016-05

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    switch ( i_Format )
                    {
                    case 1: // LIN, SBO
                        b_Stop = SYNOPTest1( InputStr.simplified(), P );
                        break;

                    case 2: // FUA, ISH, MNM, SAP, SYO, TAT, NYA
                        b_Stop = SYNOPTest2( InputStr, P );
                        break;

                    case 3: // TAM
                        i_Year = fi.baseName().right( 2 ).toInt();
                        b_Stop = SYNOPTest3( InputStr.simplified(), i_Year, P );
                        break;

                    case 4: // PAY
                        b_Stop = SYNOPTest4( InputStr.simplified(), P );
                        break;

                    case 5: // SON
                        b_Stop = SYNOPTest5( InputStr.simplified(), P );
                        break;

                    case 6: // GVN, 2016-05
                        b_Stop = SYNOPTest6( InputStr, P );
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
// LIN, SBO

bool MainWindow::SYNOPTest1( const QString &InputStr, int *P )
{
    bool b_Stop = false;

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

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// FUA, ISH, MNM, SAP, SYO, TAT, NYA

bool MainWindow::SYNOPTest2( const QString &InputStr, int *P )
{
    bool b_Stop = false;

    if ( InputStr.mid(  9, 5 ) != "-99.0" ) P[1] = 1;  // Temperature, air [deg C]
    if ( InputStr.mid( 17, 3 ) != "-99" )   P[2] = 1;  // Pressure, atmospheric [hPa]
    if ( InputStr.mid( 21, 5 ) != "-99.0" ) P[3] = 1;  // Dew/frost point [deg C]
    if ( InputStr.mid( 27, 3 ) != "-99" )   P[4] = 1;  // Wind direction [deg]
    if ( InputStr.mid( 31, 3 ) != "-99" )   P[5] = 1;  // Wind speed [m/sec]
    if ( InputStr.mid( 35, 1 ) != "/" )     P[6] = 1;  // Past weather1 [code]
    if ( InputStr.mid( 37, 2 ) != "//" )    P[7] = 1;  // Present weather [code]
    if ( InputStr.mid( 40, 1 ) != "/" )     P[8] = 1;  // Total cloud amount [code]
    if ( InputStr.mid( 42, 1 ) != "/" )     P[9] = 1;  // Low/middle cloud amount [code]
    if ( InputStr.mid( 44, 1 ) != "/" )     P[10] = 1; // Low cloud [code]
    if ( InputStr.mid( 46, 1 ) != "/" )     P[11] = 1; // Middle cloud [code]
    if ( InputStr.mid( 48, 1 ) != "/" )     P[12] = 1; // High cloud [code]
    if ( InputStr.mid( 50, 1 ) != "/" )     P[13] = 1; // Cloud base height [code]
    if ( InputStr.mid( 52, 1 ) != "/" )     P[14] = 1; // Present blowing snow [code]
    if ( InputStr.mid( 53, 1 ) != "/" )     P[15] = 1; // Past blowing snow [code]
    if ( InputStr.mid( 55, 2 ) != "//" )    P[16] = 1; // Horizontal visibility [code]

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// TAM

bool MainWindow::SYNOPTest3( const QString &InputStr, const int i_Year, int *P )
{
    bool b_Stop = false;

    if ( i_Year < 8 ) // 2000-2007
    {
//      0     1     2         3       4        5           6        7      8          9            10  11         12
//      YYGG9 IIiii iRix|h|VV N|dd|ff 1|S|nTTT 2|S|nTdTdTd 3|P0P0P0 4|PPPP 7|ww|W1|W2 8|N|Cl|Cm|Ch 333 8|N|C|hshs 8|N|C|hshs
//      31064 60680 44|9|70   6|06|02 1|0|026  2|1|084     3|8620   4|8492 7|03|1|0   8|3|0|3|2    333 8|3|3|62   8|5|0|73

        if ( InputStr.count( " " ) >= 2 ) // iRixhVV
        {
            if ( InputStr.section( " ", 2, 2 ).mid( 2, 1 ) != "/" )  P[1] = 1; // h
            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" ) P[2] = 1; // VV
        }

        if ( InputStr.count( " " ) >= 3 ) // Nddff
        {
            if ( InputStr.section( " ", 3, 3 ).left( 1 )   != "/" )  P[3] = 1; // N
            if ( InputStr.section( " ", 3, 3 ).mid( 1, 2 ) != "//" ) P[4] = 1; // dd
            if ( InputStr.section( " ", 3, 3 ).mid( 3, 2 ) != "//" ) P[5] = 1; // ff
        }

        if ( InputStr.count( " " ) >= 4 ) // 1SnTTT
            if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ) != "/" ) P[6] = 1; // sign

        if ( InputStr.count( " " ) >= 5 ) // 2SnTdTdTd
            if ( InputStr.section( " ", 5, 5 ).mid( 1, 1 ) != "/" ) P[7] = 1; // sign

        if ( InputStr.count( " " ) >= 6 ) // 3P0P0P0
            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[8] = 1; // xxxx

        if ( InputStr.count( " " ) >= 7 ) // 4PPPP
            if ( InputStr.section( " ", 7, 7 ).mid( 1, 4 ) != "////" ) P[9] = 1; // xxxx

        P[9] = 0;  // Geopotential height will be not stored in BSRN datasets, GKL 2013-03-11

        if ( InputStr.count( " " ) >= 8 ) // 7wwW1W2
        {
            if ( InputStr.section( " ", 8, 8 ).mid( 1, 2 ) != "//" ) P[10] = 1; // ww
            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" )  P[11] = 1; // W1
            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" )  P[12] = 1; // W2
        }

        if ( InputStr.count( " " ) >= 9 ) // 8NClCmCh
        {
            if ( InputStr.section( " ", 9, 9 ).mid( 1, 1 ) != "/" ) P[13] = 1; // N
            if ( InputStr.section( " ", 9, 9 ).mid( 2, 1 ) != "/" ) P[14] = 1; // Cl
            if ( InputStr.section( " ", 9, 9 ).mid( 3, 1 ) != "/" ) P[15] = 1; // Cm
            if ( InputStr.section( " ", 9, 9 ).mid( 4, 1 ) != "/" ) P[16] = 1; // Ch
        }

        if ( InputStr.count( " ") >= 10 ) // 333
        {
            if ( InputStr.section( " ", 10, 10 ) != "333" )
                b_Stop = true;
        }

        if ( InputStr.count( " " ) >= 11 ) // 8NChshs
        {
            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )  P[17] = 1; // N
            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )  P[18] = 1; // C
            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[19] = 1; // hshs
        }

        if ( InputStr.count( " " ) >= 12 ) // 8NChshs
        {
            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" ) P[20] = 1;
            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" ) P[21] = 1;
            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[22] = 1;
        }
    }

    if ( i_Year > 7 ) // 2008-20xx
    {
//     0     1     2       3        4           5        6      7        8            9   10          11          12
//     YYGG9 IIiii N|dd|ff 1|S|nTTT 2|S|nTdTdTd 3|P0P0P0 4|PPPP 7|wwW1W2 8|N|Cl|Cm|Ch 333 8|N|C|hshsh 8|N|C|hshsh 8|N|C|hshsh
//     02064 60680 1|00|00 1|1|004  2|0|107     3|8655   4|8524 7|03|0|0 8|0|0|0|2    333 8|1|0|73    8|/|/|//    8|/|/|//

        if ( InputStr.count( " " ) >= 2 ) // Nddff
        {
            if ( InputStr.section( " ", 2, 2 ).left( 1 )   != "/" )  P[3] = 1; // N
            if ( InputStr.section( " ", 2, 2 ).mid( 1, 2 ) != "//" ) P[4] = 1; // dd
            if ( InputStr.section( " ", 2, 2 ).mid( 3, 2 ) != "//" ) P[5] = 1; // ff
        }

        if ( InputStr.count( " " ) >= 3 ) // 1SnTTT
            if ( InputStr.section( " ", 3, 3 ).mid( 1, 1 ) != "/" ) P[6] = 1; // sign

        if ( InputStr.count( " " ) >= 4 ) // 2SnTdTdTd
            if ( InputStr.section( " ", 4, 4 ).mid( 1, 1 ) != "/" ) P[7] = 1; // sign

        if ( InputStr.count( " " ) >= 5 ) // 3P0P0P0
            if ( InputStr.section( " ", 5, 5 ).mid( 1, 4 ) != "////" ) P[8] = 1; // xxxx

        if ( InputStr.count( " " ) >= 6 ) // 4PPPP
            if ( InputStr.section( " ", 6, 6 ).mid( 1, 4 ) != "////" ) P[9] = 1; // xxxx

        P[9] = 0;  // Geopotential height will be not stored in BSRN datasets, GKL 2013-03-11

        if ( InputStr.count( " " ) >= 7 ) // 7wwW1W2
        {
            if ( InputStr.section( " ", 7, 7 ).mid( 1, 2 ) != "//" ) P[10] = 1; // ww
            if ( InputStr.section( " ", 7, 7 ).mid( 3, 1 ) != "/" )  P[11] = 1; // W1
            if ( InputStr.section( " ", 7, 7 ).mid( 4, 1 ) != "/" )  P[12] = 1; // W2
        }

        if ( InputStr.count( " " ) >= 8 ) // 8NClCmCh
        {
            if ( InputStr.section( " ", 8, 8 ).mid( 1, 1 ) != "/" ) P[13] = 1; // N
            if ( InputStr.section( " ", 8, 8 ).mid( 2, 1 ) != "/" ) P[14] = 1; // Cl
            if ( InputStr.section( " ", 8, 8 ).mid( 3, 1 ) != "/" ) P[15] = 1; // Cm
            if ( InputStr.section( " ", 8, 8 ).mid( 4, 1 ) != "/" ) P[16] = 1; // Ch
        }

        if ( InputStr.count( " ") >= 9 ) // 333
        {
            if ( InputStr.section( " ", 9, 9 ) != "333" )
                b_Stop = true;
        }

        if ( InputStr.count( " " ) >= 10 ) // 8NChshs
        {
            if ( InputStr.section( " ", 10, 10 ).mid( 1, 1 ) != "/" )  P[17] = 1; // N 1
            if ( InputStr.section( " ", 10, 10 ).mid( 2, 1 ) != "/" )  P[18] = 1; // C 1
            if ( InputStr.section( " ", 10, 10 ).mid( 3, 2 ) != "//" ) P[19] = 1; // hshs 1
        }

        if ( InputStr.count( " " ) >= 11 ) // 8NChshs
        {
            if ( InputStr.section( " ", 11, 11 ).mid( 1, 1 ) != "/" )  P[20] = 1; // N 2
            if ( InputStr.section( " ", 11, 11 ).mid( 2, 1 ) != "/" )  P[21] = 1; // C 2
            if ( InputStr.section( " ", 11, 11 ).mid( 3, 2 ) != "//" ) P[22] = 1; // hshs 3
        }

        if ( InputStr.count( " " ) >= 12 ) // 8NChshs
        {
            if ( InputStr.section( " ", 12, 12 ).mid( 1, 1 ) != "/" )  P[23] = 1; // N 2
            if ( InputStr.section( " ", 12, 12 ).mid( 2, 1 ) != "/" )  P[24] = 1; // C 2
            if ( InputStr.section( " ", 12, 12 ).mid( 3, 2 ) != "//" ) P[25] = 1; // hshs 3
        }

    }

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// PAY

bool MainWindow::SYNOPTest4( const QString &InputStr, int *P )
{
    bool b_Stop = false;

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

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// SON

bool MainWindow::SYNOPTest5( const QString &InputStr, int *P )
{
    bool b_Stop = false;

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

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// GVN, 2016-05

bool MainWindow::SYNOPTest6( const QString &InputStr, int *P )
{
    bool b_Stop = false;

    if ( InputStr.mid(  9, 1 ) != "/" )     P[1] = 1;  // Cloud base height [code]
    if ( InputStr.mid( 11, 2 ) != "//" )    P[2] = 1;  // Horizontal visibility [code]
    if ( InputStr.mid( 14, 3 ) != "-99" )   P[3] = 1;  // Wind direction [deg]
    if ( InputStr.mid( 18, 5 ) != "-99.0" ) P[4] = 1;  // Wind speed [m/sec]
    if ( InputStr.mid( 24, 5 ) != "-99.0" ) P[5] = 1;  // Temperature, air [deg C]
    if ( InputStr.mid( 30, 5 ) != "-99.0" ) P[6] = 1;  // Dew/frost point [deg C]
    if ( InputStr.mid( 37, 5 ) != "-99.0" ) P[7] = 1;  // Pressure, atmospheric [hPa]
    if ( InputStr.mid( 43, 1 ) != "/" )     P[8] = 1;  // Characteristic of barometric tendency [code]
    if ( InputStr.mid( 45, 5 ) != "-99.0" ) P[9] = 1;  // Amount of barometric tendency [hPa]
    if ( InputStr.mid( 51, 2 ) != "//" )    P[10] = 1; // Present weather [code]
    if ( InputStr.mid( 54, 1 ) != "/" )     P[11] = 1; // Past weather1 [code]
    if ( InputStr.mid( 56, 1 ) != "/" )     P[12] = 1; // Past weather2 [code]
    if ( InputStr.mid( 58, 1 ) != "/" )     P[13] = 1; // Low cloud [code]
    if ( InputStr.mid( 60, 1 ) != "/" )     P[14] = 1; // Middle cloud [code]
    if ( InputStr.mid( 62, 1 ) != "/" )     P[15] = 1; // High cloud [code]
    if ( InputStr.mid( 64, 1 ) != "/" )     P[16] = 1; // Total cloud amount [code]
    if ( InputStr.mid( 66, 1 ) != "/" )     P[17] = 1; // Low/middle cloud amount [code]
    if ( InputStr.mid( 68, 5 ) != "-99.0" ) P[18] = 1; // Temperature, air, maximum [deg C]
    if ( InputStr.mid( 74, 5 ) != "-99.0" ) P[19] = 1; // Temperature, air, minimum [deg C]
    if ( InputStr.mid( 80, 1 ) != "/" )     P[20] = 1; // Present blowing snow [code]
    if ( InputStr.mid( 82, 1 ) != "/" )     P[21] = 1; // Past blowing snow [code]
    if ( InputStr.mid( 84, 1 ) != "/" )     P[22] = 1; // Whiteout yes/no [y/n]

    return( b_Stop );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// LIN

QStringList MainWindow::buildSYNOPDataDescriptionHeader1( int *P, const int i_PIID )
{
    QStringList sl_Parameter;

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

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// FUA, ISH, MNM, SAP, SYO, TAT, NYA

QStringList MainWindow::buildSYNOPDataDescriptionHeader2( int *P, const int i_PIID )
{
    QStringList sl_Parameter;

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

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// TAM

QStringList MainWindow::buildSYNOPDataDescriptionHeader3( int *P, const int i_PIID, const int i_Year )
{
    QStringList sl_Parameter;

    if ( P[1]  == 1 ) sl_Parameter.append( Parameter( num2str( 45259 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // h, Cloud base height
    if ( P[2]  == 1 ) sl_Parameter.append( Parameter( num2str( 45260 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // VV, Horizontal visibility

    if ( P[3]  == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // N, Total cloud amount
    if ( P[4]  == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );    // dd, Wind direction [deg]
    if ( P[5]  == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0" ) ) );     // ff, Wind speed [m/sec]

    if ( P[6]  == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // TTT, Temperature, air [deg C]

    if ( P[7]  == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );  // TdTdTd, Dew/frost point [deg C]

    if ( P[8]  == 1 ) sl_Parameter.append( Parameter( num2str( 48823 ), num2str( i_PIID ), num2str( 359 ), tr( "###0" ) ) );    // P0P0P0, Station Pressure [hPa]

    if ( P[9]  == 1 ) sl_Parameter.append( Parameter( num2str( 123640 ), num2str( i_PIID ), num2str( 50 ), tr( "###0" ) ) );    // PPPP, Geopotential height [m]

    if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // ww, Present weather [code]
    if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // W1, Past weather1 [code]
    if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // W2, Past weather2 [code]

    if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // N, Low/middle cloud amount [code]
    if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Cl, Low cloud [code]
    if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Cm, Middle cloud [code]
    if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Ch, High cloud [code]

    // 333

    if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 57649 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // N, Amount of cloud layer 1 [code]
    if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 57652 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C, Cloud layer 1 [code]
    if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 57655 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs, Cloud base height code, layer 1 [code]

    if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 57650 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // N, Amount of cloud layer 2 [code]
    if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 57653 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C, Cloud layer 2 [code]
    if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 57656 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs, Cloud base height code, layer 2 [code]

    if ( P[23] == 1 ) sl_Parameter.append( Parameter( num2str( 57651 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // N, Amount of cloud layer 3 [code]
    if ( P[24] == 1 ) sl_Parameter.append( Parameter( num2str( 57654 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // C, Cloud layer 3 [code]
    if ( P[25] == 1 ) sl_Parameter.append( Parameter( num2str( 57657 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );     // hshs, Cloud base height code, layer 3 [code]

    if ( i_Year < 2008 )
        sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs" ) ) );
    else
        sl_Parameter.append( Parameter( num2str( 50007 ), num2str( i_PIID ), num2str( 43 ), tr( "" ), tr( "YYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NChshsh 8NChshsh 8NChshsh" ) ) );

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// PAY

QStringList MainWindow::buildSYNOPDataDescriptionHeader4( int *P, const int i_PIID )
{
    QStringList sl_Parameter;

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

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// SON

QStringList MainWindow::buildSYNOPDataDescriptionHeader5( int *P, const int i_PIID )
{
    QStringList sl_Parameter;

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

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// GVN, 2016-05

QStringList MainWindow::buildSYNOPDataDescriptionHeader6( int *P, const int i_PIID )
{
    QStringList sl_Parameter;

    if ( P[1] == 1 ) sl_Parameter.append( Parameter( num2str( 45259 ), num2str( i_PIID ), num2str( 5036 ), tr( "#0" ) ) );      // Cloud base height [code]
    if ( P[2] == 1 ) sl_Parameter.append( Parameter( num2str( 45260 ), num2str( i_PIID ), num2str( 5037 ), tr( "#0" ) ) );      // Horizontal visibility [code]
    if ( P[3] == 1 ) sl_Parameter.append( Parameter( num2str( 2221 ), num2str( i_PIID ), num2str( 5038 ), tr( "###0" ) ) );     // Wind direction [deg]
    if ( P[4] == 1 ) sl_Parameter.append( Parameter( num2str( 18906 ), num2str( i_PIID ), num2str( 5038 ), tr( "#0.0" ) ) );    // Wind speed [m/sec]
    if ( P[5] == 1 ) sl_Parameter.append( Parameter( num2str( 4610 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );   // Temperature, air [deg C]
    if ( P[6] == 1 ) sl_Parameter.append( Parameter( num2str( 4611 ), num2str( i_PIID ), num2str( 5039 ), tr( "###0.0" ) ) );   // Dew/frost point [deg C]
    if ( P[7] == 1 ) sl_Parameter.append( Parameter( num2str( 2224 ), num2str( i_PIID ), num2str( 359 ), tr( "###0.0" ), tr( "Station pressure reduced to sea level" ) ) );   // Pressure, atmospheric [hPa]
    if ( P[8] == 1 ) sl_Parameter.append( Parameter( num2str( 45311 ), num2str( i_PIID ), num2str( 359 ), tr( "#0" ) ) );       // Characteristic of barometric tendency [code]
    if ( P[9] == 1 ) sl_Parameter.append( Parameter( num2str( 45312 ), num2str( i_PIID ), num2str( 359 ), tr( "#0" ) ) );       // Amount of barometric tendency [hPa]
    if ( P[10] == 1 ) sl_Parameter.append( Parameter( num2str( 45261 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present weather [code]
    if ( P[11] == 1 ) sl_Parameter.append( Parameter( num2str( 45262 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather1 [code]
    if ( P[12] == 1 ) sl_Parameter.append( Parameter( num2str( 45263 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past weather2 [code]
    if ( P[13] == 1 ) sl_Parameter.append( Parameter( num2str( 45264 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low cloud [code]
    if ( P[14] == 1 ) sl_Parameter.append( Parameter( num2str( 45265 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Middle cloud [code]
    if ( P[15] == 1 ) sl_Parameter.append( Parameter( num2str( 45266 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // High cloud [code]
    if ( P[16] == 1 ) sl_Parameter.append( Parameter( num2str( 45267 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Total cloud amount
    if ( P[17] == 1 ) sl_Parameter.append( Parameter( num2str( 45268 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Low/middle cloud amount [code]
    if ( P[18] == 1 ) sl_Parameter.append( Parameter( num2str( 5151 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air, maximum [deg C]
    if ( P[19] == 1 ) sl_Parameter.append( Parameter( num2str( 5150 ), num2str( i_PIID ), num2str( 4722 ), tr( "###0.0" ) ) );  // Temperature, air, minimum [deg C]
    if ( P[20] == 1 ) sl_Parameter.append( Parameter( num2str( 45307 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Present blowing snow [code]
    if ( P[21] == 1 ) sl_Parameter.append( Parameter( num2str( 45308 ), num2str( i_PIID ), num2str( 530 ), tr( "#0" ) ) );      // Past blowing snow [code]
    if ( P[22] == 1 ) sl_Parameter.append( Parameter( num2str( 45309 ), num2str( i_PIID ), num2str( 530 ), tr( "" ) ) );        // Whiteout yes/no [y/n]

    return( sl_Parameter );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// LIN

QString MainWindow::buildSYNOPDataHeader1( int *P, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\t45267" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\t2221" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\t18906" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\t4610" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\t4611" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\t48823" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\t2224" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\t45261" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\t45262" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45263" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45268" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45264" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45265" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45266" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t57649" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t57652" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\t57655" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\t57650" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\t57653" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\t57656" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\t57651" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\t57654" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\t57657" );

        s_SYNOPDataHeader.append( "\t50007" );
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude\tIIiii" );

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount [code]" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\tStation Pressure [hPa]" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\tPressure, atmospheric [hPa]" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tPast weather2 [code]" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tNs 1 [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tC 1 [code]" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\thshs 1 [code]" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\tNs 2 [code]" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\tC 2 [code]" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\thshs 2 [code]" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\tNs 3 [code]" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\tC 3 [code]" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\thshs 3 [code]" );

        s_SYNOPDataHeader.append( tr( "\tYYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// FUA, ISH, MNM, SAP, SYO, TAT, NYA

QString MainWindow::buildSYNOPDataHeader2( int *P, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\t4610" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\t2224" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\t4611" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\t2221" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\t18906" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\t45262" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\t45261" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\t45267" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\t45268" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45264" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45265" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45266" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45259" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45307" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t45308" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t45260" );
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude" );

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\tPressure, atmospheric [hPa]" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount [code]" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tCloud base height [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tPresent blowing snow [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tPast blowing snow [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tHorizontal visibility [code]" );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// TAM

QString MainWindow::buildSYNOPDataHeader3( int *P, const int i_Year, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\t45259" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\t45260" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\t45267" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\t2221" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\t18906" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\t4610" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\t4611" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\t48823" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\t123640" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45261" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45262" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45263" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45268" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45264" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t45265" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t45266" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\t57649" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\t57652" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\t57655" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\t57650" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\t57653" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\t57656" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\t57651" );
        if ( P[24] == 1 ) s_SYNOPDataHeader.append( "\t57654" );
        if ( P[25] == 1 ) s_SYNOPDataHeader.append( "\t57657" );

        s_SYNOPDataHeader.append( "\t50007" );
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude\tIIiii" );

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\tCloud base height [code]" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\tHorizontal visibility [code]" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount [code]" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\tStation Pressure [hPa]" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\tGeopotential height [m]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tPast weather2 [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\tAmount of cloud layer 1 [code]" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\tCloud layer 1 [code]" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\tCloud base height code, layer 1 [code]" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\tAmount of cloud layer 2 [code]" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\tCloud layer 2 [code]" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\tCloud base height code, layer 2 [code]" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\tAmount of cloud layer 3 [code]" );
        if ( P[24] == 1 ) s_SYNOPDataHeader.append( "\tCloud layer 3 [code]" );
        if ( P[25] == 1 ) s_SYNOPDataHeader.append( "\tCloud base height code, layer 4 [code]" );

        if ( i_Year < 2008 )
            s_SYNOPDataHeader.append( tr( "\tYYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs" ) );
        else
            s_SYNOPDataHeader.append( tr( "\tYYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NChshsh 8NChshsh 8NChshsh" ) );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// PAY

QString MainWindow::buildSYNOPDataHeader4( int *P, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\t45259" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\t45260" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\t45267" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\t2221" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\t18906" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\t4610" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\t4611" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\t48823" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\t2224" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45261" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45262" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45263" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45268" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45264" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t45265" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t45266" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\t57649" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\t57652" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\t57655" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\t57650" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\t57653" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\t57656" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\t57651" );
        if ( P[24] == 1 ) s_SYNOPDataHeader.append( "\t57654" );
        if ( P[25] == 1 ) s_SYNOPDataHeader.append( "\t57657" );

        s_SYNOPDataHeader.append( "\t50007" );
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude\tIIiii" );

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\tCloud base height [code]" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\tHorizontal visibility [code]" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount [code]" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\tStation Pressure [hPa]" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\tPressure, atmospheric [hPa]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[11]  == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tPast weather2 [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\tNs 1 [code]" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\tC 1 [code]" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\thshs 1 [code]" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\tNs 2 [code]" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\tC 2 [code]" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\thshs 2 [code]" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\tNs 3 [code]" );
        if ( P[24] == 1 ) s_SYNOPDataHeader.append( "\tC 3 [code]" );
        if ( P[25] == 1 ) s_SYNOPDataHeader.append( "\thshs 3 [code]" );

        s_SYNOPDataHeader.append( tr( "\tYYGG9 IIiii iRixhVV Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// SON

QString MainWindow::buildSYNOPDataHeader5( int *P, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\t45267" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\t2221" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\t18906" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\t4610" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\t4611" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\t48823" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\t123640" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\t45261" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\t45262" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45263" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45268" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45264" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45265" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45266" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t57649" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t57652" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\t57655" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\t57650" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\t57653" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\t57656" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\t57651" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\t57654" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\t57657" );

        s_SYNOPDataHeader.append( "\t50007" );
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude\tIIiii" );

        if ( P[1]  == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount [code]" );
        if ( P[2]  == 1 ) s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[3]  == 1 ) s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[4]  == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[5]  == 1 ) s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[6]  == 1 ) s_SYNOPDataHeader.append( "\tStation Pressure [hPa]" );
        if ( P[7]  == 1 ) s_SYNOPDataHeader.append( "\tGeopotential height [m]" );
        if ( P[8]  == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[9]  == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tPast weather2 [code]" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tNs 1 [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tC 1 [code]" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\thshs 1 [code]" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\tNs 2 [code]" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\tC 2 [code]" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\thshs 2 [code]" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\tNs 3 [code]" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\tC 3 [code]" );
        if ( P[23] == 1 ) s_SYNOPDataHeader.append( "\thshs 3 [code]" );

        s_SYNOPDataHeader.append( tr( "\tYYGG9 IIiii Nddff 1SnTTT 2SnTdTdTd 3P0P0P0 4PPPP 7wwW1W2 8NhClCmCh 333 8NsChshs 8NsChshs 8NsChshs" ) );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// GVN, 2016-05

QString MainWindow::buildSYNOPDataHeader6( int *P, const bool b_Import )
{
    QString s_SYNOPDataHeader = "";

    if ( b_Import == true )
    {
        s_SYNOPDataHeader.append( "1599" ); // Date/Time

        if ( P[1] == 1 ) s_SYNOPDataHeader.append( "\t45259" );  // Cloud base height [code]
        if ( P[2] == 1 ) s_SYNOPDataHeader.append( "\t45260" );  // Horizontal visibility [code]
        if ( P[3] == 1 ) s_SYNOPDataHeader.append( "\t2221" );   // Wind direction [deg]
        if ( P[4] == 1 ) s_SYNOPDataHeader.append( "\t18906" );  // Wind speed [m/sec]
        if ( P[5] == 1 ) s_SYNOPDataHeader.append( "\t4610" );   // Temperature, air [deg C]
        if ( P[6] == 1 ) s_SYNOPDataHeader.append( "\t4611" );   // Dew/frost point [deg C]
        if ( P[7] == 1 ) s_SYNOPDataHeader.append( "\t2224" );   // Pressure, atmospheric [hPa]
        if ( P[8] == 1 ) s_SYNOPDataHeader.append( "\t45311" );  // Characteristic of barometric tendency [code]
        if ( P[9] == 1 ) s_SYNOPDataHeader.append( "\t45312" );  // Amount of barometric tendency [hPa]
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\t45261" ); // Present weather [code]
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\t45262" ); // Past weather1 [code]
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\t45263" ); // Past weather2 [code]
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\t45264" ); // Low cloud [code]
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\t45265" ); // Middle cloud [code]
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\t45266" ); // High cloud [code]
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\t45267" ); // Total cloud amount
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\t45268" ); // Low/middle cloud amount [code]
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\t5151" );  // Temperature, air, maximum [deg C]
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\t5150" );  // Temperature, air, minimum [deg C]
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\t45307" ); // Present blowing snow [code]
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\t45308" ); // Past blowing snow [code]
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\t45309");  // Whiteout yes/no [y/n]
    }
    else
    {
        s_SYNOPDataHeader.append( "Station\tDate/Time\tLatitude\tLongitude" );

        if ( P[1] == 1 )  s_SYNOPDataHeader.append( "\tCloud base height [code]" );
        if ( P[2] == 1 )  s_SYNOPDataHeader.append( "\tHorizontal visibility [code]" );
        if ( P[3] == 1 )  s_SYNOPDataHeader.append( "\tWind direction [deg]" );
        if ( P[4] == 1 )  s_SYNOPDataHeader.append( "\tWind speed [m/sec]" );
        if ( P[5] == 1 )  s_SYNOPDataHeader.append( "\tTemperature, air [deg C]" );
        if ( P[6] == 1 )  s_SYNOPDataHeader.append( "\tDew/frost point [deg C]" );
        if ( P[7] == 1 )  s_SYNOPDataHeader.append( "\tPressure, atmospheric [hPa]" );
        if ( P[8] == 1 )  s_SYNOPDataHeader.append( "\tCharacteristic of barometric tendency [code]" );
        if ( P[9] == 1 )  s_SYNOPDataHeader.append( "\tAmount of barometric tendency [hPa]" );
        if ( P[10] == 1 ) s_SYNOPDataHeader.append( "\tPresent weather [code]" );
        if ( P[11] == 1 ) s_SYNOPDataHeader.append( "\tPast weather1 [code]" );
        if ( P[12] == 1 ) s_SYNOPDataHeader.append( "\tPast weather2 [code]" );
        if ( P[13] == 1 ) s_SYNOPDataHeader.append( "\tLow cloud [code]" );
        if ( P[14] == 1 ) s_SYNOPDataHeader.append( "\tMiddle cloud [code]" );
        if ( P[15] == 1 ) s_SYNOPDataHeader.append( "\tHigh cloud [code]" );
        if ( P[16] == 1 ) s_SYNOPDataHeader.append( "\tTotal cloud amount" );
        if ( P[17] == 1 ) s_SYNOPDataHeader.append( "\tLow/middle cloud amount [code]" );
        if ( P[18] == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air, maximum [deg C]" );
        if ( P[19] == 1 ) s_SYNOPDataHeader.append( "\tTemperature, air, minimum [deg C]" );
        if ( P[20] == 1 ) s_SYNOPDataHeader.append( "\tPresent blowing snow [code]" );
        if ( P[21] == 1 ) s_SYNOPDataHeader.append( "\tPast blowing snow [code]" );
        if ( P[22] == 1 ) s_SYNOPDataHeader.append( "\tWhiteout yes/no [y/n]" );
    }

    return( s_SYNOPDataHeader );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// LIN

QString MainWindow::buildSYNOPDataOutputStr1( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int				i_Day		= 0;
    int				i_Hour		= 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr.simplified();

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	= InputStr.mid( 0, 2 ).toInt();
    i_Hour	= InputStr.mid( 2, 2 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Hour*3600 );

// ***********************************************************************************************************************

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

    OutputStr.append( "\t" + InputStr );

    return( OutputStr );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// FUA, ISH, MNM, SAP, SYO, TAT, NYA

QString MainWindow::buildSYNOPDataOutputStr2( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int				i_Day		= 0;
    int             i_Minute    = 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr;

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	 = InputStr.mid( 1, 2 ).toInt();
    i_Minute = InputStr.mid( 4, 4 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Minute*60 );

// ***********************************************************************************************************************

    if ( b_Import == false )
        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
    else
        OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

    if ( P[1] == 1 )  // Temperature, air [deg C]
    {
        if ( ( InputStr.mid( 9, 5 ) != "-99.0" ) && ( InputStr.mid( 9, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 9, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[2] == 1 ) // Pressure, atmospheric [hPa]
    {
        if ( ( InputStr.mid( 17, 3 ) != "-99" ) && ( InputStr.mid( 15, 5 ).toInt() > -90 ) )
            OutputStr.append( "\t" + InputStr.mid( 15, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[3] == 1 ) // Dew/frost point [deg C]
    {
        if ( ( InputStr.mid( 21, 5 ) != "-99.0" ) && ( InputStr.mid( 21, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 21, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[4] == 1 ) // Wind direction [deg]
    {
        if ( ( InputStr.mid( 27, 3 ) != "-99" ) && ( InputStr.mid( 27, 3 ).toInt() > -90 ) )
            OutputStr.append( "\t" + InputStr.mid( 27, 3 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[5] == 1 ) // Wind speed [m/sec]
    {
        if ( ( InputStr.mid( 31, 3 ) != "-99" ) && ( InputStr.mid( 31, 3 ).toInt() > -90 ) )
            OutputStr.append( "\t" + InputStr.mid( 31, 3 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[6] == 1 ) // Past weather1 [code]
    {
        if ( InputStr.mid( 35, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 35, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[7] == 1 ) // Present weather [code]
    {
        if ( InputStr.mid( 37, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.mid( 37, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[8] == 1 ) // Total cloud amount [code]
    {
        if ( InputStr.mid( 40, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 40, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[9] == 1 ) // Low/middle cloud amount [code]
    {
        if ( InputStr.mid( 42, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 42, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[10] == 1 ) // Low cloud [code]
    {
        if ( InputStr.mid( 44, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 44, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[11] == 1 ) // Middle cloud [code]
    {
        if ( InputStr.mid( 46, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 46, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[12] == 1 ) // High cloud [code]
    {
        if ( InputStr.mid( 48, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 48, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[13] == 1 ) // Cloud base height [code]
    {
        if ( InputStr.mid( 50, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 50, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[14] == 1 )
    {
        if ( InputStr.mid( 52, 1 ) != "/" ) // Present blowing snow [code]
            OutputStr.append( "\t" + InputStr.mid( 52, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[15] == 1 ) // Past blowing snow [code]
    {
        if ( InputStr.mid( 53, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 53, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[16] == 1 ) // Horizontal visibility [code]
    {
        if ( InputStr.mid( 55, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.mid( 55, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    OutputStr.replace( "/", "" );
    OutputStr.replace( " ", "" );

    return( OutputStr );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// TAM

QString MainWindow::buildSYNOPDataOutputStr3( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int             i           = 2;

    int				i_Day		= 0;
    int				i_Hour		= 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr.simplified();

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	= InputStr.mid( 0, 2 ).toInt();
    i_Hour	= InputStr.mid( 2, 2 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Hour*3600 );

// ***********************************************************************************************************************

    if ( b_Import == false )
        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
    else
        OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

    if ( b_Import == false ) // IIiii
        OutputStr.append( "\t" + InputStr.section( " ", 1, 1 ) );

    // iRixhVV

    if ( P[1] == 1 ) // Cloud base height [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 2, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[2] == 1 ) // Horizontal visibility [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 3, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    // Nddff

    if ( i_Year < 2008 ) // 2000-2007
        i++;

    if ( P[3] == 1 ) // Total cloud amount [code]
    {
        if ( InputStr.section( " ", i, i ).left( 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).left( 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[4] == 1 ) // Wind direction [deg]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 2 ) != "//" )
            OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", i, i ).mid( 1, 2 ).toInt()*10 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[5] == 1 ) // Wind speed [m/sec]
    {
        if ( InputStr.section( " ", i, i).mid( 3, 2 ) != "//" )
            OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", i, i ).mid( 3, 2 ).toInt() ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 1SnTTT

    if ( P[6] == 1 ) // Temperature, air [deg C]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 3 ) != "///" )
        {
            float TTT = InputStr.section( " ", i, i ).mid( 2, 3 ).toFloat()/10.;

            if ( InputStr.section( " ", i, i ).mid( 1, 1 ).toInt() == 1 )
                TTT = -1*TTT;

            if ( TTT > -90 )
                OutputStr.append( QString( "\t%1" ).arg( TTT ) );
            else
                OutputStr.append( "\t" );
        }
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 2SnTdTdTd

    if ( P[7] == 1 ) // Dew/frost point [deg C]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 3 ) != "///" )
        {
            float TdTdTd = InputStr.section( " ", i, i ).mid( 2, 3 ).toFloat()/10.;

            if ( InputStr.section( " ", i, i ).mid( 1, 1 ).toInt() == 1 )
                TdTdTd = -1*TdTdTd;

            if ( TdTdTd > -90 )
                OutputStr.append( QString( "\t%1" ).arg( TdTdTd ) );
            else
                OutputStr.append( "\t" );
        }
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 3P0P0P0

    if ( P[8] == 1 ) // Station Pressure [hPa]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 4 ) != "////" )
        {
            if ( InputStr.section( " ", i, i ).mid( 1, 4 ).toFloat()/10. <= 200. )
                OutputStr.append( QString( "\t%1" ).arg( 1000. + InputStr.section( " ", i, i ).mid( 1, 4 ).toFloat()/10. ) );
            else
                OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", i, i ).mid( 1, 4 ).toFloat()/10. ) );
        }
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 4PPPP

    if ( P[9] == 1 ) // Geopotential height [m]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 4 ) != "////" )
            OutputStr.append( QString( "\t%1" ).arg( calcGeopotentialHeight( InputStr.section( " ", i, i ) ) ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 7wwW1W2

    if ( P[10] == 1 ) // Present weather [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 2 ) != "//" )
            OutputStr.append( QString( "\t%1" ).arg( InputStr.section( " ", i, i ).mid( 1, 2 ).toInt() ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[11] == 1 ) // Past weather 1 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 3, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[12] == 1 ) // Past weather 2 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 4, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 4, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 8NhClCmCh

    if ( P[13] == 1 ) // Low/middle cloud amount [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 1, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[14] == 1 ) // Low cloud [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 2, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[15] == 1 ) // Middle cloud [code]
    {
        if ( InputStr.section( " ", i, i).mid( 3, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[16] == 1 ) // High cloud [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 4, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 4, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

//                      333

//                      InputStr.section( " ", 10, 10 ) = "333";

    i++;

    // 8NsChshs

    if ( P[17] == 1 ) // Ns 1 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 1, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[18] == 1 ) // C 1 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 2, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[19] == 1 ) // hshs 1 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 3, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 8NsChshs

    if ( P[20] == 1 ) // Ns 2 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 1, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[21] == 1 ) // C 2 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 2, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[22] == 1 ) // hshs 2 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 3, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    i++;

    // 8NsChshs

    if ( P[23] == 1 ) // Ns 3 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 1, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 1, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[24] == 1 ) // C 3 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 2, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 2, 1 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[25] == 1 ) // hshs 3 [code]
    {
        if ( InputStr.section( " ", i, i ).mid( 3, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.section( " ", i, i ).mid( 3, 2 ) );
        else
            OutputStr.append( "\t" );
    }

    OutputStr.replace( "/", "" );

    OutputStr.append( "\t" + InputStr );

    return( OutputStr );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// PAY

QString MainWindow::buildSYNOPDataOutputStr4( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int				i_Day		= 0;
    int				i_Hour		= 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr.simplified();

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	= InputStr.mid( 0, 2 ).toInt();
    i_Hour	= InputStr.mid( 2, 2 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Hour*3600 );

// ***********************************************************************************************************************

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

    OutputStr.append( "\t" + InputStr );

    return( OutputStr );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// SON

QString MainWindow::buildSYNOPDataOutputStr5( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int				i_Day		= 0;
    int				i_Hour		= 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr.simplified();

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	= InputStr.mid( 0, 2 ).toInt();
    i_Hour	= InputStr.mid( 2, 2 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Hour*3600 );

// ***********************************************************************************************************************

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

    OutputStr.append( "\t" + InputStr );

    return( OutputStr );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************
// GVN, 2016-05

QString MainWindow::buildSYNOPDataOutputStr6( const QString s_EventLabel, const QString DataStr, int *P, const int i_Year, const int i_Month, const float f_Latitude, const float f_Longitude, const bool b_Import )
{
    int				i_Day		= 0;
    int             i_Minute    = 0;

    QString			OutputStr	= "";
    QString         InputStr    = DataStr;

    QDateTime       dt          = QDateTime().toUTC();

// ***********************************************************************************************************************

    i_Day	 = InputStr.mid( 1, 2 ).toInt();
    i_Minute = InputStr.mid( 4, 4 ).toInt();

    dt.setDate( QDate( i_Year, i_Month, i_Day ) );
    dt.setTime( QTime( 0, 0, 0 ) );
    dt = dt.addSecs( i_Minute*60 );

// ***********************************************************************************************************************

    if ( b_Import == false )
        OutputStr = s_EventLabel + "\t" + dt.toString( "yyyy-MM-ddThh:mm" ) + "\t" + num2str( f_Latitude ) + "\t" + num2str( f_Longitude ); // Station Date/Time Latitude Longitude
    else
        OutputStr = dt.toString( "yyyy-MM-ddThh:mm" ); // Date/Time

    if ( P[1] == 1 )
    {
        if ( InputStr.mid( 9, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 9, 1 ) );	// Cloud base height [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[2] == 1 )
    {
        if ( InputStr.mid( 11, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.mid( 11, 2 ) );	// Horizontal visibility [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[3] == 1 ) // Wind direction [deg]
    {
        if ( ( InputStr.mid( 14, 3 ) != "-99" ) && ( InputStr.mid( 14, 3 ).toInt() > -90 ) )
            OutputStr.append( "\t" + InputStr.mid( 14, 3 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[4] == 1 ) // Wind speed [m/sec]
    {
        if ( ( InputStr.mid( 18, 5 ) != "-99.0" ) && ( InputStr.mid( 18, 5 ).toFloat() > -90.0 ) )
            OutputStr.append( "\t" + InputStr.mid( 18, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[5] == 1 )  // Temperature, air [deg C]
    {
        if ( ( InputStr.mid( 24, 5 ) != "-99.0" ) && ( InputStr.mid( 24, 5 ).toFloat() > -90.0 ) )
            OutputStr.append( "\t" + InputStr.mid( 24, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[6] == 1 ) // Dew/frost point [deg C]
    {
        if ( ( InputStr.mid( 30, 5 ) != "-99.0" ) && ( InputStr.mid( 30, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 30, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[7] == 1 ) // Pressure, atmospheric [hPa]
    {
        if ( ( InputStr.mid( 37, 5 ) != "-99.0" ) && ( InputStr.mid( 36, 6 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 36, 6 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[8] == 1 )
    {
        if ( InputStr.mid( 43, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 43, 1 ) );	// Characteristic of barometric tendency [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[9] == 1 )
    {
        if ( ( InputStr.mid( 45, 5 ) != "-99.0" ) && ( InputStr.mid( 45, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 45, 5 ) );	// Amount of barometric tendency [hPa]
        else
            OutputStr.append( "\t" );
    }

    if ( P[10] == 1 )
    {
        if ( InputStr.mid( 51, 2 ) != "//" )
            OutputStr.append( "\t" + InputStr.mid( 51, 2 ) );	// Present weather [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[11] == 1 )
    {
        if ( InputStr.mid( 54, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 54, 1 ) );	// Past weather1 [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[12] == 1 )
    {
        if ( InputStr.mid( 56, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 56, 1 ) );	// Past weather2 [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[13] == 1 )
    {
        if ( InputStr.mid( 58, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 58, 1 ) );	// Low cloud [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[14] == 1 )
    {
        if ( InputStr.mid( 60, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 60, 1 ) );	// Middle cloud [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[15] == 1 )
    {
        if ( InputStr.mid( 62, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 62, 1 ) );	// High cloud [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[16] == 1 )
    {
        if ( InputStr.mid( 64, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 64, 1 ) );	// Total cloud amount [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[17] == 1 )
    {
        if ( InputStr.mid( 66, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 66, 1 ) );	// Low/middle cloud amount [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[18] == 1 )  // Temperature, air, maximum [deg C]
    {
        if ( ( InputStr.mid( 68, 5 ) != "-99.0" ) && ( InputStr.mid( 68, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 68, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[19] == 1 )  // Temperature, air, minimum [deg C]
    {
        if ( ( InputStr.mid( 74, 5 ) != "-99.0" ) && ( InputStr.mid( 74, 5 ).toFloat() > -90. ) )
            OutputStr.append( "\t" + InputStr.mid( 74, 5 ) );
        else
            OutputStr.append( "\t" );
    }

    if ( P[20] == 1 )
    {
        if ( InputStr.mid( 80, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 80, 1 ) );	// Present blowing snow [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[21] == 1 )
    {
        if ( InputStr.mid( 82, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 82, 1 ) );	// Past blowing snow [code]
        else
            OutputStr.append( "\t" );
    }

    if ( P[22] == 1 )
    {
        if ( InputStr.mid( 84, 1 ) != "/" )
            OutputStr.append( "\t" + InputStr.mid( 84, 1 ) );	// Whiteout yes/no [y/n]
        else
            OutputStr.append( "\t" );
    }

    OutputStr.replace( "/", "" );
    OutputStr.replace( " ", "" );

    return( OutputStr );
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
int MainWindow::SYNOPConverter( const bool b_Import, const QString& s_FilenameIn, structStaff *Staff_ptr, structStation *Station_ptr, structReference *Reference_ptr, const bool b_overwriteDataset, structDataset *Dataset_ptr, const int i_NumOfFiles )
{
    int				i_Format		= 0;

    int				i_PIID			= 506;
    int				i_SourceID		= 17;
    int				i_StationNumber	= 0;

    int				i_Month			= 1;
    int				i_Year			= 2000;
    int				i_Day			= 1;

    float           f_Latitude      = 0.;
    float           f_Longitude     = 0.;

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

    QString s_FilenameOut = fi.absolutePath() + "/" + s_EventLabel + "_" + dt.toString( "yyyy-MM" ) + "_1000";

    if ( b_Import == true )
        s_FilenameOut.append( "_imp.txt" );
    else
        s_FilenameOut.append( ".txt" );

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
    {
        resetProgress( i_NumOfFiles );
        fin.close();
        return( -20 );
    }

    QTextStream tout( &fout );

// ***********************************************************************************************************************
// LR1000
// ***********************************************************************************************************************
// build SYNOP data description header

    b_Stop = false;

    if ( b_Import == true )
    {
        if ( b_overwriteDataset == true )
        {
            int i_DatasetID = findDatasetID( QString( "%1_SYNOP_%2" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ), Dataset_ptr );

            if ( i_DatasetID > 0 )
                s_DatasetID = DataSetID( num2str( i_DatasetID ) );
            else
                s_DatasetID = DataSetID( QString( "@%1_SYNOP_%2@" ).arg( s_EventLabel ).arg( dt.toString( "yyyy-MM" ) ) );
        }

        sl_Parameter.clear(); sl_Parameter.append( Parameter( num2str( 1599 ), num2str( i_PIID ), num2str( 43 ), tr( "yyyy-MM-dd'T'HH:mm" ) ) );

        switch ( i_Format )
        {
        case 1: // LIN
            sl_Parameter.append( buildSYNOPDataDescriptionHeader1( P, i_PIID ) );
            break;

        case 2: // FUA, ISH, MNM, SAP, SYO, TAT, NYA
            sl_Parameter.append( buildSYNOPDataDescriptionHeader2( P, i_PIID ) );
            break;

        case 3: // TAM
            sl_Parameter.append( buildSYNOPDataDescriptionHeader3( P, i_PIID, i_Year ) );
            break;

        case 4: // PAY
            sl_Parameter.append( buildSYNOPDataDescriptionHeader4( P, i_PIID ) );
            break;

        case 5: // SON
            sl_Parameter.append( buildSYNOPDataDescriptionHeader5( P, i_PIID ) );
            break;

        case 6: // GVN, 2016-05
            sl_Parameter.append( buildSYNOPDataDescriptionHeader6( P, i_PIID ) );
            break;

        default:
            break;
        }
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
// write data

    while ( ( tin.atEnd() == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) && ( b_Stop == false ) )
    {
        InputStr	= tin.readLine();
        ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

        if ( ( InputStr.startsWith( SearchString1 ) == true ) || ( InputStr.startsWith( SearchString2 ) == true ) )
        {
            // write data table header
            switch ( i_Format )
            {
            case 1: // LIN
                tout << buildSYNOPDataHeader1( P, b_Import ) << eol;
                break;

            case 2: // FUA, ISH, MNM, SAP, SYO, TAT, NYA
                tout << buildSYNOPDataHeader2( P, b_Import ) << eol;
                break;

            case 3: // TAM
                tout << buildSYNOPDataHeader3( P, i_Year, b_Import ) << eol;
                break;

            case 4: // PAY
                tout << buildSYNOPDataHeader4( P, b_Import ) << eol;
                break;

            case 5: // SON
                tout << buildSYNOPDataHeader5( P, b_Import ) << eol;
                break;

            case 6: // GVN
                tout << buildSYNOPDataHeader6( P, b_Import ) << eol;
                break;

            default:
                break;
            }

            while ( ( tin.atEnd() == false ) && ( b_Stop == false ) && ( ui_length != (unsigned int) _APPBREAK_ ) )
            {
                InputStr	= tin.readLine();
                ui_length	= incProgress( i_NumOfFiles, ui_filesize, ui_length, InputStr );

                if ( InputStr.startsWith( "*" ) == false )
                {
                    // write data table
                    switch ( i_Format )
                    {
                    case 1: // LIN
                        tout << buildSYNOPDataOutputStr1( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    case 2: // FUA, ISH, MNM, SAP, SYO, TAT, NYA
                        tout << buildSYNOPDataOutputStr2( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    case 3: // TAM
                        tout << buildSYNOPDataOutputStr3( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    case 4: // PAY
                        tout << buildSYNOPDataOutputStr4( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    case 5: // SON
                        tout << buildSYNOPDataOutputStr5( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    case 6: // GVN, 2016-05
                        tout << buildSYNOPDataOutputStr6( s_EventLabel, InputStr, P, i_Year, i_Month, f_Latitude, f_Longitude, b_Import ) << eol;
                        break;

                    default:
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

    case 6: // 700 hPa
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
// 2003-08-02

/*! @brief Steuerung des Expanded Measurements Converters, LR 1000 */

void MainWindow::doSYNOPConverter( const bool b_Import )
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        if ( b_Import == true )
            readBsrnReferenceIDs( false );

        initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "SYNOP converter working..." ) );

        while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress != _APPBREAK_ ) )
        {
            err = SYNOPConverter( b_Import, gsl_FilenameList.at( i ), g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

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
