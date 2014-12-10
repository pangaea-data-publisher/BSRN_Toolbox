#ifndef CONCATENATEOPTIONSDIALOG_H
#define CONCATENATEOPTIONSDIALOG_H

#include <QDialog>

#include "ui_concatenateoptionsdialog.h"

class ConcatenateOptionsDialog : public QDialog, public Ui::ConcatenateOptionsDialog
{
    Q_OBJECT

public:
    ConcatenateOptionsDialog( QWidget *parent = 0 );
};

#endif
