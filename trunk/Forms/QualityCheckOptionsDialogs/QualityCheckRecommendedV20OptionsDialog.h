#ifndef QUALITYCHECKRECOMMENDEDV20OPTIONSDIALOG_H
#define QUALITYCHECKRECOMMENDEDV20OPTIONSDIALOG_H

#include <QDialog>

class QCheckBox;
class QLabel;
class QPushButton;
class QRadioButton;


class QualityCheckRecommendedV20OptionsDialog : public QDialog
{
    Q_OBJECT;


public:
    QualityCheckRecommendedV20OptionsDialog(QWidget *parent = 0);


private slots:
    void EnableOkButton();
    void EnableValuesCheckBoxes();
    void ShowIqbal1983Info();
    void ShowSolposInfo();


public:
    QCheckBox * PhysicallyPossibleLimitsCheckBox;
    QCheckBox * ExtremelyRareLimitsCheckBox;
    QCheckBox * ComparisonsCheckBox;

    QRadioButton * Iqbal1983RadioButton;
    QRadioButton * SolposNoRefractionRadioButton;
    QRadioButton * SolposRefractionRadioButton;
    QRadioButton * Reda2008RadioButton;

    QPushButton * Iqbal1983InfoButton;
    QPushButton * SolposRefractionInfoButton;
    QPushButton * SolposNoRefractionInfoButton;

    QCheckBox * CodesCheckBox;
    QCheckBox * ValuesCleanedCheckBox;
    QCheckBox * ValuesOriginalCheckBox;
    QCheckBox * AuxiliaryDataCheckBox;
    QCheckBox * OneFileCheckBox;

    QPushButton * OK_pushButton;
    QPushButton * Cancel_pushButton;
};


#endif
