#ifndef ASTRODATA_H
#define ASTRODATA_H

#include <QtCore>
#include "BsrnData.h"

#define ASTRONOMICALUNIT 149597870691.0


class AstroData : public BsrnData
{
    Q_OBJECT


public:
    AstroData (BsrnData & DataIn);
    void calculate (const QString & AlgorithmIn);

    QString Algorithm;
    qreal   SolarConstant;


private:
    void addParameter(const QString & Key);
};


#endif // ASTRODATA_H
