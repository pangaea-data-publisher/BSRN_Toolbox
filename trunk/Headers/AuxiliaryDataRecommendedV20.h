#ifndef AUXILIARYDATARECOMMENDETV20_H
#define AUXILIARYDATARECOMMENDETV20_H

#include <QtCore>
#include "AstroData.h"
#include "BsrnData.h"


class AuxiliaryDataRecommendedV20: public AstroData
{
public:
    AuxiliaryDataRecommendedV20(BsrnData & DataIn);
    void calculate(const QString &AlgorithmIn);
    void setDefaultOrder();

    qreal & S0;
    qreal   Sigma;


private:
    void addParameter(const QString & Key);
};

#endif // AUXILIARYDATARECOMMENDETV20_H
