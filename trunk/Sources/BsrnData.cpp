#include <QtCore>
#include "BsrnData.h"
#include "Globals.h"

int NumOfSections(const QString &String);


// Constructor
BsrnData::BsrnData()
{
    Elevation = NAN;
    Latitude = NAN;
    Longitude = NAN;
}


// Constructor
BsrnData::BsrnData(const BsrnData & DataIn)
    : QObject()
{
    Abbreviations = DataIn.Abbreviations;
    DataDescription = DataIn.DataDescription;
    DateTime = DataIn.DateTime;
    Elevation = DataIn.Elevation;
    Latitude = DataIn.Latitude;
    Longitude = DataIn.Longitude;
    Order = DataIn.Order;
    Parameters = DataIn.Parameters;
    Units = DataIn.Units;
    Values = DataIn.Values;
}


// Member function: readFromStringList
void BsrnData::readFromStringList(const QStringList & sl_Input)
{
    //   Preliminaries
    int Progress = 0;
    qint32 i_Line = 0;


    //   Extract info from data description
    if (sl_Input.at(i_Line).startsWith("/* DATA DESCRIPTION:") == true)
    {
        do
        {
            i_Line++;

            //     Extract info from "Event(s):" field
            if (sl_Input.at(i_Line).startsWith("Event(s):\t") == true)
            {
                bool Ok;
                qreal Value;

                Value = sl_Input.value(i_Line).remove(QRegExp(".*LATITUDE:")).remove(QRegExp("\\*.*")).toFloat(&Ok);
                if (Ok) Latitude = Value;
                Value = sl_Input.value(i_Line).remove(QRegExp(".*LONGITUDE:")).remove(QRegExp("\\*.*")).toFloat(&Ok);
                if (Ok) Longitude = Value;
                Value = sl_Input.value(i_Line).remove(QRegExp(".*ELEVATION:")).remove(QRegExp("m(?: \\*.*)?")).toFloat(&Ok);
                if (Ok) Elevation = Value;
            }

            //     Extract info from "Parameter(s):" field
            if (sl_Input.at(i_Line).startsWith("Parameter(s):\t") == true)
            {
                do
                {
                    QString s = sl_Input.at(i_Line).section("\t",1,1).section(" * ",0,0);
                    QRegExp rx = QRegExp("^([^[]+) (?:(?:\\[)(.+)(?:\\]) )?(?:(?:\\()(.+)(?:\\)))$");
                    s.contains(rx);
                    Abbreviations[rx.cap(3)] = rx.cap(3);
                    Parameters[rx.cap(3)] = rx.cap(1);
                    Units[rx.cap(3)] = rx.cap(2);
                    Order.append(rx.cap(3));
                    i_Line++;
                }while (sl_Input.at(i_Line).startsWith("\t"));
                i_Line--;
            }

            emit lineProcessed(i_Line + 1);
            if (100 * (i_Line + 1) / sl_Input.size() > Progress)
            {
                Progress = 100 * (i_Line + 1) / sl_Input.size();
                emit progressMade(Progress);
            }
        }while (sl_Input.at(i_Line).startsWith("*/") == false && i_Line < sl_Input.size());
        i_Line++;
    }


    //   Extract info from table header line
    //     Parameter names and units if not found in header
    if (Parameters.size() != NumOfSections(sl_Input.at(i_Line)))
    {
        Abbreviations.clear();
        Parameters.clear();
        Units.clear();
        Order.clear();

        QMap<QString, QString> AllParameters = getAllBsrnParameters();

        for (int i = 0; i < NumOfSections(sl_Input.at(i_Line)); ++i)
        {
            QString s = sl_Input.at(i_Line).section("\t",i,i);
            QRegExp rx = QRegExp("^([^[]+)(?:(?: \\[)(.+)(?:\\]))?");
            s.contains(rx);
            QString Key = AllParameters.key(rx.cap(1),rx.cap(1));
            if (AllParameters.contains(Key))
                Parameters[Key] = AllParameters.value(Key);
            else
                Parameters[Key] = rx.cap(1);
            Abbreviations[Key] = Key;
            Units[Key] = rx.cap(2);
            Order.append(Key);
        }
    }
    emit lineProcessed(i_Line + 1);
    if (100 * (i_Line + 1) / sl_Input.size() > Progress)
    {
        Progress = 100 * (i_Line + 1) / sl_Input.size();
        emit progressMade(Progress);
    }
    i_Line++;


    //   Determine sizes
    qint32 HeaderSize = i_Line;
    qint32 Size = sl_Input.size() - HeaderSize;


    //   Extract data description literally
    if (sl_Input.at(0).startsWith("/* DATA DESCRIPTION:") && sl_Input.at(HeaderSize-2).startsWith("*/"))
    {
        QString Key;
        QString LastKey;
        QString Value;

        for (int i = 1; i < HeaderSize - 2; ++i)
        {
            Key = sl_Input.at(i).section("\t",0,0).remove(QRegExp(":?$"));
            Value = sl_Input.at(i).section("\t",1,1);

            if (Key.isEmpty())
                Key = LastKey;

            if (Key.isEmpty() == false)
            {
                if (Key.compare("Parameter(s)") != 0 && Key.compare("Size") != 0)
                    DataDescription[Key].append(Value);
                LastKey = Key;
            }
        }
    }


    //   Allocate memory for parameter
    for (QMap<QString,QString>::const_iterator i = Parameters.constBegin(); i != Parameters.constEnd(); ++i)
    {
        if (i.key() == "Date/Time")
            DateTime.fill(QDateTime(), Size);
        else
            Values[i.key()].fill(NAN, Size);
    }


    //   Extract field data (slow!!!)
    qint32 i_Line_start = i_Line;
    for (i_Line=i_Line_start; i_Line < sl_Input.size(); ++i_Line)
    {
        for (int i = 0; i < NumOfSections(sl_Input.at(i_Line)); ++i)
        {
            QString Key = Order.at(i);
            if (Key == "Date/Time")
                if (sl_Input.at(i_Line).section("\t",i,i).size() == 16)
                    DateTime[i_Line-HeaderSize] = QDateTime::fromString(sl_Input.at(i_Line).section("\t",i,i),"yyyy-MM-ddThh:mm");
                else
                    DateTime[i_Line-HeaderSize] = QDateTime::fromString(sl_Input.at(i_Line).section("\t",i,i),Qt::ISODate);
            else
            {
                bool Ok;
                qreal Value = sl_Input.at(i_Line).section("\t",i,i).toFloat(&Ok);
                if (Ok)
                    Values[Key][i_Line-HeaderSize] = Value;
                else
                    Values[Key][i_Line-HeaderSize] = NAN;
            }
        }
        emit lineProcessed(i_Line + 1);
        if (100 * (i_Line + 1) / sl_Input.size() > Progress)
        {
            Progress = 100 * (i_Line + 1) / sl_Input.size();
            emit progressMade(Progress);
        }
    }


    //   Extract coordinates from field data if necessary
    if (std::isnan(Latitude ) && Values.contains("Latitude" )) Latitude  = Values["Latitude" ].at(1);
    if (std::isnan(Longitude) && Values.contains("Longitude")) Longitude = Values["Longitude"].at(1);
    if (std::isnan(Elevation) && Values.contains("Elevation")) Elevation = Values["Elevation"].at(1);


    //   Check if coordinates in field data are consistent
    QVector<qreal> TestVector(this->size());
    if (Values.contains("Latitude"))
        if (Values["Latitude"] != TestVector.fill(Latitude))
            qDebug() << "Latitude not consistent";
    if (Values.contains("Longitude"))
        if (Values["Longitude"] != TestVector.fill(Longitude))
            qDebug() << "Longitude not consistent";
    if (Values.contains("Elevation"))
        if (Values["Elevation"] != TestVector.fill(Elevation))
            qDebug() << "Elevation not consistent";
}


// Member function: writeToFile
int BsrnData::writeToFile(const QString &FileName)
{
    //   Initialise file output
    QFile File(FileName);
    QTextStream ts_Output(&File);
    qint32 i_Line = 0;

    if (File.open(QIODevice::WriteOnly | QIODevice::Text) == false) return(FILENOTWRITABLE);


    //   Write data description
    if (DataDescription.size() > 0)
    {
        ts_Output << "/* DATA DESCRIPTION:" << endl;
        emit lineProcessed(i_Line++);

        for (QMap<QString, QStringList>::const_iterator i = DataDescription.constBegin(); i != DataDescription.constEnd(); ++i)
        {
            ts_Output << i.key() << ":";
            if (i.value().size() == 0)
            {
                ts_Output << "\t" << endl;
                emit lineProcessed(i_Line++);
            }
            for (int j = 0; j < i.value().size(); ++j)
            {
                ts_Output << "\t" << i.value().at(j) << endl;
                emit lineProcessed(i_Line++);
            }
        }

        ts_Output << "Parameter(s):";
        for (QStringList::ConstIterator Key = Order.constBegin(); Key != Order.constEnd(); ++Key)
        {
            ts_Output << "\t" << Parameters[*Key];
            if (Units[*Key].size() > 0) ts_Output << " [" << Units[*Key] << "]";
            ts_Output << " (" << Abbreviations[*Key] << ")" << endl;
            emit lineProcessed(i_Line++);
        }

        ts_Output << "*/" << endl;
        emit lineProcessed(i_Line++);
    }


    //   Write table header line
    for (QStringList::ConstIterator Key = Order.constBegin(); Key != Order.constEnd(); ++Key)
    {
        ts_Output << Abbreviations[*Key];
        if (Units[*Key].size() > 0) ts_Output << " [" << Units[*Key] << "]";
        if (Key != Order.constEnd()-1)
            ts_Output << "\t";
        else
            ts_Output << endl;
    }
    emit lineProcessed(i_Line++);


    //   Write field data (slow!!!)
    for (qint32 i = 0; i < size(); ++i)
    {
        QStringList sl_Line;

        for (QStringList::ConstIterator Key = Order.constBegin(); Key != Order.constEnd(); ++Key)
        {
            if (*Key == "Date/Time")
                sl_Line << DateTime.at(i).toString(Qt::ISODate);
            else
                sl_Line << QString::number(Values[*Key][i]);
        }

        ts_Output << sl_Line.replaceInStrings(QRegExp("^nan$"), "").join("\t") << endl;
        emit lineProcessed(i + 1);
    }

    //   Finish file output
    File.close();
    return _NOERROR_;
}


// Member function: set
void BsrnData::set(const qreal Value)
{
    for (QMap<QString, QVector<qreal> >::Iterator v = Values.begin(); v != Values.end(); ++v)
        v.value().fill(Value);
}


// Member function: set
void BsrnData::set(const qreal Value, BsrnData & QcCodes)
{
    for (QMap<QString, QVector<qreal> >::Iterator v = Values.begin(); v != Values.end(); ++v)
        for (qint32 i = 0; i < v.value().size(); ++i)
            if (QcCodes.Values.contains(v.key()))
                if (QcCodes.Values[v.key()][i] != 0)
                    v.value()[i] = Value;
}


// Member function: size()
qint32 BsrnData::size()
{
    qint32 Size = 0x7fffffff;
    qint32 SizeOld = Size;

    for (QMap<QString, QString>::ConstIterator p = Parameters.constBegin(); p != Parameters.constEnd(); ++p)
    {
        if (p.key() == "Date/Time" && DateTime.size() != 0)
            Size = std::min(Size,DateTime.size());
        else
            Size = std::min(Size,Values[p.key()].size());
        if (Size != SizeOld && SizeOld < 0xffff)
            qDebug() << "BsrnData::size()    Different sizes detected." << Size << SizeOld;
        SizeOld = Size;
    }

    return Size;
}


// Member function: toSiUnits
void BsrnData::toSiUnits()
{
    if (Values.keys().contains("T2"))
        if (Units["T2"].contains("C"))
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["T2"][i] += 273.15;
            Units["T2"] = "K";
        }

    if (Values.keys().contains("RH"))
        if (Units["RH"] == "%")
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["RH"][i] /= 100;
            Units["RH"] = "";
        }

    if (Values.keys().contains("PoPoPoPo"))
        if (Units["PoPoPoPo"] == "hPa")
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["PoPoPoPo"][i] *= 100;
            Units["PoPoPoPo"] = "Pa";
        }
}


// Member function: toBsrnUnits
void BsrnData::toBsrnUnits()
{
    if (Values.contains("T2"))
        if (Units["T2"] == "K")
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["T2"][i] -= 273.15;
            Units["T2"] = "°C";
        }

    if (Values.contains("RH"))
        if (Units["RH"] == "")
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["RH"][i] *= 100;
            Units["RH"] = "%";
        }

    if (Values.contains("PoPoPoPo"))
        if (Units["PoPoPoPo"] == "Pa")
        {
            for (qint32 i = 0; i < size(); ++i)
                Values["PoPoPoPo"][i] /= 100;
            Units["PoPoPoPo"] = "hPa";
        }
}


// Member function: appendToKeys
void BsrnData::appendToKeys(const QString &StringToAdd, const QStringList & KeyList)
{
    for (QStringList::ConstIterator Key = KeyList.constBegin(); Key != KeyList.constEnd(); ++Key)
        if (Parameters.contains(*Key))
            renameKey(*Key, *Key + StringToAdd);
}


// Member function: renameKey
void BsrnData::renameKey(const QString &OldKey, const QString &NewKey)
{
    if (OldKey != "Date/Time")
        Values[NewKey] = Values.take(OldKey);
    Abbreviations[NewKey] = Abbreviations.take(OldKey);
    Parameters[NewKey] = Parameters.take(OldKey);
    Units[NewKey] = Units.take(OldKey);
    Order.replaceInStrings(QRegExp("^" + OldKey + "$"), NewKey);
}


// Member function: appendToParameters
void BsrnData::appendToParameters(const QString &StringToAdd, const QStringList & KeyList)
{
    for (QStringList::ConstIterator Key = KeyList.constBegin(); Key != KeyList.constEnd(); ++Key)
        if (Parameters.contains(*Key))
            Parameters[*Key] = Parameters[*Key] + StringToAdd;
}


// Member function: appendToAbbreviations
void BsrnData::appendToAbbreviations(const QString &StringToAdd, const QStringList & KeyList)
{
    for (QStringList::ConstIterator Key = KeyList.constBegin(); Key != KeyList.constEnd(); ++Key)
        if (Abbreviations.contains(*Key))
            Abbreviations[*Key] = Abbreviations[*Key] + StringToAdd;
}

/*
// Holger Schmithüsen
// Member function: removeOther
void BsrnData::removeOther(const QStringList & KeepList)
{
    for (QMap<QString, QString>::Iterator p = Parameters.begin(); p != Parameters.end(); ++p)
    {
        if (KeepList.contains(p.key()) == false)
        {
            if (p.key() == "Date/Time")
                DateTime.clear();
            else
                Values.remove(p.key());

            Abbreviations.remove(p.key());
            Units.remove(p.key());
            Order.removeAll(p.key());
            Parameters.remove(p.key());
        }
    }
}
*/

// Rainer Sieger, 2013-07-30
// Member function: removeOther
void BsrnData::removeOther(const QStringList & KeepList)
{
    QMapIterator<QString, QString> p(Parameters);

    while(p.hasNext())
    {
        p.next();

        if (KeepList.contains(p.key()) == false)
        {
            if (p.key() == "Date/Time")
                DateTime.clear();
            else
                Values.remove(p.key());

            Abbreviations.remove(p.key());
            Units.remove(p.key());
            Order.removeAll(p.key());
            Parameters.remove(p.key());
        }
    }
}

// Member function: setUnits
void BsrnData::setUnits(const QString &Unit, const QStringList & KeyList)
{
    for (QStringList::ConstIterator Key = KeyList.constBegin(); Key != KeyList.constEnd(); ++Key)
        if (Parameters.contains(*Key))
            Units[*Key] = Unit;
}


// Member function: appendHorizontally
void BsrnData::appendHorizontally(const BsrnData & DataToAdd)
{
    for (QMap<QString, QString>::ConstIterator p = DataToAdd.Parameters.constBegin(); p != DataToAdd.Parameters.constEnd(); ++p)
    {
        addParameter(p.key(),DataToAdd);
        if ((p.key() == "Date/Time" && DateTime != DataToAdd.DateTime) || (p.key() != "Date/Time" && Values[p.key()] != DataToAdd.Values[p.key()]))
            qDebug() << "Warning: Inconsitency during execution of BsrnData::appendHorizontally with key" << p.key();
    }

    for (QMap<QString, QStringList>::ConstIterator d = DataToAdd.DataDescription.constBegin(); d != DataToAdd.DataDescription.constEnd(); ++d)
        if (DataDescription.contains(d.key()) == false || d.value() != DataDescription[d.key()])
            DataDescription.insertMulti(d.key(), d.value());
    Order.append(DataToAdd.Order);
    Order.removeDuplicates();
}


// Member function: getAllBsrnParameters
//   TODO: Extract parameters from www.pangaea.de/ddi?request=bsrn/BSRNParameterInUse&format=text&title=BSRN+Parameter+in+use
QMap<QString, QString> BsrnData::getAllBsrnParameters()
{
    QMap<QString, QString> AllParameters;

    AllParameters["Date/Time"] = "DATE/TIME";
    AllParameters["Height"] = "Height above ground";
    AllParameters["SWD"] = "Short-wave downward (GLOBAL) radiation";
    AllParameters["DIR"] = "Direct radiation";
    AllParameters["DIF"] = "Diffuse radiation";
    AllParameters["LWD"] = "Long-wave downward radiation";
    AllParameters["SWU"] = "Short-wave upward (REFLEX) radiation";
    AllParameters["LWU"] = "Long-wave upward radiation";
    AllParameters["T2"] = "Air temperature";
    AllParameters["RH"] = "Humidity, relative";
    AllParameters["PoPoPoPo"] = "Station pressure";

    return AllParameters;
}


// Member function: addParameter
void BsrnData::addParameter(const QString &Key, const BsrnData DataIn)
{
    if (Parameters.contains(Key) == false && DataIn.Parameters.contains(Key) == true)
    {
        if (Key == "Date/Time")
            DateTime = DataIn.DateTime;
        else
            Values[Key] = DataIn.Values[Key];
        Abbreviations[Key] = DataIn.Abbreviations[Key];
        Units[Key] = DataIn.Units[Key];
        Parameters[Key] = DataIn.Parameters[Key];
    }
}


// Member function: addParameter
void BsrnData::addParameter(const QString &Key, const QString &ParameterName, const QString &Unit, const QString &Abbreviation)
{
    if (Parameters.contains(Key) == false)
    {
        if (Key == "Date/Time")
            DateTime.fill(QDateTime(), size());
        else
            Values[Key].fill(NAN, size());
        Abbreviations[Key] = Abbreviation;
        Units[Key] = Unit;
        Parameters[Key] = ParameterName;
    }
}


// Member function: setDefaultOrder
void BsrnData::setDefaultOrder()
{
    Order = Parameters.keys();
    if (Order.contains("Date/Time"))
      Order.move(Order.indexOf("Date/Time"),0);
}


// Member function: setSortedOrder
void BsrnData::setSortedOrder()
{
    Order.sort();
    if (Order.contains("Date/Time"))
      Order.move(Order.indexOf("Date/Time"),0);
}


// Helper function: NumOfSections
int NumOfSections( const QString &String )
{
    return( String.count( "\t" ) + 1 );
}
