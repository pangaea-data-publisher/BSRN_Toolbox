#ifndef FORMATFORMATTEDOPTIONSDIALOG_H
#define FORMATFORMATTEDOPTIONSDIALOG_H

#include <QDialog>

#include "ui_formatformattedoptionsdialog.h"

class FormatFormattedOptionsDialog : public QDialog, public Ui::FormatFormattedOptionsDialog
{
    Q_OBJECT

public:
    FormatFormattedOptionsDialog( QWidget *parent = 0 );
};

#endif
