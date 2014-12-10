#ifndef FORMATUNFORMATTEDOPTIONSDIALOG_H
#define FORMATUNFORMATTEDOPTIONSDIALOG_H

#include <QDialog>

#include "ui_formatunformattedoptionsdialog.h"

class FormatUnformattedOptionsDialog : public QDialog, public Ui::FormatUnformattedOptionsDialog
{
    Q_OBJECT

public:
    FormatUnformattedOptionsDialog( QWidget *parent = 0 );
};

#endif
