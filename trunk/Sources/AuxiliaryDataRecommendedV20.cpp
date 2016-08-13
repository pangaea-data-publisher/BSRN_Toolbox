#include "Application.h"
#include "AuxiliaryDataRecommendedV20.h"

// Constructor
AuxiliaryDataRecommendedV20::AuxiliaryDataRecommendedV20(BsrnData & DataIn)
    : AstroData(DataIn),
      S0 (SolarConstant)
{
    Sigma = NAN;
    BsrnData::addParameter("DIF",DataIn);
    BsrnData::addParameter("DIR",DataIn);
    setDefaultOrder();
}


// Member function: calculate
//  This is excluded from the constructor enable display of progress during calculation
void AuxiliaryDataRecommendedV20::calculate(const QString &AlgorithmIn)
{
    AstroData::calculate(AlgorithmIn);
    renameKey("SolarZenithAngle", "SZA");

    //   Other scalar values
    if (std::isnan(S0))
    {
        S0 = 1368;
        DataDescription["Solar constant"] = QStringList(QString::number(SolarConstant) + " W/m**2");
        qDebug() << "Algorithm " << Algorithm << " does not specify S0.\nS0 = 1368 W/m^2 will be used.";
    }
    Sigma = 5.67e-8;
    DataDescription["Stefan-Boltzmann constant"] = QStringList(QString::number(Sigma) + " W*m**-2*K**-4");

    //   Other vector values
    addParameter("AU");
    addParameter("Mu0");
    addParameter("Sa");
    addParameter("SumSW");
    setDefaultOrder();

    for (qint32 i = 0; i<size(); i++)
    {
        if (!std::isnan(Values["SZA"][i]))
            if (Values["SZA"][i] <= 90)
                Values["Mu0"][i] = cos(Values["SZA"][i]*M_PI/180.0);
            else
                Values["Mu0"][i] = 0.0;
        else
            Values["Mu0"][i] = NAN;
        Values["AU"][i] = Values["EarthSunDistance"][i] / ASTRONOMICALUNIT;
        Values["Sa"][i] = S0 / pow(Values["AU"][i],2);
        if (Values.contains("DIF") && Values.contains("DIR"))
            Values["SumSW"][i] = Values["DIF"][i] + Values["DIR"][i] * Values["Mu0"][i];
    }
}


// Member function: addParameter
void AuxiliaryDataRecommendedV20::addParameter(const QString &Key)
{
    if      (Key == "AU"   ) BsrnData::addParameter(Key, "Earth-sun distance", "AU", "AU");
    else if (Key == "Mu0"  ) BsrnData::addParameter(Key, "Cosine of solar zenith angle", "", "Mu0");
    else if (Key == "Sa"   ) BsrnData::addParameter(Key, "Solar constant adjusted for earth-sun distance", "W/m**2", "Sa");
    else if (Key == "SumSW") BsrnData::addParameter(Key, "Diffuse SW + direct normal SW x Mu0", "W/m**2", "SumSW");
}


// Member function: setDefaultOrder
void AuxiliaryDataRecommendedV20::setDefaultOrder()
{
    Order.clear();
    Order << "Date/Time" << "SZA" << "Mu0" << "AU" << "Sa" << "SumSW" << "SwdExtraTerrestrial";
    for (int i = 0; i < Order.size(); ++i)
        if (Parameters.contains(Order.at(i)) == false)
            Order[i] = "";
    Order.removeAll("");
}

