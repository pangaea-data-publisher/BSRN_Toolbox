#ifndef BSRNDATA_H
#define BSRNDATA_H

#include<QtCore>
#include"Application.h"


class BsrnData : public QObject
{
    Q_OBJECT


public:
    BsrnData();
    BsrnData(const BsrnData & DataIn);

    void addParameter(const QString &Key, const BsrnData DataIn);
    void addParameter(const QString &Key, const QString &ParameterName, const QString &Unit, const QString &Abbreviation);
    void appendHorizontally(const BsrnData & DataToAdd);
    void appendToAbbreviations(const QString &StringToAdd, const QStringList &KeyList);
    void appendToKeys(const QString &StringToAdd, const QStringList &KeyList);
    void appendToParameters(const QString &StringToAdd, const QStringList &KeyList);
    QMap<QString, QString> getAllBsrnParameters();
    void prependHorizontally(const BsrnData & DataToAdd);
    int readFromFile(const QString &FileName);
    void readFromStringList(const QStringList &sl_Input);
    void removeOther(const QStringList &KeepList);
    void renameKey(const QString &OldKey, const QString &NewKey);
    void set(const qreal Value);
    void set(const qreal Value, BsrnData & QcCodes);
    void setDefaultOrder();
    void setSortedOrder();
    void setUnits(const QString &Unit, const QStringList &KeyList);
    qint32 size();
    void toBsrnUnits();
    void toSiUnits();
    int writeToFile(const QString &FileName);

    QMap<QString, QString> Abbreviations;
    QMap<QString, QStringList> DataDescription;
    QVector<QDateTime> DateTime;
    qreal Elevation;
    qreal Latitude;
    qreal Longitude;
    QStringList Order;
    QMap<QString, QString> Parameters;
    QMap<QString, QVector<qreal> > Values;
    QMap<QString, QString> Units;


protected:
    bool Abort;


signals:
    void lineProcessed(int Line);
    void progressMade(int Percentage);
};


#endif // BSRNDATA_H
