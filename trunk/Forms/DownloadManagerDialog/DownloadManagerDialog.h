#ifndef DOWNLOADMANAGERDIALOG_H
#define DOWNLOADMANAGERDIALOG_H

#include <QDialog>

#include "ui_downloadmanagerdialog.h"

class DownloadManagerDialog : public QDialog, public Ui::DownloadManagerDialog
{
    Q_OBJECT

public:
    DownloadManagerDialog( QWidget *parent = 0 );
    QString getDocumentDir();

private slots:
    void BrowseDownloadDirectory();
    void selectAllStations(bool selected);
    void enableAllStations(bool enabled);
    void SelectAllStations();
    void SelectNoneStation();
    void selectAllMonths(bool selected);
    void SelectAllMonth();
    void SelectNoneMonth();
    void selectAllYears(bool selected);
    void SelectAllYears();
    void SelectNoneYear();

    void enableAllYears(bool enabled);
    void EnableAllYears();
    void DisableAllYears();

    void enableAllMonths(bool enabled);
    void EnableAllMonths();
    void DisableAllMonths();

    void on_DownloadDirectory_lineEdit_textChanged();
    void on_FTPServer_lineEdit_textChanged();
    void on_User_lineEdit_textChanged();
    void on_Password_lineEdit_textChanged();
    void on_CheckAvailability_checkBox_toggled();

private:
    bool checkAccount();
};

#endif
