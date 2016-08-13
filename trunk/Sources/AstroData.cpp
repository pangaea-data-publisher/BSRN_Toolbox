#include <QtCore>
#include "AstroData.h"
#include "solpos00.h"


// Constructor
AstroData::AstroData(BsrnData & DataIn)
    : BsrnData(DataIn)
{
    SolarConstant = NAN;

    Order.clear();
    if (Parameters.contains("Date/Time"))
        Order << "Date/Time";
    removeOther(QStringList(Order << "T2" << "PoPoPoPo"));
}


// Member function: calculate
//  This is excluded from the constructor to enable display of progress during calculation
void AstroData::calculate (const QString &AlgorithmIn)
{
    //   Preliminaries
    Algorithm = AlgorithmIn;
    DataDescription["Algorithm"] = QStringList(AlgorithmIn);
    toSiUnits();


    //   Iqbal1983
    if (Algorithm == "Iqbal1983")
    {
        //     Initialisation
        addParameter("SolarZenithAngle");
        addParameter("EarthSunDistance");
        addParameter("DirectExtraTerrestrial");
        addParameter("SwdExtraTerrestrial");
        setDefaultOrder();

        //     Perform calculation
        for (qint32 i = 0; i<size(); i++)
        {
            //       Day number of the year (Chapter 1.2, p. 3)
            qreal dn = DateTime.at(i).date().dayOfYear();
            //       Day angle in radiants (1.2.2)
            qreal Gamma = 2 * M_PI * (dn - 1) / 365;
            //       Eccentricity correction factor of the earth's orbit (Eq. 1.2.1)
            qreal E0 = 1.00011 + 0.034221 * qCos(Gamma) \
                               + 0.001280 * qSin(Gamma) \
                               + 0.000719 * qCos(2*Gamma) \
                               + 0.000077 * qSin(2*Gamma);
            //       Mean sun-earth distance in m (Chapter 1.2, p. 2)
            qreal r0 = 1.496e11;
            //       Sun-earth distance in m (Eq. 1.2.1)
            qreal r = r0 / qSqrt(E0);
            //       Solar declination in degrees (Eq. 1.3.1)
            qreal Delta = (0.006918 - 0.399912 * qCos(Gamma) \
                                    + 0.070257 * qSin(Gamma) \
                                    - 0.006758 * qCos(2*Gamma) \
                                    + 0.000907 * qSin(2*Gamma) \
                                    - 0.002697 * qCos(3*Gamma) \
                                    + 0.001480 * qSin(3*Gamma)) * 180 / M_PI;
            //       Equation of Time in minutes (Eq. 1.4.1)
            qreal Et = (0.000075 + 0.001868 * qCos(Gamma) \
                                 - 0.032077 * qSin(Gamma) \
                                 - 0.014615 * qCos(2*Gamma) \
                                 - 0.04089  * qSin(2*Gamma)) * 229.18;
            //       Local apparent time / true solar time in minutes (Chapter 1.4, pp. 11, 13)
            qreal UTC = - DateTime[i].time().secsTo(QTime::fromString("00:00:00")) / 60;
            qreal LAT = UTC + 4 * Longitude + Et;
            //       Hour angle in degrees (Chapter 1.5, pp. 15, 28)
            qreal Omega = (12 - LAT/60) * 15;
            //       Geographic latitude in degrees (Chapter 1.5, p. 15)
            qreal Phi = Latitude;
            //       Zenith angle in degrees (Eq. 1.5.1)
            qreal ThetaZ = qAcos(qSin(Delta/180*M_PI) * qSin(Phi/180*M_PI) + \
                                 qCos(Delta/180*M_PI) * qCos(Phi/180*M_PI) * qCos(Omega/180*M_PI)) * 180 / M_PI;
            //       Solar constant in W/m^2 (Chapter 3.4, p. 53)
            qreal ISC = 1367;
            //       Extraterrestrial Irradiance in W/m^2 (Eq. 4.2.1)
            qreal I0n = ISC * E0;
            //       Irradiance on a horizontal surface in W/m^2 (Eq. 4.2.2)
            qreal I0 = I0n * qMax(qCos(ThetaZ/180*M_PI), 0.0);

            //     Set AstroData variables
            Values["SolarZenithAngle"][i] = ThetaZ;
            Values["EarthSunDistance"][i] = r;
            Values["DirectExtraTerrestrial"][i] = I0n;
            Values["SwdExtraTerrestrial"][i] = I0;
            SolarConstant = ISC;

            emit lineProcessed(i + 1);
        }
        DataDescription["Solar constant"] = QStringList(QString::number(SolarConstant) + " W/m**2");
    }


    //   Solpos
    if (Algorithm.startsWith("Solpos"))
    {
        //     Initialisation
        addParameter("SolarZenithAngle");
        addParameter("EarthSunDistance");
        addParameter("DirectExtraTerrestrial");
        addParameter("SwdExtraTerrestrial");
        setDefaultOrder();

        //     Perform calculation
        posdata * PosData = new posdata;
        S_init(PosData);
        for (qint32 i = 0; i<size(); ++i)
        {
            //       Define input for solpos
            PosData->year          = DateTime[i].date().year();
            PosData->daynum        = DateTime[i].date().dayOfYear();
            PosData->hour          = DateTime[i].time().hour();
            PosData->minute        = DateTime[i].time().minute();
            PosData->second        = DateTime[i].time().second();
            PosData->longitude     = Longitude;
            PosData->latitude      = Latitude;
            PosData->timezone      = 0;
            if (Algorithm == "Solpos with Refraction")
            {
                if (std::isnan(Values["PoPoPoPo"][i]) == false)
                    PosData->press = Values["PoPoPoPo"][i]/100;
                else
                    PosData->press = -999.9;
                if (std::isnan(Values["T2"][i]) == false)
                    PosData->temp  = Values["T2"][i]-273.15;
                else
                    PosData->temp  = -999.9;
                PosData->function  = (S_GEOM | S_REFRAC | S_ETR);
            }
            else if (Algorithm == "Solpos without Refraction")
                PosData->function  = (S_GEOM | S_ZENETR | S_ETR);
            //       Calculate
            long ErrorCode = S_solpos(PosData);

            //       Set AstroData variables
            if (ErrorCode == 0)
            {
                if (Algorithm == "Solpos with Refraction")
                    Values["SolarZenithAngle"][i]       = PosData->zenref;
                else if (Algorithm == "Solpos without Refraction")
                    Values["SolarZenithAngle"][i]   = PosData->zenetr;
                Values["DirectExtraTerrestrial"][i] = PosData->etrn;
                Values["SwdExtraTerrestrial"][i]    = PosData->etr;
                Values["EarthSunDistance"][i]       = ASTRONOMICALUNIT/sqrt(PosData->erv);
            }
            else
            {
                Values["SolarZenithAngle"][i]       = NAN;
                Values["EarthSunDistance"][i]       = NAN;
                Values["DirectExtraTerrestrial"][i] = NAN;
                Values["SwdExtraTerrestrial"][i]    = NAN;
            }

            emit lineProcessed(i + 1);
        }
        SolarConstant             = PosData->solcon;
        DataDescription["Solar constant"] = QStringList(QString::number(SolarConstant) + " W/m**2");
    }
}


// Member function: addParameter
void AstroData::addParameter(const QString &Key)
{
    if      (Key == "SolarZenithAngle"      ) BsrnData::addParameter(Key, "Solar zenith angle", "deg", "SZA");
    else if (Key == "EarthSunDistance"      ) BsrnData::addParameter(Key, "Distance Earth-Sun", "m", "EarthSunDistance");
    else if (Key == "DirectExtraTerrestrial") BsrnData::addParameter(Key, "Direct radiation at top of atmosphere", "W/m**2", "DIRTOA");
    else if (Key == "SwdExtraTerrestrial"   ) BsrnData::addParameter(Key, "Shortwave downward radiation at top of atmosphere", "W/m**2", "SWDTOA");
}
