/****************************************************************
**
** Preferences
** 2008-03-21, Dr. Rainer Sieger
** last change: 2016-06-06
**
****************************************************************/

#include "Application.h"
#include "simplecrypt.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 2016-06-06

/*! @brief Sichern der Programmeinstellungen */

void MainWindow::savePreferences()
{
    SimpleCrypt crypto( Q_UINT64_C( 0x07cfffa122768de4f ) ); //some random number

    #if defined(Q_OS_LINUX)
        QSettings settings( getPreferenceFilename(), QSettings::IniFormat );
    #endif

    #if defined(Q_OS_MAC)
        QSettings settings( getPreferenceFilename(), QSettings::NativeFormat );
    #endif

    #if defined(Q_OS_WIN)
        QSettings settings( getPreferenceFilename(), QSettings::IniFormat );
    #endif

    settings.beginGroup( QCoreApplication::applicationName() );

    settings.beginGroup( "GeneralSettings" );
    settings.setValue( "NumOfProgramStarts", gi_NumOfProgramStarts );

    settings.setValue( "WindowX", x() );
    settings.setValue( "WindowY", y() );
    settings.setValue( "WindowHeight", height() );
    settings.setValue( "WindowWidth", width() );

    settings.setValue( "DialogX", posDialog.x() );
    settings.setValue( "DialogY", posDialog.y() );

    settings.setValue( "Path", gs_Path );
    settings.endGroup();

    // ConcatenateOptionsDialog
    settings.beginGroup( "ConcatenateOptionsDialog" );
    settings.setValue( "SkipNFirstLines", gi_SkipNFirstLines );
    settings.setValue( "DeleteOriginalFiles", gb_DeleteOriginalFiles );
    settings.endGroup();

    settings.beginGroup( "Flags" );

    // Station flags
    QString s_StationFlags = "";
    for ( int i=1; i<=MAX_NUM_OF_STATIONS; ++i )
    {
        if ( gb_Station[i] == true )
            s_StationFlags.append( "1," );
        else
            s_StationFlags.append( "0," );
    }
    settings.setValue( "StationFlags", s_StationFlags );

    // Month flags
    QString s_MonthFlags = "";
    for ( int i=1; i<=MAX_NUM_OF_MONTHS; ++i )
    {
        if ( gb_Month[i] == true )
            s_MonthFlags.append( "1," );
        else
            s_MonthFlags.append( "0," );
    }
    settings.setValue( "MonthFlags", s_MonthFlags );

    // Year flags
    QString s_YearFlags = "";
    for ( int i=1; i<=MAX_NUM_OF_YEARS; ++i )
    {
        if ( gb_Year[i] == true )
            s_YearFlags.append( "1," );
        else
            s_YearFlags.append( "0," );
    }
    settings.setValue( "YearFlags", s_YearFlags );

    settings.endGroup();

    settings.beginGroup( "DownloadManagerDialog" );
    settings.setValue( "FTPServer", crypto.encryptToString( gs_FTPServer ) );
    settings.setValue( "User", crypto.encryptToString( gs_User ) );
    settings.setValue( "Password", crypto.encryptToString( gs_Password ) );
    settings.setValue( "DownloadPath", gs_DownloadPath );
    settings.setValue( "DecompressFiles", gb_DecompressFiles );
    settings.setValue( "CheckFiles", gb_CheckFiles );
    settings.setValue( "CheckAvailability", gb_CheckAvailability );
    settings.endGroup();

    settings.beginGroup( "FormatedTextfileOptionsDialog" );
    settings.setValue( "FieldDelimiter", gi_FieldDelimiter );
    settings.setValue( "MissingValue", gs_MissingValue );
    settings.setValue( "FieldAlignment", gi_FieldAlignment );
    settings.setValue( "FieldWidth", gi_FieldWidth );
    settings.endGroup();

    settings.beginGroup( "ReferenceID" );
    settings.setValue( "LastRefercenceID", gi_lastReferenceID );
    settings.endGroup();

    // Quality check options
    settings.beginGroup( "QualityCheckOptions" );
    settings.setValue( "CheckPhysicallyPossibleLimits", gb_CheckPhysicallyPossibleLimits );
    settings.setValue( "CheckExtremelyRareLimits", gb_CheckExtremelyRareLimits );
    settings.setValue( "CheckComparisons", gb_CheckComparisons );
    settings.setValue( "AuxiliaryDataAlgorithm", gs_AuxiliaryDataAlgorithm );
    settings.setValue( "OutputCodes", gb_OutputCodes );
    settings.setValue( "OutputCleanedValues", gb_OutputCleanedValues );
    settings.setValue( "OutputOriginalValues", gb_OutputOriginalValues );
    settings.setValue( "OutputAuxiliaryData", gb_OutputAuxiliaryData );
    settings.setValue( "OutputOneFile", gb_OutputOneFile );
    settings.endGroup();

    settings.endGroup();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 21.11.2007

/*! @brief Laden der Programmeinstellungen */

void MainWindow::loadPreferences()
{
    SimpleCrypt crypto( Q_UINT64_C( 0x07cfffa122768de4f ) ); //some random number

    #if defined(Q_OS_LINUX)
        gi_Codec = _UTF8_; // UTF-8
        gi_EOL   = _UNIX_;
    #endif

    #if defined(Q_OS_MAC)
        gi_Codec = _APPLEROMAN_; // Mac Roman
        gi_EOL   = _UNIX_;
    #endif

    #if defined(Q_OS_WIN)
        gi_Codec = _LATIN1_; // Latin-1 = ISO 8859-1
        gi_EOL   = _WIN_;
    #endif

// **********************************************************************************************
    #if defined(Q_OS_LINUX)
        QSettings settings( getPreferenceFilename(), QSettings::IniFormat );
    #endif

    #if defined(Q_OS_MAC)
        QSettings settings( getPreferenceFilename(), QSettings::NativeFormat );
    #endif

    #if defined(Q_OS_WIN)
        QSettings settings( getPreferenceFilename(), QSettings::IniFormat );
    #endif

    settings.beginGroup( QCoreApplication::applicationName() );

    settings.beginGroup( "GeneralSettings" );
    gi_NumOfProgramStarts = settings.value( "NumOfProgramStarts", 0 ).toInt();

    posWindow.setX( settings.value( "WindowX", 100 ).toInt() );
    posWindow.setY( settings.value( "WindowY", 100 ).toInt() );
    sizeWindow.setHeight( settings.value( "WindowHeight", 250 ).toInt() );
    sizeWindow.setWidth( settings.value( "WindowWidth", 600 ).toInt() );

    posDialog.setX( settings.value( "DialogX", 110 ).toInt() );
    posDialog.setY( settings.value( "DialogY", 110 ).toInt() );

    gs_Path = settings.value( "Path", getDocumentDir() ).toString();
    settings.endGroup();

    settings.beginGroup( "ConcatenateOptionsDialog" );
    gi_SkipNFirstLines      = settings.value( "SkipNFirstLines", 0 ).toInt();
    gb_DeleteOriginalFiles  = settings.value( "DeleteOriginalFiles", true  ).toBool();
    settings.endGroup();

    settings.beginGroup( "Flags" );

    // Station flags
    QString s_StationFlags = settings.value( "StationFlags", "" ).toString();

    if ( s_StationFlags.isEmpty() == true )
    {
        for ( int i=1; i<MAX_NUM_OF_STATIONS; ++i )
            s_StationFlags.append( "0," );
        s_StationFlags.append( "0" );
    }

    for ( int i=0; i<MAX_NUM_OF_STATIONS; ++i )
    {
        if ( s_StationFlags.section( ",", i, i ) == "0" )
            gb_Station[i+1] = false;
        else
            gb_Station[i+1] = true;
    }

// Month flags
    QString s_MonthFlags = settings.value( "MonthFlags", "" ).toString();

    if ( s_MonthFlags.isEmpty() == true )
    {
        for ( int i=1; i<MAX_NUM_OF_MONTHS; ++i )
            s_MonthFlags.append( "0," );
        s_MonthFlags.append( "0" );
    }

    for ( int i=0; i<MAX_NUM_OF_MONTHS; ++i )
    {
        if ( s_MonthFlags.section( ",", i, i ) == "0" )
            gb_Month[i+1] = false;
        else
            gb_Month[i+1] = true;
    }

// Year flags
    QString s_YearFlags = settings.value( "YearFlags", "" ).toString();

    if ( s_YearFlags.isEmpty() == true )
    {
        for ( int i=1; i<MAX_NUM_OF_YEARS; ++i )
            s_YearFlags.append( "0," );
        s_YearFlags.append( "0" );
    }

    for ( int i=0; i<MAX_NUM_OF_YEARS; ++i )
    {
        if ( s_YearFlags.section( ",", i, i ) == "0" )
            gb_Year[i+1] = false;
        else
            gb_Year[i+1] = true;
    }

    settings.endGroup();

    settings.beginGroup( "DownloadManagerDialog" );
    gs_FTPServer         = crypto.decryptToString( settings.value( "FTPServer", "ftp.bsrn.awi.de" ).toString() );
    gs_User              = crypto.decryptToString( settings.value( "User", "" ).toString() );
    gs_Password          = crypto.decryptToString( settings.value( "Password", "" ).toString() );
    gs_DownloadPath      = settings.value( "DownloadPath", getDocumentDir() ).toString();
    gb_DecompressFiles   = settings.value( "DecompressFiles", true ).toBool();
    gb_CheckFiles        = settings.value( "CheckFiles", true ).toBool();
    gb_CheckAvailability = settings.value( "CheckAvailability", false ).toBool();
    settings.endGroup();

    settings.beginGroup( "FormatedTextfileOptionsDialog" );
    gs_MissingValue     = settings.value( "MissingValue", "-9999" ).toString();
    gi_FieldDelimiter   = settings.value( "FieldDelimiter", _TABSTOP_ ).toInt();
    gi_FieldAlignment   = settings.value( "FieldAlignment", QTextStream::AlignLeft ).toInt();
    gi_FieldWidth       = settings.value( "FieldWidth", 20 ).toInt();
    settings.endGroup();

    settings.beginGroup( "ReferenceID" );
    gi_lastReferenceID = settings.value( "LastRefercenceID", 70000 ).toInt();
    settings.endGroup();

    // Quality check options
    settings.beginGroup( "QualityCheckOptions" );
    gs_AuxiliaryDataAlgorithm           = settings.value( "AuxiliaryDataAlgorithm", "Solpos without Refraction" ).toString();
    gb_CheckPhysicallyPossibleLimits    = settings.value( "CheckPhysicallyPossibleLimits", true ).toBool();
    gb_CheckExtremelyRareLimits         = settings.value( "CheckExtremelyRareLimits", true ).toBool();
    gb_CheckComparisons                 = settings.value( "CheckComparisons", true ).toBool();
    gb_OutputCodes                      = settings.value( "OutputCodes", true ).toBool();
    gb_OutputCleanedValues              = settings.value( "OutputCleanedValues", false ).toBool();
    gb_OutputOriginalValues             = settings.value( "OutputOriginalValues", false ).toBool();
    gb_OutputAuxiliaryData              = settings.value( "OutputAuxiliaryData", false ).toBool();
    gb_OutputOneFile                    = settings.value( "OutputOneFile", false ).toBool();
    settings.endGroup();

    settings.endGroup();
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Ermitteln des Dateinamens fuer die Programmeinstellungen.
*   @return Name der Datei fuer die Programmeinstellungen. Dateiname ist gleich "_ERROR_",
*   wenn Betriebssystem ungleich Windows, Mac OS oder Unix ist.
*/

QString MainWindow::getPreferenceFilename()
{
    QString s_PrefFilename = "";

    #if defined(Q_OS_LINUX)
        s_PrefFilename = QDir::homePath() + QLatin1String( "/.config/" ) + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/" + QCoreApplication::applicationName() + QLatin1String( ".conf" );
    #endif

    #if defined(Q_OS_MAC)
        s_PrefFilename = QDir::homePath() + QLatin1String( "/Library/Preferences/" ) + QCoreApplication::organizationDomain().section( ".", 1, 1 ) + "." + QCoreApplication::organizationDomain().section( ".", 0, 0 ) + "." + QCoreApplication::applicationName() + QLatin1String( ".plist" );
    #endif

    #if defined(Q_OS_WIN)
        if ( QCoreApplication::applicationName().toLower().endsWith( "portable" ) )
        {
            s_PrefFilename = QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName() + QLatin1String( ".ini" );
        }
        else
        {
            QSettings cfg( QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName() );
            s_PrefFilename = QFileInfo( cfg.fileName() ).absolutePath() + "/" + QCoreApplication::applicationName() + "/" + QCoreApplication::applicationName() + QLatin1String( ".ini" );
        }
    #endif

    return( s_PrefFilename );
}
