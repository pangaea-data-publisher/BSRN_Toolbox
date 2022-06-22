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
    connect(CheckAvailability_checkBox, SIGNAL(toggled(bool)), this, SLOT(on_CheckAvailability_checkBox_toggled()));
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

void DownloadManagerDialog::selectAllStations(bool selected) {
    ALE_checkBox->setChecked( selected );
    ASP_checkBox->setChecked( selected );
    BAR_checkBox->setChecked( selected );
    BER_checkBox->setChecked( selected );
    BIL_checkBox->setChecked( selected );
    BON_checkBox->setChecked( selected );
    BOS_checkBox->setChecked( selected );
    BOU_checkBox->setChecked( selected );
    BRB_checkBox->setChecked( selected );
    BUD_checkBox->setChecked( selected );
    CAB_checkBox->setChecked( selected );
    CAM_checkBox->setChecked( selected );
    CAP_checkBox->setChecked( selected );
    CAR_checkBox->setChecked( selected );
    CLH_checkBox->setChecked( selected );
    CNR_checkBox->setChecked( selected );
    COC_checkBox->setChecked( selected );
    DAA_checkBox->setChecked( selected );
    DAR_checkBox->setChecked( selected );
    DOM_checkBox->setChecked( selected );
    DON_checkBox->setChecked( selected );
    DRA_checkBox->setChecked( selected );
    DWN_checkBox->setChecked( selected );
    E13_checkBox->setChecked( selected );
    ENA_checkBox->setChecked( selected );
    EUR_checkBox->setChecked( selected );
    FLO_checkBox->setChecked( selected );
    FPE_checkBox->setChecked( selected );
    FUA_checkBox->setChecked( selected );
    GAN_checkBox->setChecked( selected );
    GIM_checkBox->setChecked( selected );
    GCR_checkBox->setChecked( selected );
    GOB_checkBox->setChecked( selected );
    GUR_checkBox->setChecked( selected );
    GVN_checkBox->setChecked( selected );
    HOW_checkBox->setChecked( selected );
    ILO_checkBox->setChecked( selected );
    ISH_checkBox->setChecked( selected );
    IZA_checkBox->setChecked( selected );
    KWA_checkBox->setChecked( selected );
    LAU_checkBox->setChecked( selected );
    LER_checkBox->setChecked( selected );
    LIN_checkBox->setChecked( selected );
    LLN_checkBox->setChecked( selected );
    LRC_checkBox->setChecked( selected );
    LYU_checkBox->setChecked( selected );
    MAN_checkBox->setChecked( selected );
    MNM_checkBox->setChecked( selected );
    NAU_checkBox->setChecked( selected );
    NEW_checkBox->setChecked( selected );
    NYA_checkBox->setChecked( selected );
    OHY_checkBox->setChecked( selected );
    PAL_checkBox->setChecked( selected );
    PAR_checkBox->setChecked( selected );
    PAY_checkBox->setChecked( selected );
    PSA_checkBox->setChecked( selected );
    PSU_checkBox->setChecked( selected );
    PTR_checkBox->setChecked( selected );
    REG_checkBox->setChecked( selected );
    RLM_checkBox->setChecked( selected );
    RUN_checkBox->setChecked( selected );
    SAP_checkBox->setChecked( selected );
    SBO_checkBox->setChecked( selected );
    SEL_checkBox->setChecked( selected );
    SMS_checkBox->setChecked( selected );
    SON_checkBox->setChecked( selected );
    SOV_checkBox->setChecked( selected );
    SPO_checkBox->setChecked( selected );
    SXF_checkBox->setChecked( selected );
    SYO_checkBox->setChecked( selected );
    TAM_checkBox->setChecked( selected );
    TAT_checkBox->setChecked( selected );
    TIK_checkBox->setChecked( selected );
    TIR_checkBox->setChecked( selected );
    TOR_checkBox->setChecked( selected );
    XIA_checkBox->setChecked( selected );
    YUS_checkBox->setChecked( selected );
    //ZVE_checkBox->setChecked( selected );
    ABS_checkBox->setChecked( selected );
    CYL_checkBox->setChecked( selected );
    NPT_checkBox->setChecked( selected );
    INO_checkBox->setChecked( selected );
    TNB_checkBox->setChecked( selected );
}

void DownloadManagerDialog::enableAllStations(bool selected) {
    ALE_checkBox->setEnabled( selected );
    ASP_checkBox->setEnabled( selected );
    BAR_checkBox->setEnabled( selected );
    BER_checkBox->setEnabled( selected );
    BIL_checkBox->setEnabled( selected );
    BON_checkBox->setEnabled( selected );
    BOS_checkBox->setEnabled( selected );
    BOU_checkBox->setEnabled( selected );
    BRB_checkBox->setEnabled( selected );
    BUD_checkBox->setEnabled( selected );
    CAB_checkBox->setEnabled( selected );
    CAM_checkBox->setEnabled( selected );
    CAP_checkBox->setEnabled( selected );
    CAR_checkBox->setEnabled( selected );
    CLH_checkBox->setEnabled( selected );
    CNR_checkBox->setEnabled( selected );
    COC_checkBox->setEnabled( selected );
    DAA_checkBox->setEnabled( selected );
    DAR_checkBox->setEnabled( selected );
    DOM_checkBox->setEnabled( selected );
    DON_checkBox->setEnabled( selected );
    DRA_checkBox->setEnabled( selected );
    DWN_checkBox->setEnabled( selected );
    E13_checkBox->setEnabled( selected );
    ENA_checkBox->setEnabled( selected );
    EUR_checkBox->setEnabled( selected );
    FLO_checkBox->setEnabled( selected );
    FPE_checkBox->setEnabled( selected );
    FUA_checkBox->setEnabled( selected );
    GAN_checkBox->setEnabled( selected );
    GCR_checkBox->setEnabled( selected );
    GOB_checkBox->setEnabled( selected );
    GUR_checkBox->setEnabled( selected );
    GVN_checkBox->setEnabled( selected );
    HOW_checkBox->setEnabled( selected );
    ILO_checkBox->setEnabled( selected );
    ISH_checkBox->setEnabled( selected );
    IZA_checkBox->setEnabled( selected );
    KWA_checkBox->setEnabled( selected );
    LAU_checkBox->setEnabled( selected );
    LER_checkBox->setEnabled( selected );
    LIN_checkBox->setEnabled( selected );
    LLN_checkBox->setEnabled( selected );
    LRC_checkBox->setEnabled( selected );
    MAN_checkBox->setEnabled( selected );
    MNM_checkBox->setEnabled( selected );
    NAU_checkBox->setEnabled( selected );
    NEW_checkBox->setEnabled( selected );
    NYA_checkBox->setEnabled( selected );
    PAL_checkBox->setEnabled( selected );
    PAY_checkBox->setEnabled( selected );
    PSA_checkBox->setEnabled( selected );
    PSU_checkBox->setEnabled( selected );
    PTR_checkBox->setEnabled( selected );
    REG_checkBox->setEnabled( selected );
    RLM_checkBox->setEnabled( selected );
    SAP_checkBox->setEnabled( selected );
    SBO_checkBox->setEnabled( selected );
    SMS_checkBox->setEnabled( selected );
    SON_checkBox->setEnabled( selected );
    SOV_checkBox->setEnabled( selected );
    SPO_checkBox->setEnabled( selected );
    SXF_checkBox->setEnabled( selected );
    SYO_checkBox->setEnabled( selected );
    TAM_checkBox->setEnabled( selected );
    TAT_checkBox->setEnabled( selected );
    TIK_checkBox->setEnabled( selected );
    TIR_checkBox->setEnabled( selected );
    TOR_checkBox->setEnabled( selected );
    XIA_checkBox->setEnabled( selected );
    //ZVE_checkBox->setEnabled( selected );
    ABS_checkBox->setEnabled( selected );
    CYL_checkBox->setEnabled( selected );
    NPT_checkBox->setEnabled( selected );
    INO_checkBox->setEnabled( selected );
    TNB_checkBox->setEnabled( selected );
}

void DownloadManagerDialog::SelectAllStations()
{
    selectAllStations(true);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneStation()
{
    selectAllStations(false);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::selectAllMonths(bool selected)
{
    January_checkBox->setChecked( selected );
    February_checkBox->setChecked( selected );
    March_checkBox->setChecked( selected );
    April_checkBox->setChecked( selected );
    May_checkBox->setChecked( selected );
    June_checkBox->setChecked( selected );
    July_checkBox->setChecked( selected );
    August_checkBox->setChecked( selected );
    September_checkBox->setChecked( selected );
    October_checkBox->setChecked( selected );
    November_checkBox->setChecked( selected );
    December_checkBox->setChecked( selected );
}
void DownloadManagerDialog::enableAllMonths(bool selected) {
    January_checkBox->setEnabled( selected );
    February_checkBox->setEnabled( selected );
    March_checkBox->setEnabled( selected );
    April_checkBox->setEnabled( selected );
    May_checkBox->setEnabled( selected );
    June_checkBox->setEnabled( selected );
    July_checkBox->setEnabled( selected );
    August_checkBox->setEnabled( selected );
    September_checkBox->setEnabled( selected );
    October_checkBox->setEnabled( selected );
    November_checkBox->setEnabled( selected );
    December_checkBox->setEnabled( selected );
}
void DownloadManagerDialog::SelectAllMonth()
{
    selectAllMonths(true);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneMonth()
{
    selectAllMonths(false);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::EnableAllMonths()
{
    enableAllMonths(true);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::DisableAllMonths()
{
    enableAllMonths(false);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::selectAllYears(bool selected) {
    Year1992_checkBox->setChecked( selected );
    Year1993_checkBox->setChecked( selected );
    Year1994_checkBox->setChecked( selected );
    Year1995_checkBox->setChecked( selected );
    Year1996_checkBox->setChecked( selected );
    Year1997_checkBox->setChecked( selected );
    Year1998_checkBox->setChecked( selected );
    Year1999_checkBox->setChecked( selected );
    Year2000_checkBox->setChecked( selected );
    Year2001_checkBox->setChecked( selected );
    Year2002_checkBox->setChecked( selected );
    Year2003_checkBox->setChecked( selected );
    Year2004_checkBox->setChecked( selected );
    Year2005_checkBox->setChecked( selected );
    Year2006_checkBox->setChecked( selected );
    Year2007_checkBox->setChecked( selected );
    Year2008_checkBox->setChecked( selected );
    Year2009_checkBox->setChecked( selected );
    Year2010_checkBox->setChecked( selected );
    Year2011_checkBox->setChecked( selected );
    Year2012_checkBox->setChecked( selected );
    Year2013_checkBox->setChecked( selected );
    Year2014_checkBox->setChecked( selected );
    Year2015_checkBox->setChecked( selected );
    Year2016_checkBox->setChecked( selected );
    Year2017_checkBox->setChecked( selected );
    Year2018_checkBox->setChecked( selected );
    Year2019_checkBox->setChecked( selected );
    Year2020_checkBox->setChecked( selected );
    Year2021_checkBox->setChecked( selected );
    Year2022_checkBox->setChecked( selected );
}
void DownloadManagerDialog::enableAllYears(bool selected) {
    Year1992_checkBox->setEnabled( selected );
    Year1993_checkBox->setEnabled( selected );
    Year1994_checkBox->setEnabled( selected );
    Year1995_checkBox->setEnabled( selected );
    Year1996_checkBox->setEnabled( selected );
    Year1997_checkBox->setEnabled( selected );
    Year1998_checkBox->setEnabled( selected );
    Year1999_checkBox->setEnabled( selected );
    Year2000_checkBox->setEnabled( selected );
    Year2001_checkBox->setEnabled( selected );
    Year2002_checkBox->setEnabled( selected );
    Year2003_checkBox->setEnabled( selected );
    Year2004_checkBox->setEnabled( selected );
    Year2005_checkBox->setEnabled( selected );
    Year2006_checkBox->setEnabled( selected );
    Year2007_checkBox->setEnabled( selected );
    Year2008_checkBox->setEnabled( selected );
    Year2009_checkBox->setEnabled( selected );
    Year2010_checkBox->setEnabled( selected );
    Year2011_checkBox->setEnabled( selected );
    Year2012_checkBox->setEnabled( selected );
    Year2013_checkBox->setEnabled( selected );
    Year2014_checkBox->setEnabled( selected );
    Year2015_checkBox->setEnabled( selected );
    Year2016_checkBox->setEnabled( selected );
    Year2017_checkBox->setEnabled( selected );
    Year2018_checkBox->setEnabled( selected );
    Year2019_checkBox->setEnabled( selected );
    Year2020_checkBox->setEnabled( selected );
    Year2021_checkBox->setEnabled( selected );
    Year2022_checkBox->setEnabled( selected );
}
void DownloadManagerDialog::SelectAllYears()
{
    selectAllYears(true);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::SelectNoneYear()
{
    selectAllYears(false);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::EnableAllYears()
{
    enableAllYears(true);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::DisableAllYears()
{
    enableAllYears(false);
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doDownloadManagerDialog( QString &s_DownloadPath, QString &s_FTPServer, QString &s_User,
                                         QString &s_Password, bool &b_DecompressFiles, bool &b_CheckFiles,
                                         bool &b_CheckAvailability, bool b_Station[MAX_NUM_OF_STATIONS+1],
                                         bool b_Month[MAX_NUM_OF_MONTHS+1], bool b_Year[MAX_NUM_OF_YEARS+1] )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************************************

    DownloadManagerDialog dialog( this );

    // Stations
    dialog.ABS_checkBox->setChecked( b_Station[ABS] );
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
    dialog.CAP_checkBox->setChecked( b_Station[CAP] );
    dialog.CAR_checkBox->setChecked( b_Station[CAR] );
    dialog.CLH_checkBox->setChecked( b_Station[CLH] );
    dialog.CNR_checkBox->setChecked( b_Station[CNR] );
    dialog.COC_checkBox->setChecked( b_Station[COC] );
    dialog.CYL_checkBox->setChecked( b_Station[CYL] );
    dialog.DAA_checkBox->setChecked( b_Station[DAA] );
    dialog.DAR_checkBox->setChecked( b_Station[DAR] );
    dialog.DOM_checkBox->setChecked( b_Station[DOM] );
    dialog.DON_checkBox->setChecked( b_Station[DON] );
    dialog.DRA_checkBox->setChecked( b_Station[DRA] );
    dialog.DWN_checkBox->setChecked( b_Station[DWN] );
    dialog.E13_checkBox->setChecked( b_Station[E13] );
    dialog.ENA_checkBox->setChecked( b_Station[ENA] );
    dialog.EUR_checkBox->setChecked( b_Station[EUR] );
    dialog.FLO_checkBox->setChecked( b_Station[FLO] );
    dialog.FPE_checkBox->setChecked( b_Station[FPE] );
    dialog.FUA_checkBox->setChecked( b_Station[FUA] );
    dialog.GAN_checkBox->setChecked( b_Station[GAN] );
    dialog.GCR_checkBox->setChecked( b_Station[GCR] );
    dialog.GIM_checkBox->setChecked( b_Station[GIM] );
    dialog.GOB_checkBox->setChecked( b_Station[GOB] );
    dialog.GUR_checkBox->setChecked( b_Station[GUR] );
    dialog.GVN_checkBox->setChecked( b_Station[GVN] );
    dialog.HOW_checkBox->setChecked( b_Station[HOW] );
    dialog.ILO_checkBox->setChecked( b_Station[ILO] );
    dialog.ISH_checkBox->setChecked( b_Station[ISH] );
    dialog.IZA_checkBox->setChecked( b_Station[IZA] );
    dialog.KWA_checkBox->setChecked( b_Station[KWA] );
    dialog.LAU_checkBox->setChecked( b_Station[LAU] );
    dialog.LER_checkBox->setChecked( b_Station[LER] );
    dialog.LIN_checkBox->setChecked( b_Station[LIN] );
    dialog.LLN_checkBox->setChecked( b_Station[LLN] );
    dialog.LRC_checkBox->setChecked( b_Station[LRC] );
    dialog.LYU_checkBox->setChecked( b_Station[LYU] );
    dialog.MAN_checkBox->setChecked( b_Station[MAN] );
    dialog.MNM_checkBox->setChecked( b_Station[MNM] );
    dialog.INO_checkBox->setChecked( b_Station[INO] );
    dialog.NAU_checkBox->setChecked( b_Station[NAU] );
    dialog.NEW_checkBox->setChecked( b_Station[NEW] );
    dialog.NPT_checkBox->setChecked( b_Station[NPT] );
    dialog.NYA_checkBox->setChecked( b_Station[NYA] );
    dialog.OHY_checkBox->setChecked( b_Station[OHY] );
    dialog.PAL_checkBox->setChecked( b_Station[PAL] );
    dialog.PAR_checkBox->setChecked( b_Station[PAR] );
    dialog.PAY_checkBox->setChecked( b_Station[PAY] );
    dialog.PSA_checkBox->setChecked( b_Station[PSA] );
    dialog.PSU_checkBox->setChecked( b_Station[PSU] );
    dialog.PTR_checkBox->setChecked( b_Station[PTR] );
    dialog.REG_checkBox->setChecked( b_Station[REG] );
    dialog.RLM_checkBox->setChecked( b_Station[RLM] );
    dialog.RUN_checkBox->setChecked( b_Station[RUN] );
    dialog.SAP_checkBox->setChecked( b_Station[SAP] );
    dialog.SBO_checkBox->setChecked( b_Station[SBO] );
    dialog.SEL_checkBox->setChecked( b_Station[SEL] );
    dialog.SMS_checkBox->setChecked( b_Station[SMS] );
    dialog.SON_checkBox->setChecked( b_Station[SON] );
    dialog.SOV_checkBox->setChecked( b_Station[SOV] );
    dialog.SPO_checkBox->setChecked( b_Station[SPO] );
    dialog.SXF_checkBox->setChecked( b_Station[SXF] );
    dialog.SYO_checkBox->setChecked( b_Station[SYO] );
    dialog.TAM_checkBox->setChecked( b_Station[TAM] );
    dialog.TAT_checkBox->setChecked( b_Station[TAT] );
    dialog.TIK_checkBox->setChecked( b_Station[TIK] );
    dialog.TIR_checkBox->setChecked( b_Station[TIR] );
    dialog.TNB_checkBox->setChecked( b_Station[TNB] );
    dialog.TOR_checkBox->setChecked( b_Station[TOR] );
    dialog.XIA_checkBox->setChecked( b_Station[XIA] );
    dialog.YUS_checkBox->setChecked( b_Station[YUS] );
    //dialog.ZVE_checkBox->setChecked( b_Station[ZVE] );







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
    dialog.Year2020_checkBox->setChecked( b_Year[29] );
    dialog.Year2021_checkBox->setChecked( b_Year[30] );
    dialog.Year2022_checkBox->setChecked( b_Year[31] );

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
        b_Station[ABS]  = dialog.ABS_checkBox->isChecked();
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
        b_Station[CAP]  = dialog.CAP_checkBox->isChecked();
        b_Station[CAR]  = dialog.CAR_checkBox->isChecked();
        b_Station[CLH]  = dialog.CLH_checkBox->isChecked();
        b_Station[CNR]  = dialog.CNR_checkBox->isChecked();
        b_Station[COC]  = dialog.COC_checkBox->isChecked();
        b_Station[CYL]  = dialog.CYL_checkBox->isChecked();
        b_Station[DAA]  = dialog.DAA_checkBox->isChecked();
        b_Station[DAR]  = dialog.DAR_checkBox->isChecked();
        b_Station[DOM]  = dialog.DOM_checkBox->isChecked();
        b_Station[DON]  = dialog.DON_checkBox->isChecked();
        b_Station[DRA]  = dialog.DRA_checkBox->isChecked();
        b_Station[DWN]  = dialog.DWN_checkBox->isChecked();
        b_Station[E13]  = dialog.E13_checkBox->isChecked();
        b_Station[ENA]  = dialog.ENA_checkBox->isChecked();
        b_Station[EUR]  = dialog.EUR_checkBox->isChecked();
        b_Station[FLO]  = dialog.FLO_checkBox->isChecked();
        b_Station[FPE]  = dialog.FPE_checkBox->isChecked();
        b_Station[FUA]  = dialog.FUA_checkBox->isChecked();
        b_Station[GAN]  = dialog.GAN_checkBox->isChecked();
        b_Station[GCR]  = dialog.GCR_checkBox->isChecked();
        b_Station[GIM]  = dialog.GIM_checkBox->isChecked();
        b_Station[GOB]  = dialog.GOB_checkBox->isChecked();
        b_Station[GUR]  = dialog.GUR_checkBox->isChecked();
        b_Station[GVN]  = dialog.GVN_checkBox->isChecked();
        b_Station[HOW]  = dialog.HOW_checkBox->isChecked();
        b_Station[ILO]  = dialog.ILO_checkBox->isChecked();
        b_Station[ISH]  = dialog.ISH_checkBox->isChecked();
        b_Station[IZA]  = dialog.IZA_checkBox->isChecked();
        b_Station[KWA]  = dialog.KWA_checkBox->isChecked();
        b_Station[LAU]  = dialog.LAU_checkBox->isChecked();
        b_Station[LER]  = dialog.LER_checkBox->isChecked();
        b_Station[LIN]  = dialog.LIN_checkBox->isChecked();
        b_Station[LLN]  = dialog.LLN_checkBox->isChecked();
        b_Station[LRC]  = dialog.LRC_checkBox->isChecked();
        b_Station[LYU]  = dialog.LYU_checkBox->isChecked();
        b_Station[MAN]  = dialog.MAN_checkBox->isChecked();
        b_Station[MNM]  = dialog.MNM_checkBox->isChecked();
        b_Station[INO]  = dialog.INO_checkBox->isChecked();
        b_Station[NAU]  = dialog.NAU_checkBox->isChecked();
        b_Station[NEW]  = dialog.NEW_checkBox->isChecked();
        b_Station[NPT]  = dialog.NPT_checkBox->isChecked();
        b_Station[NYA]  = dialog.NYA_checkBox->isChecked();
        b_Station[OHY]  = dialog.OHY_checkBox->isChecked();
        b_Station[PAL]  = dialog.PAL_checkBox->isChecked();
        b_Station[PAR]  = dialog.PAR_checkBox->isChecked();
        b_Station[PAY]  = dialog.PAY_checkBox->isChecked();
        b_Station[PSA]  = dialog.PSA_checkBox->isChecked();
        b_Station[PSU]  = dialog.PSU_checkBox->isChecked();
        b_Station[PTR]  = dialog.PTR_checkBox->isChecked();
        b_Station[REG]  = dialog.REG_checkBox->isChecked();
        b_Station[RLM]  = dialog.RLM_checkBox->isChecked();
        b_Station[RUN]  = dialog.RUN_checkBox->isChecked();
        b_Station[SAP]  = dialog.SAP_checkBox->isChecked();
        b_Station[SBO]  = dialog.SBO_checkBox->isChecked();
        b_Station[SEL]  = dialog.SEL_checkBox->isChecked();
        b_Station[SMS]  = dialog.SMS_checkBox->isChecked();
        b_Station[SON]  = dialog.SON_checkBox->isChecked();
        b_Station[SOV]  = dialog.SOV_checkBox->isChecked();
        b_Station[SPO]  = dialog.SPO_checkBox->isChecked();
        b_Station[SXF]  = dialog.SXF_checkBox->isChecked();
        b_Station[SYO]  = dialog.SYO_checkBox->isChecked();
        b_Station[TAM]  = dialog.TAM_checkBox->isChecked();
        b_Station[TAT]  = dialog.TAT_checkBox->isChecked();
        b_Station[TIK]  = dialog.TIK_checkBox->isChecked();
        b_Station[TIR]  = dialog.TIR_checkBox->isChecked();
        b_Station[TNB]  = dialog.TNB_checkBox->isChecked();
        b_Station[TOR]  = dialog.TOR_checkBox->isChecked();
        b_Station[XIA]  = dialog.XIA_checkBox->isChecked();
        b_Station[YUS]  = dialog.YUS_checkBox->isChecked();
        //b_Station[ZVE]  = dialog.ZVE_checkBox->isChecked();






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
        b_Year[29]	= dialog.Year2020_checkBox->isChecked();
        b_Year[30]	= dialog.Year2021_checkBox->isChecked();
        b_Year[31]	= dialog.Year2022_checkBox->isChecked();

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
        i_DialogResult      = QDialog::Rejected;
        break;

    default:
        i_DialogResult      = QDialog::Rejected;
        break;
    }

    posDialog = dialog.pos();

    return( i_DialogResult );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

void DownloadManagerDialog::on_CheckAvailability_checkBox_toggled()
{
    if ( CheckAvailability_checkBox->isChecked() == true )
    {
        DecompressFiles_checkBox->setEnabled( false );
        CheckFiles_checkBox->setEnabled( false );

        DisableAllYears();
        DisableAllMonths();
    }
    else
    {
        DecompressFiles_checkBox->setEnabled( true );
        CheckFiles_checkBox->setEnabled( true );

        EnableAllYears();
        EnableAllMonths();
    }
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
