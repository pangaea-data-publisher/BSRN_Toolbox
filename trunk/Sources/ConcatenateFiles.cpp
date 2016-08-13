/* concatenateFiles.cpp       */
/* 2008-05-13                 */
/* Dr. Rainer Sieger          */

#include "Application.h"

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************
// 2008-05-13

/*! @brief Verbindet 1 bis n Dateien zu einer neuen mit dem Namen FilenameOut. Die Inputdateien werden geloescht, wenn b_RemoveFile = true.
*
*   @param s_FilenameOut Name der neuen Datei.
*   @param sl_FilenameList Liste aller zu verbindenen Dateien.
*   @param s_ProgressMessage Ausgabetext fuer ProgressDialog->
*   @param i_SkipNFirstLines Anzahl der Zeilen die ab der zweiten Datei ueberlesen werden sollen.
*   @param b_deleteOriginalFiles Bestimmt ob die Inputdatei geloescht werden soll oder nicht.
*
*   @return Fehlercode
*/

int MainWindow::concatenateFiles( const QString &s_FilenameOut, const QStringList &sl_FilenameList,
                                    const QString &s_ProgressMessage, const int i_SkipNFirstLines,
                                    const bool b_deleteOriginalFiles )
{
    int   i				   = 0;
    int   j				   = 1;

    int   stopProgress	   = 0;

    bool  b_isMetadataFile = false;

    QFile fin;

// ************************************************************************************************

    QFile fout( s_FilenameOut );

    if ( fout.open( QIODevice::WriteOnly | QIODevice::Text) == false )
        return( -20 );

    QTextStream tout( &fout );

// ************************************************************************************************

    if ( ( s_FilenameOut.contains( "BSRN_LR000" ) == true ) || ( s_FilenameOut.endsWith( "BSRN_RefImp.txt" ) == true ) )
        b_isMetadataFile = true;

    if ( b_isMetadataFile == false )
        initFileProgress( sl_FilenameList.count(), sl_FilenameList.at( 0 ), s_ProgressMessage );

// ************************************************************************************************

    while ( ( i < sl_FilenameList.count() ) && ( stopProgress != _APPBREAK_ ) )
    {
        setStatusBarFileInProgress( sl_FilenameList.at( i ) );

        fin.setFileName( sl_FilenameList.at( i ) );

        if ( fin.open( QIODevice::ReadOnly | QIODevice::Text ) == true )
        {
            QTextStream tin( &fin );

            if ( ( i > 0 ) && ( i_SkipNFirstLines > 0 ) )
            {
                j = 0;

                while ( ( tin.atEnd() == false ) && ( j++ < i_SkipNFirstLines ) )
                    tin.readLine();
            }

            while ( tin.atEnd() == false )
                tout << tin.readLine() << endl;

            fin.close();

            if ( b_deleteOriginalFiles == true )
                fin.remove();
        }
        else
        {
            stopProgress = _APPBREAK_;
        }

        if ( b_isMetadataFile == false )
            stopProgress = incFileProgress( sl_FilenameList.count(), ++i );
        else
            i++;
    }

// ************************************************************************************************

    fout.close();

    if ( b_isMetadataFile == false )
        resetFileProgress( sl_FilenameList.count() );

    if ( stopProgress == _APPBREAK_ )
        return( _APPBREAK_ );

    return( _NOERROR_ );
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************
// 02.08.2003

/*! @brief Steuerung der Methode concatenateFiles */

void MainWindow::doConcatenateFiles()
{
    int	err	         = _NOERROR_;
    int stopProgress = _NOERROR_;

// **********************************************************************************************

    if ( existsFirstFile( gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList ) == true )
    {
        if ( gsl_FilenameList.count() < 2 )
            err = -50;

        if ( err == _NOERROR_ )
        {
            err = doConcatenateOptionsDialog( gi_SkipNFirstLines, gb_DeleteOriginalFiles );

            if ( err == QDialog::Accepted )
                err = concatenateFiles( gs_Path + "/" + "ConcatenatedFiles.txt", gsl_FilenameList, tr( "Concatenate files working..." ), gi_SkipNFirstLines, gb_DeleteOriginalFiles );
            else
                err = _ERROR_;
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// **********************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Concatenate files was canceled" ), false, false );

    onError( err );
}
