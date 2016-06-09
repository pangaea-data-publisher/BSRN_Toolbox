#include <QtWidgets>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>

#include "Application.h"
#include "QualityCheckRecommendedV20OptionsDialog.h"

// Constructor
QualityCheckRecommendedV20OptionsDialog::QualityCheckRecommendedV20OptionsDialog(QWidget * parent)
    : QDialog(parent)
{
    // Widgets definitions
    QGroupBox * ChecksGroupBox = new QGroupBox(tr("Checks"));
    PhysicallyPossibleLimitsCheckBox = new QCheckBox(tr("&Physically possible limits"));
    ExtremelyRareLimitsCheckBox = new QCheckBox(tr("&Extremely rare limits"));
    ComparisonsCheckBox = new QCheckBox(tr("&Comparisons"));

    QGroupBox * AlgorithmGroupBox = new QGroupBox(tr("Algorithm for Astronomical Auxiliary Data"));
    Iqbal1983RadioButton = new QRadioButton(tr("&Iqbal 1983"));
    SolposNoRefractionRadioButton = new QRadioButton(tr("&Solpos without Refraction"));
    SolposRefractionRadioButton = new QRadioButton(tr("Solpos with &Refraction"));
    Reda2008RadioButton = new QRadioButton(tr("Reda 2008"));

    Iqbal1983InfoButton = new QPushButton(tr("Info"));
    SolposRefractionInfoButton = new QPushButton(tr("Info"));
    SolposNoRefractionInfoButton = new QPushButton(tr("Info"));

    QGroupBox * OutputGroupBox = new QGroupBox(tr("Output"));
    AuxiliaryDataCheckBox = new QCheckBox(tr("&Auxiliary data"));
    ValuesCleanedCheckBox = new QCheckBox(tr("C&leaned data"));
    ValuesOriginalCheckBox = new QCheckBox(tr("Ori&ginal data"));
    CodesCheckBox = new QCheckBox(tr("&Quality codes"));
    OneFileCheckBox = new QCheckBox(tr("One co&mbined output file"));
    EnableValuesCheckBoxes();

    OK_pushButton = new QPushButton(tr("&OK"));
    OK_pushButton->setDefault(true);
    EnableOkButton();
    Cancel_pushButton = new QPushButton(tr("Cancel"));


    // Connect signals and slots
    connect(PhysicallyPossibleLimitsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(ExtremelyRareLimitsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(ComparisonsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(Iqbal1983InfoButton, SIGNAL(clicked()), this, SLOT(ShowIqbal1983Info()));
    connect(SolposRefractionInfoButton, SIGNAL(clicked()), this, SLOT(ShowSolposInfo()));
    connect(SolposNoRefractionInfoButton, SIGNAL(clicked()), this, SLOT(ShowSolposInfo()));
    connect(AuxiliaryDataCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(CodesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(ValuesCleanedCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(ValuesCleanedCheckBox,SIGNAL(stateChanged(int)), this, SLOT(EnableValuesCheckBoxes()));
    connect(ValuesOriginalCheckBox, SIGNAL(stateChanged(int)), this, SLOT(EnableOkButton()));
    connect(ValuesOriginalCheckBox,SIGNAL(stateChanged(int)), this, SLOT(EnableValuesCheckBoxes()));
    connect(OneFileCheckBox,SIGNAL(stateChanged(int)), this, SLOT(EnableValuesCheckBoxes()));
    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));


    // Layout
    QVBoxLayout * ChecksLayout = new QVBoxLayout;
    ChecksLayout->addWidget(PhysicallyPossibleLimitsCheckBox);
    ChecksLayout->addWidget(ExtremelyRareLimitsCheckBox);
    ChecksLayout->addWidget(ComparisonsCheckBox);
    ChecksGroupBox->setLayout(ChecksLayout);

    QGridLayout * AlgorithmLayout = new QGridLayout;
    AlgorithmLayout->addWidget(Iqbal1983RadioButton,1,1);
    AlgorithmLayout->addWidget(Iqbal1983InfoButton,1,2);
    AlgorithmLayout->addWidget(SolposRefractionRadioButton,2,1);
    AlgorithmLayout->addWidget(SolposRefractionInfoButton,2,2);
    AlgorithmLayout->addWidget(SolposNoRefractionRadioButton,3,1);
    AlgorithmLayout->addWidget(SolposNoRefractionInfoButton,3,2);
    AlgorithmGroupBox->setLayout(AlgorithmLayout);

    QVBoxLayout * OutputLayout = new QVBoxLayout;
    OutputLayout->addWidget(AuxiliaryDataCheckBox);
    OutputLayout->addWidget(ValuesCleanedCheckBox);
    OutputLayout->addWidget(ValuesOriginalCheckBox);
    OutputLayout->addWidget(CodesCheckBox);
    OutputLayout->insertSpacing(4,10);
    OutputLayout->addWidget(OneFileCheckBox);
    OutputGroupBox->setLayout(OutputLayout);

    QHBoxLayout * ButtonLayout = new QHBoxLayout;
    ButtonLayout->addStretch();
    ButtonLayout->addWidget(OK_pushButton);
    ButtonLayout->addWidget(Cancel_pushButton);
    ButtonLayout->addStretch();

    QVBoxLayout * MainLayout = new QVBoxLayout;
    MainLayout->addWidget(ChecksGroupBox);
    MainLayout->addWidget(AlgorithmGroupBox);
    MainLayout->addWidget(OutputGroupBox);
    MainLayout->addLayout(ButtonLayout);

    setLayout(MainLayout);
    setWindowTitle(tr("Quality Check Options - BSRN Recommended V2.0"));
    setFixedSize(sizeHint());
}


// Slot enableOkButton
void QualityCheckRecommendedV20OptionsDialog::EnableOkButton()
{
    OK_pushButton->setEnabled((Iqbal1983RadioButton            ->isChecked()     ^      \
                               SolposRefractionRadioButton     ->isChecked()     ^      \
                               SolposNoRefractionRadioButton   ->isChecked()       ) && \
                              (CodesCheckBox                   ->isChecked()     ||     \
                               ValuesCleanedCheckBox           ->isChecked()     ||     \
                               (ValuesOriginalCheckBox         ->isChecked() &&         \
                                ValuesOriginalCheckBox         ->isEnabled()   ) ||     \
                                AuxiliaryDataCheckBox          ->isChecked()       )    );
}


// Slot enableOkButton
void QualityCheckRecommendedV20OptionsDialog::EnableValuesCheckBoxes()
{
    ValuesCleanedCheckBox->setEnabled(!OneFileCheckBox->isChecked() || !ValuesOriginalCheckBox->isChecked());
    ValuesOriginalCheckBox->setEnabled(OneFileCheckBox->isChecked() && !ValuesCleanedCheckBox->isChecked());
    if (!ValuesOriginalCheckBox->isEnabled()) ValuesOriginalCheckBox->setChecked(false);
}


// Slot ShowIqbal1983Info
void QualityCheckRecommendedV20OptionsDialog::ShowIqbal1983Info()
{
    QMessageBox::information(this,tr("Algorithm Iqbal1983"),tr("This algorithm calculates auxiliary parameter as\n"
                                                               "described in:\n"
                                                               "Iqbal, Muhammad: An Introduction to Solar Radiation,\n"
                                                               "Academic Press Canada, 1983, ISBN 0-12-373750-8\n\n"
                                                               "Solar constant used is 1376 W/m2. There is no\n"
                                                               "atmospheric refraction taken into account."));
}


// Slot ShowSolposInfo
void QualityCheckRecommendedV20OptionsDialog::ShowSolposInfo()
{
    QMessageBox::information(this,tr("Algorithm SOLPOS"),tr("This algorithm calculates auxiliary parameter using\n"
                                                            "the code 'SOLPOS' supplied by the National Renewable\n"
                                                            "Energy Laboratory, USA. The code and documentation\n"
                                                            "can be found at:\n"
                                                            "http://rredc.nrel.gov/solar/codesandalgorithms/solpos/\n\n"
                                                            "Solar constant used is 1376 W/m2. The algorithm supports\n"
                                                            "atmospheric refraction. For this, the input parameter\n"
                                                            "'Station pressure [hPa] (PoPoPoPo)' and\n"
                                                            "'Air temperature at 2 m height [°C] (T2)' are required."));
}


// do-Function
int MainWindow::doQualityCheckRecommendedV20OptionsDialog(bool & b_CheckPhysicallyPossibleLimits, bool & b_CheckExtremelyRareLimits, bool & b_CheckComparisons, QString & s_AuxiliaryDataAlgorithm, bool & b_OutputCodes, bool & b_OutputCleanedValues, bool & b_OutputOriginalValues, bool & b_OutputAuxiliaryData, bool & b_OutputOneFile)
{
    // Initialise dialog
    int i_DialogResult = QDialog::Rejected;

    QualityCheckRecommendedV20OptionsDialog dialog( this );

    // Set states
    dialog.PhysicallyPossibleLimitsCheckBox->setChecked(b_CheckPhysicallyPossibleLimits);
    dialog.ExtremelyRareLimitsCheckBox->setChecked(b_CheckExtremelyRareLimits);
    dialog.ComparisonsCheckBox->setChecked(b_CheckComparisons);
    if      (s_AuxiliaryDataAlgorithm == "Iqbal1983"                ) dialog.Iqbal1983RadioButton         ->setChecked(true);
    else if (s_AuxiliaryDataAlgorithm == "Solpos with Refraction"   ) dialog.SolposRefractionRadioButton  ->setChecked(true);
    else if (s_AuxiliaryDataAlgorithm == "Solpos without Refraction") dialog.SolposNoRefractionRadioButton->setChecked(true);

    /* Holger
    dialog.CodesCheckBox->setChecked(b_OutputCodes);
    dialog.ValuesCleanedCheckBox->setChecked(b_OutputCleanedValues);
    dialog.ValuesOriginalCheckBox->setChecked(b_OutputOriginalValues);
    dialog.AuxiliaryDataCheckBox->setChecked(b_OutputAuxiliaryData);
    dialog.OneFileCheckBox->setChecked(b_OutputOneFile);
    */

    // + Rainer, nun wird b_OutputOriginalValues auch richtig eingetragen. SLOT(EnableOkButton(), ausgeloest von ValuesOriginalCheckBox, SIGNAL(stateChanged(int) verwendet den aktuellen Status von OneFileCheckBox. Daher muss erst OneFileCheckBox richtig gesetzt sein.
    dialog.OneFileCheckBox->setChecked(b_OutputOneFile);
    dialog.CodesCheckBox->setChecked(b_OutputCodes);
    dialog.ValuesCleanedCheckBox->setChecked(b_OutputCleanedValues);
    dialog.ValuesOriginalCheckBox->setChecked(b_OutputOriginalValues);
    dialog.AuxiliaryDataCheckBox->setChecked(b_OutputAuxiliaryData);
    // - Rainer

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( dialog.sizeHint() );
    dialog.show();

    // Get states
    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        b_CheckPhysicallyPossibleLimits = dialog.PhysicallyPossibleLimitsCheckBox->isChecked();
        b_CheckExtremelyRareLimits      = dialog.ExtremelyRareLimitsCheckBox     ->isChecked();
        b_CheckComparisons              = dialog.ComparisonsCheckBox             ->isChecked();
        if      (dialog.Iqbal1983RadioButton         ->isChecked()) s_AuxiliaryDataAlgorithm = "Iqbal1983";
        else if (dialog.SolposRefractionRadioButton  ->isChecked()) s_AuxiliaryDataAlgorithm = "Solpos with Refraction";
        else if (dialog.SolposNoRefractionRadioButton->isChecked()) s_AuxiliaryDataAlgorithm = "Solpos without Refraction";
        b_OutputCodes                   = dialog.CodesCheckBox                   ->isChecked();
        b_OutputCleanedValues           = dialog.ValuesCleanedCheckBox           ->isChecked();
        b_OutputOriginalValues          = dialog.ValuesOriginalCheckBox          ->isChecked();
        b_OutputAuxiliaryData           = dialog.AuxiliaryDataCheckBox           ->isChecked();
        b_OutputOneFile                 = dialog.OneFileCheckBox                 ->isChecked();

        i_DialogResult = QDialog::Accepted;
        break;

    case QDialog::Rejected:
        i_DialogResult = QDialog::Rejected;
        break;

    default:
        i_DialogResult = QDialog::Rejected;
        break;
    }

    posDialog = dialog.pos();

    return( i_DialogResult );
}
