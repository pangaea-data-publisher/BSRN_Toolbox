#ifndef SELECTPARAMETERSDIALOG_H
#define SELECTPARAMETERSDIALOG_H

#include "ui_selectparametersdialog.h"

class SelectParametersDialog : public QDialog, public Ui::SelectParametersDialog
{
Q_OBJECT

public:
    SelectParametersDialog( QStringList ParameterList, QStringList ItemListLeft, QStringList ItemListRight, int n = 1000, QWidget *parent = 0 );

    void enableOKButton();

private slots:
    void selectAll();
    void deselectAll();
    void sortList();
    void slotLeft2Right();
    void slotRight2Left();

private:
    int         i_maxNumOfItems;
    QStringList list;
};

#endif
