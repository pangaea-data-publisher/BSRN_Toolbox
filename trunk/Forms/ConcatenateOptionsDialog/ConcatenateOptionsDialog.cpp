#include <QtWidgets>
#include "Application.h"
#include "ConcatenateOptionsDialog.h"

ConcatenateOptionsDialog::ConcatenateOptionsDialog( QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    QRegExp regExp("[0-9]\\d{0,5}");
    skipNFirstLines_lineEdit->setValidator(new QRegExpValidator(regExp, this));

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doConcatenateOptionsDialog( int &i_SkipNFirstLines, bool &b_deleteOriginalFiles )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************************************

    ConcatenateOptionsDialog dialog( this );

    dialog.skipNFirstLines_lineEdit->setText( QString( "%1" ).arg( i_SkipNFirstLines ) );
    dialog.deleteOriginalFiles_checkBox->setChecked( b_deleteOriginalFiles );
    dialog.skipNFirstLines_lineEdit->selectAll();

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( dialog.sizeHint() );
    dialog.show();

// ******************************************************************************************************

    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        i_SkipNFirstLines		= dialog.skipNFirstLines_lineEdit->text().toInt();
        b_deleteOriginalFiles	= dialog.deleteOriginalFiles_checkBox->isChecked();

        i_DialogResult          = QDialog::Accepted;
        break;

    case QDialog::Rejected:
        break;

    default:
        break;
    }

    posDialog = dialog.pos();

    return( i_DialogResult );
}
