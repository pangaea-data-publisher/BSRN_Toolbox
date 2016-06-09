#include <QtWidgets>
#include "Application.h"
#include "FormatFormattedOptionsDialog.h"

FormatFormattedOptionsDialog::FormatFormattedOptionsDialog( QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    QRegExp regExp("[0-9]\\d{0,2}");
    fieldWidth_lineEdit->setValidator(new QRegExpValidator( regExp, this ) );

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doFormatFormattedOptionsDialog( int &i_FieldAlignment, int &i_FieldWidth, QString &s_MissingValue )
{
    int i_DialogResult = QDialog::Rejected;

// ***********************************************************************************************************************

    FormatFormattedOptionsDialog dialog( this );

    switch ( i_FieldAlignment )
    {
        case QTextStream::AlignRight:
            dialog.alignRight_radioButton->setChecked( true );
            break;
        default:
            dialog.alignLeft_radioButton->setChecked( true );
            break;
    }

    dialog.fieldWidth_lineEdit->setText( num2str( i_FieldWidth ) );
    dialog.missingValue_lineEdit->setText( s_MissingValue );
    dialog.fieldWidth_lineEdit->selectAll();

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( dialog.sizeHint() );
    dialog.show();

// ******************************************************************************************************

    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        if ( dialog.alignLeft_radioButton->isChecked() == true )
            i_FieldAlignment = QTextStream::AlignLeft;

        if ( dialog.alignRight_radioButton->isChecked() == true )
            i_FieldAlignment = QTextStream::AlignRight;

        i_FieldWidth = (int) dialog.fieldWidth_lineEdit->text().toInt();

        if ( i_FieldWidth < 0 )
            i_FieldWidth = 1;

        if ( i_FieldWidth > 100 )
            i_FieldWidth = 100;

        s_MissingValue = dialog.missingValue_lineEdit->text();

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
