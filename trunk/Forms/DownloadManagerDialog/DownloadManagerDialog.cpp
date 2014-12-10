#include <QtWidgets>
#include "Application.h"
#include "DownloadManagerDialog.h"

DownloadManagerDialog::DownloadManagerDialog( QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));

    connect(SelectAllStations_pushButton, SIGNAL(clicked()), this, SLOT(SelectAllStations()));
    connect(SelectNoneStation_pushButton, SIGNAL(clicked()), this, SLOT(SelectNoneStation()));
    connect(SelectAllMonth_pushButton, SIGNAL(clicked()), this, SLOT(SelectAllMonth()));
    connect(SelectNoneMonth_pushButton, SIGNAL(clicked()), this, SLOT(SelectNoneMonth()));
    connect(SelectAllYears_pushButton, SIGNAL(clicked()), this, SLOT(SelectAllYears()));
    connect(SelectNoneYear_pushButton, SIGNAL(clicked()), this, SLOT(SelectNoneYear()));
    connect(BrowseDownloadDirectory_pushButton, SIGNAL(clicked()), this, SLOT(BrowseDownloadDirectory()));
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::BrowseDownloadDirectory()
{
    QString s_DownloadPath	= "";
    QString s_Path			= DownloadDirectory_lineEdit->text();

    QFileInfo fi( s_Path );

    if ( fi.exists() == false )
        s_Path = getDocumentDir();

    s_DownloadPath = QFileDialog::getExistingDirectory( this, tr( "Choose a folder to process all files whitin this folder" ), s_Path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog );

    if ( s_DownloadPath.isEmpty() == false )
    {
        if ( s_DownloadPath.endsWith( "/" ) == true )
            DownloadDirectory_lineEdit->setText( QDir::toNativeSeparators( s_DownloadPath ) );
        else
            DownloadDirectory_lineEdit->setText( QDir::toNativeSeparators( s_DownloadPath + "/" ) );
    }
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

QString DownloadManagerDialog::getDocumentDir()
{
    #if defined(Q_OS_LINUX)
        return( QDir::homePath() );
    #endif

    #if defined(Q_OS_MAC)
        return( QDir::homePath() );
    #endif

    #if defined(Q_OS_WIN)
        return( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ) );
    #endif
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectAllStations()
{
    ALE_checkBox->setChecked( true );
    ASP_checkBox->setChecked( true );
    BAR_checkBox->setChecked( true );
    BER_checkBox->setChecked( true );
    BIL_checkBox->setChecked( true );
    BON_checkBox->setChecked( true );
    BOS_checkBox->setChecked( true );
    BOU_checkBox->setChecked( true );
    BRB_checkBox->setChecked( true );
    BUD_checkBox->setChecked( true );
    CAB_checkBox->setChecked( true );
    CAM_checkBox->setChecked( true );
    CAR_checkBox->setChecked( true );
    CLH_checkBox->setChecked( true );
    CNR_checkBox->setChecked( true );
    COC_checkBox->setChecked( true );
    DAA_checkBox->setChecked( true );
    DAR_checkBox->setChecked( true );
    DOM_checkBox->setChecked( true );
    DRA_checkBox->setChecked( true );
    DWN_checkBox->setChecked( true );
    E13_checkBox->setChecked( true );
    EUR_checkBox->setChecked( true );
    FLO_checkBox->setChecked( true );
    FPE_checkBox->setChecked( true );
    FUA_checkBox->setChecked( true );
    GCR_checkBox->setChecked( true );
    GOB_checkBox->setChecked( true );
    GVN_checkBox->setChecked( true );
    ILO_checkBox->setChecked( true );
    ISH_checkBox->setChecked( true );
    IZA_checkBox->setChecked( true );
    KWA_checkBox->setChecked( true );
    LAU_checkBox->setChecked( true );
    LER_checkBox->setChecked( true );
    LIN_checkBox->setChecked( true );
    MAN_checkBox->setChecked( true );
    MNM_checkBox->setChecked( true );
    NAU_checkBox->setChecked( true );
    NYA_checkBox->setChecked( true );
    PAL_checkBox->setChecked( true );
    PAY_checkBox->setChecked( true );
    PSA_checkBox->setChecked( true );
    PSU_checkBox->setChecked( true );
    PTR_checkBox->setChecked( true );
    REG_checkBox->setChecked( true );
    RLM_checkBox->setChecked( true );
    SAP_checkBox->setChecked( true );
    SBO_checkBox->setChecked( true );
    SMS_checkBox->setChecked( true );
    SON_checkBox->setChecked( true );
    SOV_checkBox->setChecked( true );
    SPO_checkBox->setChecked( true );
    SXF_checkBox->setChecked( true );
    SYO_checkBox->setChecked( true );
    TAM_checkBox->setChecked( true );
    TAT_checkBox->setChecked( true );
    TIK_checkBox->setChecked( true );
    TOR_checkBox->setChecked( true );
    XIA_checkBox->setChecked( true );
    ZVE_checkBox->setChecked( true );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneStation()
{
    ALE_checkBox->setChecked( false );
    ASP_checkBox->setChecked( false );
    BAR_checkBox->setChecked( false );
    BER_checkBox->setChecked( false );
    BIL_checkBox->setChecked( false );
    BON_checkBox->setChecked( false );
    BOS_checkBox->setChecked( false );
    BOU_checkBox->setChecked( false );
    BRB_checkBox->setChecked( false );
    BUD_checkBox->setChecked( false );
    CAB_checkBox->setChecked( false );
    CAM_checkBox->setChecked( false );
    CAR_checkBox->setChecked( false );
    CLH_checkBox->setChecked( false );
    CNR_checkBox->setChecked( false );
    COC_checkBox->setChecked( false );
    DAA_checkBox->setChecked( false );
    DAR_checkBox->setChecked( false );
    DOM_checkBox->setChecked( false );
    DRA_checkBox->setChecked( false );
    DWN_checkBox->setChecked( false );
    E13_checkBox->setChecked( false );
    EUR_checkBox->setChecked( false );
    FLO_checkBox->setChecked( false );
    FPE_checkBox->setChecked( false );
    FUA_checkBox->setChecked( false );
    GCR_checkBox->setChecked( false );
    GOB_checkBox->setChecked( false );
    GVN_checkBox->setChecked( false );
    ILO_checkBox->setChecked( false );
    ISH_checkBox->setChecked( false );
    IZA_checkBox->setChecked( false );
    KWA_checkBox->setChecked( false );
    LAU_checkBox->setChecked( false );
    LER_checkBox->setChecked( false );
    LIN_checkBox->setChecked( false );
    MAN_checkBox->setChecked( false );
    MNM_checkBox->setChecked( false );
    NAU_checkBox->setChecked( false );
    NYA_checkBox->setChecked( false );
    PAL_checkBox->setChecked( false );
    PAY_checkBox->setChecked( false );
    PSA_checkBox->setChecked( false );
    PSU_checkBox->setChecked( false );
    PTR_checkBox->setChecked( false );
    REG_checkBox->setChecked( false );
    RLM_checkBox->setChecked( false );
    SAP_checkBox->setChecked( false );
    SBO_checkBox->setChecked( false );
    SMS_checkBox->setChecked( false );
    SON_checkBox->setChecked( false );
    SOV_checkBox->setChecked( false );
    SPO_checkBox->setChecked( false );
    SXF_checkBox->setChecked( false );
    SYO_checkBox->setChecked( false );
    TAM_checkBox->setChecked( false );
    TAT_checkBox->setChecked( false );
    TIK_checkBox->setChecked( false );
    TOR_checkBox->setChecked( false );
    XIA_checkBox->setChecked( false );
    ZVE_checkBox->setChecked( false );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectAllMonth()
{
    January_checkBox->setChecked( true );
    February_checkBox->setChecked( true );
    March_checkBox->setChecked( true );
    April_checkBox->setChecked( true );
    May_checkBox->setChecked( true );
    June_checkBox->setChecked( true );
    July_checkBox->setChecked( true );
    August_checkBox->setChecked( true );
    September_checkBox->setChecked( true );
    October_checkBox->setChecked( true );
    November_checkBox->setChecked( true );
    December_checkBox->setChecked( true );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneMonth()
{
    January_checkBox->setChecked( false );
    February_checkBox->setChecked( false );
    March_checkBox->setChecked( false );
    April_checkBox->setChecked( false );
    May_checkBox->setChecked( false );
    June_checkBox->setChecked( false );
    July_checkBox->setChecked( false );
    August_checkBox->setChecked( false );
    September_checkBox->setChecked( false );
    October_checkBox->setChecked( false );
    November_checkBox->setChecked( false );
    December_checkBox->setChecked( false );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectAllYears()
{
    Year1992_checkBox->setChecked( true );
    Year1993_checkBox->setChecked( true );
    Year1994_checkBox->setChecked( true );
    Year1995_checkBox->setChecked( true );
    Year1996_checkBox->setChecked( true );
    Year1997_checkBox->setChecked( true );
    Year1998_checkBox->setChecked( true );
    Year1999_checkBox->setChecked( true );
    Year2000_checkBox->setChecked( true );
    Year2001_checkBox->setChecked( true );
    Year2002_checkBox->setChecked( true );
    Year2003_checkBox->setChecked( true );
    Year2004_checkBox->setChecked( true );
    Year2005_checkBox->setChecked( true );
    Year2006_checkBox->setChecked( true );
    Year2007_checkBox->setChecked( true );
    Year2008_checkBox->setChecked( true );
    Year2009_checkBox->setChecked( true );
    Year2010_checkBox->setChecked( true );
    Year2011_checkBox->setChecked( true );
    Year2012_checkBox->setChecked( true );
    Year2013_checkBox->setChecked( true );
    Year2014_checkBox->setChecked( true );
    Year2015_checkBox->setChecked( true );
    Year2016_checkBox->setChecked( true );
    Year2017_checkBox->setChecked( true );
    Year2018_checkBox->setChecked( true );
    Year2019_checkBox->setChecked( true );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneYear()
{
    Year1992_checkBox->setChecked( false );
    Year1993_checkBox->setChecked( false );
    Year1994_checkBox->setChecked( false );
    Year1995_checkBox->setChecked( false );
    Year1996_checkBox->setChecked( false );
    Year1997_checkBox->setChecked( false );
    Year1998_checkBox->setChecked( false );
    Year1999_checkBox->setChecked( false );
    Year2000_checkBox->setChecked( false );
    Year2001_checkBox->setChecked( false );
    Year2002_checkBox->setChecked( false );
    Year2003_checkBox->setChecked( false );
    Year2004_checkBox->setChecked( false );
    Year2005_checkBox->setChecked( false );
    Year2006_checkBox->setChecked( false );
    Year2007_checkBox->setChecked( false );
    Year2008_checkBox->setChecked( false );
    Year2009_checkBox->setChecked( false );
    Year2010_checkBox->setChecked( false );
    Year2011_checkBox->setChecked( false );
    Year2012_checkBox->setChecked( false );
    Year2013_checkBox->setChecked( false );
    Year2014_checkBox->setChecked( false );
    Year2015_checkBox->setChecked( false );
    Year2016_checkBox->setChecked( false );
    Year2017_checkBox->setChecked( false );
    Year2018_checkBox->setChecked( false );
    Year2019_checkBox->setChecked( false );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doDownloadManagerDialog( QString &s_DownloadPath, QString &s_FTPServer, QString &s_User,
                                         QString &s_Password, bool &b_DecompressFiles, bool &b_CheckFiles,
                                         bool &b_CheckAvailability, bool b_Station[MAX_NUM_OF_STATION+1],
                                         bool b_Month[MAX_NUM_OF_MONTH+1], bool b_Year[MAX_NUM_OF_YEAR+1] )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************************************

    DownloadManagerDialog dialog( this );

    // Stations
    dialog.ALE_checkBox->setChecked( b_Station[ALE] );
    dialog.ASP_checkBox->setChecked( b_Station[ASP] );
    dialog.BAR_checkBox->setChecked( b_Station[BAR] );
    dialog.BER_checkBox->setChecked( b_Station[BER] );
    dialog.BIL_checkBox->setChecked( b_Station[BIL] );
    dialog.BON_checkBox->setChecked( b_Station[BON] );
    dialog.BOS_checkBox->setChecked( b_Station[BOS] );
    dialog.BOU_checkBox->setChecked( b_Station[BOU] );
    dialog.BRB_checkBox->setChecked( b_Station[BRB] );
    dialog.BUD_checkBox->setChecked( b_Station[BUD] );
    dialog.CAB_checkBox->setChecked( b_Station[CAB] );
    dialog.CAM_checkBox->setChecked( b_Station[CAM] );
    dialog.CAR_checkBox->setChecked( b_Station[CAR] );
    dialog.CLH_checkBox->setChecked( b_Station[CLH] );
    dialog.CNR_checkBox->setChecked( b_Station[CNR] );
    dialog.COC_checkBox->setChecked( b_Station[COC] );
    dialog.DAA_checkBox->setChecked( b_Station[DAA] );
    dialog.DAR_checkBox->setChecked( b_Station[DAR] );
    dialog.DOM_checkBox->setChecked( b_Station[DOM] );
    dialog.DRA_checkBox->setChecked( b_Station[DRA] );
    dialog.DWN_checkBox->setChecked( b_Station[DWN] );
    dialog.E13_checkBox->setChecked( b_Station[E13] );
    dialog.EUR_checkBox->setChecked( b_Station[EUR] );
    dialog.FLO_checkBox->setChecked( b_Station[FLO] );
    dialog.FPE_checkBox->setChecked( b_Station[FPE] );
    dialog.FUA_checkBox->setChecked( b_Station[FUA] );
    dialog.GCR_checkBox->setChecked( b_Station[GCR] );
    dialog.GOB_checkBox->setChecked( b_Station[GOB] );
    dialog.GVN_checkBox->setChecked( b_Station[GVN] );
    dialog.ILO_checkBox->setChecked( b_Station[ILO] );
    dialog.ISH_checkBox->setChecked( b_Station[ISH] );
    dialog.IZA_checkBox->setChecked( b_Station[IZA] );
    dialog.KWA_checkBox->setChecked( b_Station[KWA] );
    dialog.LAU_checkBox->setChecked( b_Station[LAU] );
    dialog.LER_checkBox->setChecked( b_Station[LER] );
    dialog.LIN_checkBox->setChecked( b_Station[LIN] );
    dialog.MAN_checkBox->setChecked( b_Station[MAN] );
    dialog.MNM_checkBox->setChecked( b_Station[MNM] );
    dialog.NAU_checkBox->setChecked( b_Station[NAU] );
    dialog.NYA_checkBox->setChecked( b_Station[NYA] );
    dialog.PAL_checkBox->setChecked( b_Station[PAL] );
    dialog.PAY_checkBox->setChecked( b_Station[PAY] );
    dialog.PSA_checkBox->setChecked( b_Station[PSA] );
    dialog.PSU_checkBox->setChecked( b_Station[PSU] );
    dialog.PTR_checkBox->setChecked( b_Station[PTR] );
    dialog.REG_checkBox->setChecked( b_Station[REG] );
    dialog.RLM_checkBox->setChecked( b_Station[RLM] );
    dialog.SAP_checkBox->setChecked( b_Station[SAP] );
    dialog.SBO_checkBox->setChecked( b_Station[SBO] );
    dialog.SMS_checkBox->setChecked( b_Station[SMS] );
    dialog.SON_checkBox->setChecked( b_Station[SON] );
    dialog.SOV_checkBox->setChecked( b_Station[SOV] );
    dialog.SPO_checkBox->setChecked( b_Station[SPO] );
    dialog.SXF_checkBox->setChecked( b_Station[SXF] );
    dialog.SYO_checkBox->setChecked( b_Station[SYO] );
    dialog.TAM_checkBox->setChecked( b_Station[TAM] );
    dialog.TAT_checkBox->setChecked( b_Station[TAT] );
    dialog.TIK_checkBox->setChecked( b_Station[TIK] );
    dialog.TOR_checkBox->setChecked( b_Station[TOR] );
    dialog.XIA_checkBox->setChecked( b_Station[XIA] );
    dialog.ZVE_checkBox->setChecked( b_Station[ZVE] );

    // Month
    dialog.January_checkBox->setChecked( b_Month[1] );
    dialog.February_checkBox->setChecked( b_Month[2] );
    dialog.March_checkBox->setChecked( b_Month[3] );
    dialog.April_checkBox->setChecked( b_Month[4] );
    dialog.May_checkBox->setChecked( b_Month[5] );
    dialog.June_checkBox->setChecked( b_Month[6] );
    dialog.July_checkBox->setChecked( b_Month[7] );
    dialog.August_checkBox->setChecked( b_Month[8] );
    dialog.September_checkBox->setChecked( b_Month[9] );
    dialog.October_checkBox->setChecked( b_Month[10] );
    dialog.November_checkBox->setChecked( b_Month[11] );
    dialog.December_checkBox->setChecked( b_Month[12] );

    // Years
    dialog.Year1992_checkBox->setChecked( b_Year[1] );
    dialog.Year1993_checkBox->setChecked( b_Year[2] );
    dialog.Year1994_checkBox->setChecked( b_Year[3] );
    dialog.Year1995_checkBox->setChecked( b_Year[4] );
    dialog.Year1996_checkBox->setChecked( b_Year[5] );
    dialog.Year1997_checkBox->setChecked( b_Year[6] );
    dialog.Year1998_checkBox->setChecked( b_Year[7] );
    dialog.Year1999_checkBox->setChecked( b_Year[8] );
    dialog.Year2000_checkBox->setChecked( b_Year[9] );
    dialog.Year2001_checkBox->setChecked( b_Year[10] );
    dialog.Year2002_checkBox->setChecked( b_Year[11] );
    dialog.Year2003_checkBox->setChecked( b_Year[12] );
    dialog.Year2004_checkBox->setChecked( b_Year[13] );
    dialog.Year2005_checkBox->setChecked( b_Year[14] );
    dialog.Year2006_checkBox->setChecked( b_Year[15] );
    dialog.Year2007_checkBox->setChecked( b_Year[16] );
    dialog.Year2008_checkBox->setChecked( b_Year[17] );
    dialog.Year2009_checkBox->setChecked( b_Year[18] );
    dialog.Year2010_checkBox->setChecked( b_Year[19] );
    dialog.Year2011_checkBox->setChecked( b_Year[20] );
    dialog.Year2012_checkBox->setChecked( b_Year[21] );
    dialog.Year2013_checkBox->setChecked( b_Year[22] );
    dialog.Year2014_checkBox->setChecked( b_Year[23] );
    dialog.Year2015_checkBox->setChecked( b_Year[24] );
    dialog.Year2016_checkBox->setChecked( b_Year[25] );
    dialog.Year2017_checkBox->setChecked( b_Year[26] );
    dialog.Year2018_checkBox->setChecked( b_Year[27] );
    dialog.Year2019_checkBox->setChecked( b_Year[28] );

    // Options
    dialog.DecompressFiles_checkBox->setChecked( b_DecompressFiles );
    dialog.CheckFiles_checkBox->setChecked( b_CheckFiles );
    dialog.CheckAvailability_checkBox->setChecked( b_CheckAvailability );

    if ( s_DownloadPath.isEmpty() == false )
    {
        if ( s_DownloadPath.endsWith( QDir::toNativeSeparators( "/" ) ) == true )
            dialog.DownloadDirectory_lineEdit->setText( QDir::toNativeSeparators( s_DownloadPath ) );
        else
            dialog.DownloadDirectory_lineEdit->setText( QDir::toNativeSeparators( s_DownloadPath + "/" ) );
    }

    // Account
    dialog.FTPServer_lineEdit->setText( s_FTPServer );
    dialog.User_lineEdit->setText( s_User );
    dialog.Password_lineEdit->setText( s_Password );

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( dialog.sizeHint() );
    dialog.show();

// ******************************************************************************************************

    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        // Stations
        b_Station[ALE]  = dialog.ALE_checkBox->isChecked();
        b_Station[ASP]  = dialog.ASP_checkBox->isChecked();
        b_Station[BAR]  = dialog.BAR_checkBox->isChecked();
        b_Station[BER]  = dialog.BER_checkBox->isChecked();
        b_Station[BIL]  = dialog.BIL_checkBox->isChecked();
        b_Station[BON]  = dialog.BON_checkBox->isChecked();
        b_Station[BOS]  = dialog.BOS_checkBox->isChecked();
        b_Station[BOU]  = dialog.BOU_checkBox->isChecked();
        b_Station[BRB]  = dialog.BRB_checkBox->isChecked();
        b_Station[BUD]  = dialog.BUD_checkBox->isChecked();
        b_Station[CAB]  = dialog.CAB_checkBox->isChecked();
        b_Station[CAM]  = dialog.CAM_checkBox->isChecked();
        b_Station[CAR]  = dialog.CAR_checkBox->isChecked();
        b_Station[CLH]  = dialog.CLH_checkBox->isChecked();
        b_Station[CNR]  = dialog.CNR_checkBox->isChecked();
        b_Station[COC]  = dialog.COC_checkBox->isChecked();
        b_Station[DAA]  = dialog.DAA_checkBox->isChecked();
        b_Station[DAR]  = dialog.DAR_checkBox->isChecked();
        b_Station[DOM]  = dialog.DOM_checkBox->isChecked();
        b_Station[DRA]  = dialog.DRA_checkBox->isChecked();
        b_Station[DWN]  = dialog.DWN_checkBox->isChecked();
        b_Station[E13]  = dialog.E13_checkBox->isChecked();
        b_Station[EUR]  = dialog.EUR_checkBox->isChecked();
        b_Station[FLO]  = dialog.FLO_checkBox->isChecked();
        b_Station[FPE]  = dialog.FPE_checkBox->isChecked();
        b_Station[FUA]  = dialog.FUA_checkBox->isChecked();
        b_Station[GCR]  = dialog.GCR_checkBox->isChecked();
        b_Station[GOB]  = dialog.GOB_checkBox->isChecked();
        b_Station[GVN]  = dialog.GVN_checkBox->isChecked();
        b_Station[ILO]  = dialog.ILO_checkBox->isChecked();
        b_Station[ISH]  = dialog.ISH_checkBox->isChecked();
        b_Station[IZA]  = dialog.IZA_checkBox->isChecked();
        b_Station[KWA]  = dialog.KWA_checkBox->isChecked();
        b_Station[LAU]  = dialog.LAU_checkBox->isChecked();
        b_Station[LER]  = dialog.LER_checkBox->isChecked();
        b_Station[LIN]  = dialog.LIN_checkBox->isChecked();
        b_Station[MAN]  = dialog.MAN_checkBox->isChecked();
        b_Station[MNM]  = dialog.MNM_checkBox->isChecked();
        b_Station[NAU]  = dialog.NAU_checkBox->isChecked();
        b_Station[NYA]  = dialog.NYA_checkBox->isChecked();
        b_Station[PAL]  = dialog.PAL_checkBox->isChecked();
        b_Station[PAY]  = dialog.PAY_checkBox->isChecked();
        b_Station[PSA]  = dialog.PSA_checkBox->isChecked();
        b_Station[PSU]  = dialog.PSU_checkBox->isChecked();
        b_Station[PTR]  = dialog.PTR_checkBox->isChecked();
        b_Station[REG]  = dialog.REG_checkBox->isChecked();
        b_Station[RLM]  = dialog.RLM_checkBox->isChecked();
        b_Station[SAP]  = dialog.SAP_checkBox->isChecked();
        b_Station[SBO]  = dialog.SBO_checkBox->isChecked();
        b_Station[SMS]  = dialog.SMS_checkBox->isChecked();
        b_Station[SON]  = dialog.SON_checkBox->isChecked();
        b_Station[SOV]  = dialog.SOV_checkBox->isChecked();
        b_Station[SPO]  = dialog.SPO_checkBox->isChecked();
        b_Station[SXF]  = dialog.SXF_checkBox->isChecked();
        b_Station[SYO]  = dialog.SYO_checkBox->isChecked();
        b_Station[TAM]  = dialog.TAM_checkBox->isChecked();
        b_Station[TAT]  = dialog.TAT_checkBox->isChecked();
        b_Station[TIK]  = dialog.TIK_checkBox->isChecked();
        b_Station[TOR]  = dialog.TOR_checkBox->isChecked();
        b_Station[XIA]  = dialog.XIA_checkBox->isChecked();
        b_Station[ZVE]  = dialog.ZVE_checkBox->isChecked();

        // Month
        b_Month[1]	= dialog.January_checkBox->isChecked();
        b_Month[2]	= dialog.February_checkBox->isChecked();
        b_Month[3]	= dialog.March_checkBox->isChecked();
        b_Month[4]	= dialog.April_checkBox->isChecked();
        b_Month[5]	= dialog.May_checkBox->isChecked();
        b_Month[6]	= dialog.June_checkBox->isChecked();
        b_Month[7]	= dialog.July_checkBox->isChecked();
        b_Month[8]	= dialog.August_checkBox->isChecked();
        b_Month[9]	= dialog.September_checkBox->isChecked();
        b_Month[10]	= dialog.October_checkBox->isChecked();
        b_Month[11]	= dialog.November_checkBox->isChecked();
        b_Month[12]	= dialog.December_checkBox->isChecked();

    // Years
        b_Year[1]	= dialog.Year1992_checkBox->isChecked();
        b_Year[2]	= dialog.Year1993_checkBox->isChecked();
        b_Year[3]	= dialog.Year1994_checkBox->isChecked();
        b_Year[4]	= dialog.Year1995_checkBox->isChecked();
        b_Year[5]	= dialog.Year1996_checkBox->isChecked();
        b_Year[6]	= dialog.Year1997_checkBox->isChecked();
        b_Year[7]	= dialog.Year1998_checkBox->isChecked();
        b_Year[8]	= dialog.Year1999_checkBox->isChecked();
        b_Year[9]	= dialog.Year2000_checkBox->isChecked();
        b_Year[10]	= dialog.Year2001_checkBox->isChecked();
        b_Year[11]	= dialog.Year2002_checkBox->isChecked();
        b_Year[12]	= dialog.Year2003_checkBox->isChecked();
        b_Year[13]	= dialog.Year2004_checkBox->isChecked();
        b_Year[14]	= dialog.Year2005_checkBox->isChecked();
        b_Year[15]	= dialog.Year2006_checkBox->isChecked();
        b_Year[16]	= dialog.Year2007_checkBox->isChecked();
        b_Year[17]	= dialog.Year2008_checkBox->isChecked();
        b_Year[18]	= dialog.Year2009_checkBox->isChecked();
        b_Year[19]	= dialog.Year2010_checkBox->isChecked();
        b_Year[20]	= dialog.Year2011_checkBox->isChecked();
        b_Year[21]	= dialog.Year2012_checkBox->isChecked();
        b_Year[22]	= dialog.Year2013_checkBox->isChecked();
        b_Year[23]	= dialog.Year2014_checkBox->isChecked();
        b_Year[24]	= dialog.Year2015_checkBox->isChecked();
        b_Year[25]	= dialog.Year2016_checkBox->isChecked();
        b_Year[26]	= dialog.Year2017_checkBox->isChecked();
        b_Year[27]	= dialog.Year2018_checkBox->isChecked();
        b_Year[28]	= dialog.Year2019_checkBox->isChecked();

        b_DecompressFiles	= dialog.DecompressFiles_checkBox->isChecked();
        b_CheckFiles		= dialog.CheckFiles_checkBox->isChecked();
        b_CheckAvailability	= dialog.CheckAvailability_checkBox->isChecked();

        s_FTPServer			= dialog.FTPServer_lineEdit->text();
        s_User				= dialog.User_lineEdit->text();
        s_Password			= dialog.Password_lineEdit->text();

        s_DownloadPath		= dialog.DownloadDirectory_lineEdit->text();

        i_DialogResult      = QDialog::Accepted;
        break;

    case QDialog::Rejected:
        break;

    default:
        break;
    }

    posDialog = dialog.pos();

    return( i_DialogResult );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::on_DownloadDirectory_lineEdit_textChanged()
{
    if ( checkAccount() == true )
    {
        QFileInfo fi( DownloadDirectory_lineEdit->text() );

        if ( fi.exists() == true )
            OK_pushButton->setEnabled( true );
        else
            OK_pushButton->setEnabled( false );
    }
    else
        OK_pushButton->setEnabled( false );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::on_FTPServer_lineEdit_textChanged()
{
    OK_pushButton->setEnabled( checkAccount() );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::on_User_lineEdit_textChanged()
{
    OK_pushButton->setEnabled( checkAccount() );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::on_Password_lineEdit_textChanged()
{
    OK_pushButton->setEnabled( checkAccount() );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

bool DownloadManagerDialog::checkAccount()
{
    if ( ( FTPServer_lineEdit->text() == "ftp.bsrn.awi.de" ) && ( User_lineEdit->text().isEmpty() == false ) && ( Password_lineEdit->text().isEmpty() == false ) )
        return( true );

    return( false );
}
