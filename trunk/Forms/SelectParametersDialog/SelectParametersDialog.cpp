// ***********************************************************************************************
// *                                                                                             *
// * SelectParametersDialog.cpp - Selects parameter from parameter list                          *
// *                                                                                             *
// * Dr. Rainer Sieger - 2016-06-06                                                              *
// *                                                                                             *
// ***********************************************************************************************

#include <QtGui>

#include "Application.h"
#include "SelectParametersDialog.h"

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

int MainWindow::doSelectParametersDialog( const int i_mode, int *P )
{
    int         i_DialogResult          = QDialog::Rejected;
    int         i_NumOfMetadataColumns  = 5;

    QString     s_LR_Label              = "";

    QStringList sl_ParameterList;
    QStringList sl_ItemListLeft;
    QStringList sl_ItemListRight;

// ***********************************************************************************************************************

    switch( i_mode )
    {
    case LR0100:
    case LR0300:
    case LR0100plusLR0300:
    case LR0500:
        sl_ParameterList.append( tr( "Station" ) );                 // P[0]
        sl_ParameterList.append( tr( "Date/Time" ) );               // P[1]
        sl_ParameterList.append( tr( "Latitude" ) );                // P[2]
        sl_ParameterList.append( tr( "Longitude" ) );               // P[3]
        sl_ParameterList.append( tr( "Height above ground [m]" ) ); // P[4]
        break;

    default:
        break;
    }

    switch( i_mode )
    {
    case LR0100:
        sl_ParameterList.append( tr( "Short-wave downward (GLOBAL) radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave downward (GLOBAL) radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave downward (GLOBAL) radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave downward (GLOBAL) radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Direct radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Direct radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Direct radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Direct radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Diffuse radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Diffuse radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Diffuse radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Diffuse radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave downward radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave downward radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave downward radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave downward radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Air temperature [deg C]" ) );
        sl_ParameterList.append( tr( "Humidity, relative [%]" ) );
        sl_ParameterList.append( tr( "Station pressure [hPa]" ) );

        s_LR_Label = tr( "Parameters of logical record LR0100" );
        break;

    case LR0300:
    case LR0100plusLR0300:
        sl_ParameterList.append( tr( "Short-wave upward (REFLEX) radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave upward (REFLEX) radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave upward (REFLEX) radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Short-wave upward (REFLEX) radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave upward radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave upward radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave upward radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Long-wave upward radiation, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Net radiation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Net radiation, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "Net radiation, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "Net radiation, maximum [W/m**2]" ) );

        s_LR_Label = tr( "Parameters of logical record LR0300" );
        break;

    case LR0500:
        sl_ParameterList.append( tr( "UV-a global [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-a global, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-a global, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-a global, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b direct [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b direct, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b direct, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b direct, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b global [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b global, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b global, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b global, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b diffuse [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b diffuse, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b diffuse, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV-b diffuse, maximum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV upward reflected [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV upward reflected, standard deviation [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV upward reflected, minimum [W/m**2]" ) );
        sl_ParameterList.append( tr( "UV upward reflected, maximum [W/m**2]" ) );

        s_LR_Label = tr( "Parameters of logical record LR0500" );
        break;

    default:
        break;
    }

    switch ( i_mode)
    {
    case LR0100:
        for ( int i=1; i<=3; i++ )
            sl_ItemListRight.append( sl_ParameterList.at( i ) );
        break;
    case LR0300:
    case LR0100plusLR0300:
    case LR0500:
        sl_ItemListRight.append( sl_ParameterList.at( 1 ) ); // Date/Time
        break;
    default:
        for ( int i=0; i<i_NumOfMetadataColumns; i++ )
            sl_ItemListRight.append( sl_ParameterList.at( i ) );
        break;
    }

    for ( int i=i_NumOfMetadataColumns; i<sl_ParameterList.count(); i++ )
    {
        if ( ( P[i-i_NumOfMetadataColumns+1] > 0 ) &&
             ( sl_ParameterList.at( i ).contains( "standard deviation" ) == false ) &&
             ( sl_ParameterList.at( i ).contains( "minimum" ) == false ) &&
             ( sl_ParameterList.at( i ).contains( "maximum" ) == false ) )
                sl_ItemListRight.append( sl_ParameterList.at( i ) );
    }

    for ( int i=0; i<sl_ParameterList.count(); i++ )
    {
        if ( sl_ItemListRight.contains( sl_ParameterList.at( i ) ) == false )
            sl_ItemListLeft.append( sl_ParameterList.at( i ) );
    }

// ***********************************************************************************************************************

    SelectParametersDialog dialog( sl_ParameterList, sl_ItemListLeft, sl_ItemListRight, 32 );

    dialog.LR_label->setText( s_LR_Label );

    dialog.OK_pushButton->setWhatsThis( "Close dialog" );
    dialog.Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    dialog.move( posDialog );
    dialog.resize( sizeParameterDialog );
    dialog.show();

// ******************************************************************************************************

    switch ( dialog.exec() )
    {
    case QDialog::Accepted:
        for ( int i=0; i<sl_ParameterList.count(); i++ )
            P[i] = 0;

        for ( int j=0; j<dialog.lb2->count(); ++j )
        {
            for ( int i=0; i<sl_ParameterList.count(); i++ )
            {
                if ( sl_ParameterList.at( i ) == dialog.lb2->item( j )->text() )
                    P[i] = 1;
            }
        }

        i_DialogResult = QDialog::Accepted;
        break;

    case QDialog::Rejected:
        i_DialogResult = QDialog::Rejected;
        break;

    default:
        i_DialogResult = QDialog::Rejected;
        break;
    }

    posDialog           = dialog.pos();
    sizeParameterDialog = dialog.size();

    return( i_DialogResult );
}

// ***********************************************************************************************************************
// ***********************************************************************************************************************
// ***********************************************************************************************************************

SelectParametersDialog::SelectParametersDialog( QStringList sl_ParameterList, QStringList sl_ItemListLeft, QStringList sl_ItemListRight, int i_maxNumOfParameters, QWidget *parent ) : QDialog( parent )
{
    setupUi( this );

    connect(OK_pushButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(Cancel_pushButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(SelectAll_pushButton, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(DeselectAll_pushButton, SIGNAL(clicked()), this, SLOT(deselectAll()));
    connect(Left2Right_pushButton, SIGNAL(clicked()), this, SLOT(slotLeft2Right()));
    connect(Right2Left_pushButton, SIGNAL(clicked()), this, SLOT(slotRight2Left()));
    connect(lb1, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotLeft2Right()));
    connect(lb2, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotRight2Left()));

    lb1->setSelectionMode( QAbstractItemView::ExtendedSelection );
    lb2->setSelectionMode( QAbstractItemView::ExtendedSelection );

    OK_pushButton->setWhatsThis( "Close dialog" );
    Cancel_pushButton->setWhatsThis( "Cancel dialog" );

    i_maxNumOfItems = i_maxNumOfParameters;

    for ( int i=0; i<sl_ItemListLeft.count(); ++i )
        lb1->addItem( sl_ItemListLeft.at( i ) );

    for ( int i=0; i<sl_ItemListRight.count(); ++i )
        lb2->addItem( sl_ItemListRight.at( i ) );

    list = sl_ParameterList;

    enableOKButton();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/*
 * SLOT slotLeft2Right
 *
 * Move all selected items of the first ListBox into the
 * second ListBox
 */

void SelectParametersDialog::slotLeft2Right()
{
    int n = 0;

    for ( int i=0; i<lb1->count(); ++i )
    {
        if ( lb1->item( i )->isSelected() == true )
            ++n;
    }

    if ( n<i_maxNumOfItems )
    {
        n = lb1->count()-1;

        for ( int i=0; i<=n; ++i )
        {
            if ( lb1->item( i )->isSelected() == true )
                lb2->addItem( lb1->item( i )->text() );
        }

        for ( int i=n; i>=0; --i )
        {
            if ( lb1->item( i )->isSelected() == true )
                lb1->takeItem( i );
        }
    }
    else
    {
        QMessageBox::information( this, "BSRN Toolbox", QString( "The application you have choosen\nsupports a maximum of %1 parameters." ).arg( i_maxNumOfItems ) );

        for ( int i=0; i<lb1->count(); ++i )
            lb1->setCurrentItem( lb1->item( i ), QItemSelectionModel::Deselect );
    }

    enableOKButton();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/*
 * SLOT slotRight2Left
 *
 * Move all selected items of the second ListBox into the
 * first ListBox
 */

void SelectParametersDialog::slotRight2Left()
{
    int n = lb2->count()-1;

    for ( int i=0; i<=n; ++i )
    {
        if ( lb2->item( i )->isSelected() == true )
            lb1->addItem( lb2->item( i )->text() );
    }

    for ( int i=n; i>=0; --i )
    {
        if ( lb2->item( i )->isSelected() == true )
            lb2->takeItem( i );
    }

    enableOKButton();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void SelectParametersDialog::selectAll()
{
    for ( int i=0; i<lb1->count(); ++i )
        lb1->setCurrentItem( lb1->item( i ), QItemSelectionModel::Select );

    slotLeft2Right();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void SelectParametersDialog::deselectAll()
{
    sortList();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void SelectParametersDialog::sortList()
{
    while ( lb1->count() > 0 )
        lb1->takeItem( 0 );

    while ( lb2->count() > 0 )
        lb2->takeItem( 0 );

    for ( int i=0; i<list.count(); ++i )
        lb1->addItem( list.at( i ) );

    enableOKButton();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void SelectParametersDialog::enableOKButton()
{
    if ( lb2->count() <= i_maxNumOfItems )
    {
        SelectAll_pushButton->setDefault( false );
        OK_pushButton->setEnabled( true );
        OK_pushButton->setDefault( true );
        OK_pushButton->setFocus();
    }
    else
    {
        OK_pushButton->setEnabled( false );
        OK_pushButton->setDefault( false );
        SelectAll_pushButton->setDefault( true );
        SelectAll_pushButton->setFocus();

        if ( lb2->count() > i_maxNumOfItems )
            QMessageBox::information( this, "BSRN Toolbox", QString( "The application you have choosen\nsupports a maximum of %1 parameters.\nPlease remove %2 parameter(s) from the\nlist or click on cancel." ).arg( i_maxNumOfItems ).arg( lb2->count() - i_maxNumOfItems ) );
    }
}
