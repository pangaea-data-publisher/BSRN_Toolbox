/* httpGet.cpp                */
/* 2008-05-20                 */
/* Dr. Rainer Sieger          */

#include <QMessagebox>
#include <QtNetwork>
#include <QHttp>

#include "Application.h"

/*! @brief Liest eine Datei im Internet
*
*   @param url URL der zu ladenen Datei
*   @param s_FilenameOut Namen der Ausgabedatei.
*
*   @return Fehlercode, 0 = kein Fehler, -1 = URL ist nicht OK, -2 = URL ist keine http-Adresse, -3 = Der Pfad ist nicht vollstaendig, -4 = Datei laesst sich nicht oeffnen
*/

int MainWindow::httpGetFile( const QUrl &url, const QString& s_FilenameOut )
{
    if ( url.isValid() == FALSE )
        return ( -1 );

	if ( url.scheme().toLower() != "http" )
        return ( -2 );

    if ( url.path().isEmpty() == TRUE )
        return ( -3 );

	file_BSRN_IDs.setFileName( s_FilenameOut );
    
	if ( file_BSRN_IDs.open( QIODevice::WriteOnly  | QIODevice::Text ) == FALSE)
        return ( -4 );

	http.setHost( url.host(), url.port(80) );
    http.get( url.path(), &file_BSRN_IDs );
    http.close();

    return( 0 );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Slot, wird am Ende der Datenuebertragung aufgerufen. Schliessen der Datei. Lesen der IDs.
*
* @param error http Fehler
*/

void MainWindow::httpDone( bool error )
{
    file_BSRN_IDs.close();

	readIDs();

	if ( error )
		QMessageBox::warning( this, getApplicationName(), tr( "Cannot download BSRN IDs file.\nThe default ID file was used." ) );
}
