// ***********************************************************************************************
// *                                                                                             *
// * AboutDialog.cpp - About dialog of program                                                   *
// *                                                                                             *
// * Dr. Rainer Sieger - 2012-01-27                                                              *
// *                                                                                             *
// ***********************************************************************************************

#include <QtWidgets>

#include "AboutDialog.h"

AboutDialog::AboutDialog( QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
}
