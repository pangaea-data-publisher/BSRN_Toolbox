/* 2007-11-07                 */
/* Dr. Rainer Sieger          */

#include <QtGui>

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2007-11-08

/*! @brief Schreibt die Status-Datei
*
*   @param FilenameIn Name der Input-Datei
*   @param sl_ExportFilenames Liste der Exportdateinamen
*   @param s_LR Name des Logocal Records
*
*   @return Fehlercode
*/

int MainWindow::writeStatusFile( const QString& FilenameIn, const QStringList sl_ExportFilenames, const QString& s_LR )
{
	int				i				= 0;
	int				n				= 0;
	int				k				= 0;

	int				j				= 0;
	int				i_Year			= 0;

	QDate			dt				= QDate();

	QString			SearchStr1		= "";
	QString			SearchStr2		= "";

	int				ASP[MAX_NUM_OF_YEAR+1];
	int				BAR[MAX_NUM_OF_YEAR+1];
	int				BER[MAX_NUM_OF_YEAR+1];
	int				BIL[MAX_NUM_OF_YEAR+1];
	int				BON[MAX_NUM_OF_YEAR+1];
	int				BOS[MAX_NUM_OF_YEAR+1];
	int				BOU[MAX_NUM_OF_YEAR+1];
	int				CAB[MAX_NUM_OF_YEAR+1];
	int				CAM[MAX_NUM_OF_YEAR+1];
	int				CAR[MAX_NUM_OF_YEAR+1];
	int				CLH[MAX_NUM_OF_YEAR+1];
	int				DAA[MAX_NUM_OF_YEAR+1];
	int				DAR[MAX_NUM_OF_YEAR+1];
	int				DRA[MAX_NUM_OF_YEAR+1];
	int				E13[MAX_NUM_OF_YEAR+1];
	int				FLO[MAX_NUM_OF_YEAR+1];
	int				FPE[MAX_NUM_OF_YEAR+1];
	int				GCR[MAX_NUM_OF_YEAR+1];
	int				GVN[MAX_NUM_OF_YEAR+1];
	int				ILO[MAX_NUM_OF_YEAR+1];
	int				KWA[MAX_NUM_OF_YEAR+1];
	int				LAU[MAX_NUM_OF_YEAR+1];
	int				LER[MAX_NUM_OF_YEAR+1];
	int				LIN[MAX_NUM_OF_YEAR+1];
	int				MAN[MAX_NUM_OF_YEAR+1];
	int				NAU[MAX_NUM_OF_YEAR+1];
	int				NYA[MAX_NUM_OF_YEAR+1];
	int				PAL[MAX_NUM_OF_YEAR+1];
	int				PAY[MAX_NUM_OF_YEAR+1];
	int				PSU[MAX_NUM_OF_YEAR+1];
	int				REG[MAX_NUM_OF_YEAR+1];
	int				SBO[MAX_NUM_OF_YEAR+1];
	int				SOV[MAX_NUM_OF_YEAR+1];
	int				SPO[MAX_NUM_OF_YEAR+1];
	int				SYO[MAX_NUM_OF_YEAR+1];
	int				TAM[MAX_NUM_OF_YEAR+1];
	int				TAT[MAX_NUM_OF_YEAR+1];
	int				TOR[MAX_NUM_OF_YEAR+1];
	int				XIA[MAX_NUM_OF_YEAR+1];

	QString			FilenameOut = "";

// ***********************************************************************************************************************

	QFileInfo fi( FilenameIn );

	if ( s_LR == "LR0100" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR0100.txt";
		SearchStr1	= "radiation_19";
		SearchStr2	= "radiation_20";
	}

	if ( s_LR == "LR0500" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR0500.txt";
		SearchStr1	= "Ultra-violet";
		SearchStr2	= "ultra-violet";
	}

	if ( s_LR == "LR1000" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR1000.txt";
		SearchStr1	= "surface_synop";
		SearchStr2	= "SYNOP";
	}

	if ( s_LR == "LR1100" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR1100.txt";
		SearchStr1	= "Radiosonde";
		SearchStr2	= "radiosonde";
	}

	if ( s_LR == "LR1200" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR1200.txt";
		SearchStr1	= "Ozone";
		SearchStr2	= "ozone";
	}

	if ( s_LR == "LR1300" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR1300.txt";
		SearchStr1	= "Expanded";
		SearchStr2	= "expanded";
	}

	if ( s_LR == "LR3010" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR3010.txt";
		SearchStr1	= "10m";
		SearchStr2	= "_10m";
	}

	if ( s_LR == "LR3030" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR3030.txt";
		SearchStr1	= "30m";
		SearchStr2	= "_30m";
	}

	if ( s_LR == "LR3300" )
	{
		FilenameOut = fi.absolutePath() + "/BSRN_LR3300.txt";
		SearchStr1	= "300m";
		SearchStr2	= "_300m";
	}

// ***********************************************************************************************************************

	QFile fout( FilenameOut );

	if ( fout.open( QIODevice::WriteOnly | QIODevice::Text ) == FALSE )
		return( -20 );

	QTextStream tout( &fout ); // tout.setEncoding( QTextStream::Latin1 );

	memset( ASP, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BAR, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BER, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BIL, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BON, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BOS, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( BOU, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( CAB, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( CAM, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( CAR, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( CLH, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( DAA, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( DAR, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( DRA, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( E13, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( FLO, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( FPE, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( GCR, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( GVN, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( ILO, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( KWA, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( LAU, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( LER, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( LIN, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( MAN, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( NAU, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( NYA, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( PAL, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( PAY, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( PSU, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( REG, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( SBO, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( SOV, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( SPO, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( SYO, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( TAM, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( TAT, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( TOR, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );
	memset( XIA, 0, (MAX_NUM_OF_YEAR+1)*sizeof( int ) );

//---------------------------------------------------------------------------------------------

	n = sl_ExportFilenames.count();

	initProgress( n );

//---------------------------------------------------------------------------------------------

	for ( i=0; i<n; i++ )
	{
		i_Year = sl_ExportFilenames[i].right(4).toInt();

		if ( ( i_Year >= 1992 ) && ( i_Year <= 1992+MAX_NUM_OF_YEAR ) )
		{
			incProgress( ++k );

			if ( ( sl_ExportFilenames[i].contains( SearchStr1 ) == 1 ) || ( sl_ExportFilenames[i].contains( SearchStr2 ) == 1 ) )
			{
				if ( sl_ExportFilenames[i].startsWith( "ASP" ) == TRUE ) ASP[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BAR" ) == TRUE ) BAR[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BER" ) == TRUE ) BER[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BIL" ) == TRUE ) BIL[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BON" ) == TRUE ) BON[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BOS" ) == TRUE ) BOS[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "BOU" ) == TRUE ) BOU[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "CAB" ) == TRUE ) CAB[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "CAM" ) == TRUE ) CAM[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "CAR" ) == TRUE ) CAR[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "CLH" ) == TRUE ) CLH[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "DAA" ) == TRUE ) DAA[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "DAR" ) == TRUE ) DAR[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "DRA" ) == TRUE ) DRA[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "E13" ) == TRUE ) E13[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "FLO" ) == TRUE ) FLO[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "FPE" ) == TRUE ) FPE[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "GCR" ) == TRUE ) GCR[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "GVN" ) == TRUE ) GVN[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "ILO" ) == TRUE ) ILO[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "KWA" ) == TRUE ) KWA[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "LAU" ) == TRUE ) LAU[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "LER" ) == TRUE ) LER[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "LIN" ) == TRUE ) LIN[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "MAN" ) == TRUE ) MAN[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "NAU" ) == TRUE ) NAU[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "NYA" ) == TRUE ) NYA[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "PAL" ) == TRUE ) PAL[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "PAY" ) == TRUE ) PAY[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "PSU" ) == TRUE ) PSU[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "REG" ) == TRUE ) REG[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "SBO" ) == TRUE ) SBO[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "SOV" ) == TRUE ) SOV[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "SPO" ) == TRUE ) SPO[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "SYO" ) == TRUE ) SYO[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "TAM" ) == TRUE ) TAM[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "TAT" ) == TRUE ) TAT[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "TOR" ) == TRUE ) TOR[i_Year-1991]++;
				if ( sl_ExportFilenames[i].startsWith( "XIA" ) == TRUE ) XIA[i_Year-1991]++;
			}
		}
	}

//---------------------------------------------------------------------------------------------------

	i_Year = dt.currentDate().year()-1;

	tout << "Station\tShort name";

	for ( j=1992; j<=i_Year; j++ )
		tout << "\t" << j;

	tout << endl;

	tout << "Alice Springs\tASP"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ){ if ( ASP[i] > 0 ) tout << "\t" << ASP[i]; else tout << "\t"; } tout << endl;
	tout << "Barrow\tBAR"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BAR[i] > 0 ) tout << "\t" << BAR[i]; else tout << "\t"; } tout << endl;
	tout << "Bermuda\tBER"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BER[i] > 0 ) tout << "\t" << BER[i]; else tout << "\t"; } tout << endl;
	tout << "Billings\tBIL"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BIL[i] > 0 ) tout << "\t" << BIL[i]; else tout << "\t"; } tout << endl;
	tout << "Bondville\tBON"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BON[i] > 0 ) tout << "\t" << BON[i]; else tout << "\t"; } tout << endl;
	tout << "Boulder, SURFRAD\tBOS"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BOS[i] > 0 ) tout << "\t" << BOS[i]; else tout << "\t"; } tout << endl;
	tout << "Boulder\tBOU"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( BOU[i] > 0 ) tout << "\t" << BOU[i]; else tout << "\t"; } tout << endl;
	tout << "Cabauw\tCAB"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( CAB[i] > 0 ) tout << "\t" << CAB[i]; else tout << "\t"; } tout << endl;
	tout << "Camborne\tCAM"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( CAM[i] > 0 ) tout << "\t" << CAM[i]; else tout << "\t"; } tout << endl;
	tout << "Carpentras\tCAR"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( CAR[i] > 0 ) tout << "\t" << CAR[i]; else tout << "\t"; } tout << endl;
	tout << "Chesapeake Light\tCLH"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( CLH[i] > 0 ) tout << "\t" << CLH[i]; else tout << "\t"; } tout << endl;
	tout << "De Aar\tDAA"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( DAA[i] > 0 ) tout << "\t" << DAA[i]; else tout << "\t"; } tout << endl;
	tout << "Darwin\tDAR"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( DAR[i] > 0 ) tout << "\t" << DAR[i]; else tout << "\t"; } tout << endl;
	tout << "Desert Rock\tDRA"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( DRA[i] > 0 ) tout << "\t" << DRA[i]; else tout << "\t"; } tout << endl;
	tout << "S. Great Plains\tE13"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( E13[i] > 0 ) tout << "\t" << E13[i]; else tout << "\t"; } tout << endl;
	tout << "Florianopolis\tFLO"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( FLO[i] > 0 ) tout << "\t" << FLO[i]; else tout << "\t"; } tout << endl;
	tout << "Fort Peck\tFPE"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( FPE[i] > 0 ) tout << "\t" << FPE[i]; else tout << "\t"; } tout << endl;
	tout << "Goodwin Creek\tGCR"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( GCR[i] > 0 ) tout << "\t" << GCR[i]; else tout << "\t"; } tout << endl;
	tout << "Neumayer Station\tGVN"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( GVN[i] > 0 ) tout << "\t" << GVN[i]; else tout << "\t"; } tout << endl;
	tout << "Ilorin\tILO"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( ILO[i] > 0 ) tout << "\t" << ILO[i]; else tout << "\t"; } tout << endl;
	tout << "Kwajalein\tKWA"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( KWA[i] > 0 ) tout << "\t" << KWA[i]; else tout << "\t"; } tout << endl;
	tout << "Lauder\tLAU"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( LAU[i] > 0 ) tout << "\t" << LAU[i]; else tout << "\t"; } tout << endl;
	tout << "Lerwick\tLER"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( LER[i] > 0 ) tout << "\t" << LER[i]; else tout << "\t"; } tout << endl;
	tout << "Lindenberg\tLIN"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( LIN[i] > 0 ) tout << "\t" << LIN[i]; else tout << "\t"; } tout << endl;
	tout << "Momote\tMAN"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( MAN[i] > 0 ) tout << "\t" << MAN[i]; else tout << "\t"; } tout << endl;
	tout << "Nauru Island\tNAU"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( NAU[i] > 0 ) tout << "\t" << NAU[i]; else tout << "\t"; } tout << endl;
	tout << "Ny-Ålesund\tNYA"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( NYA[i] > 0 ) tout << "\t" << NYA[i]; else tout << "\t"; } tout << endl;
	tout << "Palaiseau Cedex\tPAL"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( PAL[i] > 0 ) tout << "\t" << PAL[i]; else tout << "\t"; } tout << endl;
	tout << "Payerne\tPAY"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( PAY[i] > 0 ) tout << "\t" << PAY[i]; else tout << "\t"; } tout << endl;
	tout << "Rock Springs\tPSU"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( PSU[i] > 0 ) tout << "\t" << PSU[i]; else tout << "\t"; } tout << endl;
	tout << "Regina\tREG"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( REG[i] > 0 ) tout << "\t" << REG[i]; else tout << "\t"; } tout << endl;
	tout << "Sede Boqer\tSBO"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( SBO[i] > 0 ) tout << "\t" << SBO[i]; else tout << "\t"; } tout << endl;
	tout << "Solar Village\tSOV"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( SOV[i] > 0 ) tout << "\t" << SOV[i]; else tout << "\t"; } tout << endl;
	tout << "South Pole\tSPO"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( SPO[i] > 0 ) tout << "\t" << SPO[i]; else tout << "\t"; } tout << endl;
	tout << "Syowa\tSYO"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( SYO[i] > 0 ) tout << "\t" << SYO[i]; else tout << "\t"; } tout << endl;
	tout << "Tamanrasset\tTAM"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( TAM[i] > 0 ) tout << "\t" << TAM[i]; else tout << "\t"; } tout << endl;
	tout << "Tateno\tTAT"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( TAT[i] > 0 ) tout << "\t" << TAT[i]; else tout << "\t"; } tout << endl;
	tout << "Toravere\tTOR"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( TOR[i] > 0 ) tout << "\t" << TOR[i]; else tout << "\t"; } tout << endl;
	tout << "Xianghe\tXIA"; for ( i=1; i<=MAX_NUM_OF_YEAR; i++ ) { if ( XIA[i] > 0 ) tout << "\t" << XIA[i]; else tout << "\t"; } tout << endl;

	resetProgress();

	fout.close();

	return( k );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermitteln der Dateinamen und Aufruf der Schreibfunktion fuer die Erstellung der Statusdateien
*
*   @param FilenameIn Name der Inputdatei
*
*   @return Fehlercode
*/

int MainWindow::buildStatusFiles( const QString& FilenameIn )
{
	int				i						= 0;
	int				n						= 0;
	int				err						= NOERROR;
	int				i_ExportFilenameColumn	= -1;
	int				i_NumOfStatusFiles		= 9;
	int				stopProgress			= 0;

	QString			InputStr				= "";
	QString			s_Filename				= "";

	QStringList		sl_ExportFilenames;

// ***********************************************************************************************************************

	QFileInfo fi( FilenameIn );

	if ( ( fi.exists() == FALSE ) || ( fi.suffix().toLower() == "zip" ) || ( fi.suffix().toLower() == "gz" ) )
		return( -1 );

// ***********************************************************************************************************************

	QFile fin( FilenameIn );

	if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == FALSE )
		return( -10 );

	QTextStream tin( &fin ); // tin.setEncoding( QTextStream::Latin1 );

// ***********************************************************************************************************************

	InputStr  = tin.readLine();

	if ( InputStr.contains( "Export filename" ) == FALSE )
	{
		fin.close();
		return( -40 );
	}

// ***********************************************************************************************************************

	n = NumOfSections( InputStr );

	for ( i=0; i<n; i++ )
	{
		if ( InputStr.section( "\t", i, i ) == "Export filename" )
			i_ExportFilenameColumn = i;
	}

// ***********************************************************************************************************************

	initFileProgress( i_NumOfStatusFiles, FilenameIn, tr( "Building status files..." ) );

// ***********************************************************************************************************************

	while ( tin.atEnd() == FALSE )
	{		
		InputStr = tin.readLine();

		if ( InputStr.contains( "horizon" ) == 0 )
		{
			s_Filename = InputStr.section( "\t", i_ExportFilenameColumn, i_ExportFilenameColumn );
			sl_ExportFilenames.append( s_Filename.left( s_Filename.length()-3 ) );
		}
	}

	fin.close();

// ***********************************************************************************************************************

	if ( sl_ExportFilenames.count() == 0 )
	{
		resetFileProgress( i_NumOfStatusFiles );
		err = ERROR;
	}

// ***********************************************************************************************************************

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
		sl_ExportFilenames.sort();

// ***********************************************************************************************************************

	i = 0;

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR0100" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR0100" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR0500" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR0500" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR1000" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR1000" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR1100" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR1100" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR1200" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR1200" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR1300" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR1300" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR3010" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR3010" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR3030" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR3030" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

	if ( ( err == NOERROR ) && ( stopProgress != APPBREAK ) )
	{
		setStatusBar( tr( "File in progress: " ) + QDir::toNativeSeparators( FilenameIn ) + tr( " - building LR3300" ) );
		writeStatusFile( FilenameIn, sl_ExportFilenames, "LR3300" );
		stopProgress = incFileProgress( i_NumOfStatusFiles, ++i );
	}

// ***********************************************************************************************************************

	resetFileProgress( i_NumOfStatusFiles );

	return( NOERROR );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung der Erstellung der Statusdateien */

void MainWindow::doBuildStatusFiles()
{
	int		err				= 0;

// **********************************************************************************************

	clearList( sl_FilenameIn );

	if ( chooseFile() == NOERROR )
		err = buildStatusFiles( sl_FilenameIn.at( 0 ) );

	clearList( sl_FilenameIn );

// **********************************************************************************************

	onError( err );
}

