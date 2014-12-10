#include <QtWidgets>
#include "Application.h"
#include "FormatUnformattedOptionsDialog.h"

FormatUnformattedOptionsDialog::FormatUnformattedOptionsDialog( QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doFormatUnformattedOptionsDialog( QString &s_MissingValue, int &i_FieldDelimiter )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************************************

    FormatUnformattedOptionsDialog dialog( this );

    switch ( i_FieldDelimiter )
    {
        case _COMMA_:
            dialog.comma_radioButton->setChecked( true );
            break;
        case _SEMICOLON_:
            dialog.semicolon_radioButton->setChecked( true );
            break;
        case _SPACE_:
            dialog.space_radioButton->setChecked( true );
            break;
        default:
            dialog.tabstop_radioButton->setChecked( true );
            break;
    }

    dialog.missingValue_lineEdit->setText( s_MissingValue );
    dialog.missingValue_lineEdit->selectAll();

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( dialog.sizeHint() );
    dialog.show();

// ******************************************************************************************************

    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        s_MissingValue = dialog.missingValue_lineEdit->text();

        if ( dialog.comma_radioButton->isChecked() == true )
            i_FieldDelimiter = _COMMA_;

        if ( dialog.semicolon_radioButton->isChecked() == true )
            i_FieldDelimiter = _SEMICOLON_;

        if ( dialog.space_radioButton->isChecked() == true )
            i_FieldDelimiter = _SPACE_;

        if ( dialog.tabstop_radioButton->isChecked() == true )
            i_FieldDelimiter = _TABSTOP_;

        i_DialogResult = QDialog::Accepted;
        break;

    case QDialog::Rejected:
        break;

    default:
        break;
    }

    posDialog = dialog.pos();

    return( i_DialogResult );
}
