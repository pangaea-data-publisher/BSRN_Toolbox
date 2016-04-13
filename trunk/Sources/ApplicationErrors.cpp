/* ApplicationErrors.cpp      */
/* 2008-03-27                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! Errorhandling */

void MainWindow::onError( const int err )
{
    switch ( err )
    {
    case _APPBREAK_:      // Progress aborted
        break;
    case _ERROR_:         // Error
        break;
    case _NOERROR_:       // No error
        break;
    case _CHOOSEABORTED_: // Choose aborted
        break;
    case _FILENOEXISTS_: // File not exists
        QMessageBox::information( this, getApplicationName( true ), tr( "Import file not exists" ) );
        break;
    case _NODATAFOUND_:
        QMessageBox::information( this, getApplicationName( true ), tr( "No data found. Read the station-to-archive\nfile format description (hdl:10013/epic.42596)" ) );
        break ;

    case -10:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't open file.\nPossible locked by another application." ) );
        break;
    case -11:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't open BSRN IDs database.\nPlease contact rsieger@pangaea.de" ) );
        break;
    case -20:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't create file.\nPlease contact rsieger@pangaea.de" ) );
        break ;
    case -30:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't find BSRN_fcheck\nPlease contact rsieger@pangaea.de" ) );
        break ;
    case -31:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't find BSRN_fcheck.exe\nPlease contact rsieger@pangaea.de" ) );
        break ;
    case -32:
        QMessageBox::information( this, getApplicationName( true ), tr( "Can't find BSRN_fcheck\nPlease contact rsieger@pangaea.de" ) );
        break ;
    case -40:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. Read the station-to-archive\nfile format description (hdl:10013/epic.42596)" ) );
        break ;
    case -41:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. Please check filename\nand date of data in the second line." ) );
        break ;
    case -42:
        QMessageBox::warning( this, getApplicationName( true ), tr( "The format of the BSRN IDs database is\nwrong. Please contact rsieger@pangaea.de" ) );
        break;
    case -50:
        QMessageBox::information( this, getApplicationName( true ), tr( "Select more the one file!" ) );
        break ;
    case -60:
        QMessageBox::information( this, getApplicationName( true ), tr( "Download of BSRN IDs database fails.\nPlease check your connection to the\ninternet and refresh the database again." ) );
        break;
    case -61:
        QMessageBox::information( this, getApplicationName( true ), tr( "Download of BSRN dataset IDs database\nfails. Please check your connection to the\ninternet and refresh the database again." ) );
        break;
    case -70:
        QMessageBox::information( this, getApplicationName( true ), tr( "Maximum number of methods was reached.\nPlease contact rsieger@pangaea.de" ) );
        break;
    case -71:
        QMessageBox::information( this, getApplicationName( true ), tr( "Maximum number of datasets was reached.\nPlease contact rsieger@pangaea.de" ) );
        break;

    case NODATAFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "No data found. The qualtiy check routine/ndoes not work with station-to-archive files/nPlease read the manual first." ) );
        break ;
    case DATETIMENOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'Date/Time'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case LATITUDENOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'Latitude'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case LONGITUDENOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'Longitude'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case ELEVATIONNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'Elevation [m]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case HEIGHTNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'Height [m]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case SWDNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'SWD [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case DIRNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'DIR [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case DIFNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'DIF [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case LWDNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'LWD [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case SWUNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'SWU [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case LWUNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'LWU [W/m**2]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case T2NOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'T2 [°C]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case RHNOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'RH [%]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;
    case POPOPOPONOTFOUND:
        QMessageBox::information( this, getApplicationName( true ), tr( "Wrong format. File does not contain\nparameter 'PoPoPoPo [hPa]'.\nThe qualtiy check routine does not work with station-to-archive files. Please read the manual first." ) );
        break ;

    default :
        QMessageBox::information( this, getApplicationName( true ), tr( "Unknown error.\nPlease contact rsieger@pangaea.de" ) );
        break ;
    }
}

