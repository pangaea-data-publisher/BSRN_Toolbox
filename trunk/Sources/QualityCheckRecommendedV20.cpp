// Author:        Holger Schmithüsen (holger.schmithuesen@awi.de)
// Created:       2011-09-07
// Last modified: 2012-01-XX

#include "Application.h"
#include "BsrnData.h"
#include "AuxiliaryDataRecommendedV20.h"

#define LT_PHYSICAL_POSSIBLE 0
#define GT_PHYSICAL_POSSIBLE 1
#define LT_EXTREMELY_RARE    2
#define GT_EXTREMELY_RARE    3
#define LT_COMPARISON        4
#define GT_COMPARISON        5


int MainWindow::QualityCheckRecommendedV20(const QString & FileNameIn, QString & FileNameOut, const bool & b_CheckPhysicallyPossibleLimits, const bool & b_CheckExtremelyRareLimits, const bool & b_CheckComparisons, const QString & s_AuxiliaryDataAlgorithm, const bool & b_OutputCodes, const bool & b_OutputCleanedValues, const bool & b_OutputOriginalValues, const bool & b_OutputAuxiliaryData, const bool & b_OutputOneFile)
{
    qDebug() << "QualityCheckRecommendedV20 started";
    QTime t;
    t.start();


    // Read data from file
    initProgress(2, FileNameIn, tr("Reading input"), 100);
    QStringList sl_Input;
    int n = readFile(FileNameIn, sl_Input, 0); //assume UTF-8. TODO: Fix!
    if (n<0) return n;
    qDebug() << "bsrndata read from file" << t.elapsed();

    BsrnData Measurements;
    connect(&Measurements, SIGNAL(progressMade(int)), this, SLOT(incProgress(int)));
    Measurements.readFromStringList(sl_Input);
    disconnect(&Measurements, SIGNAL(progressMade(int)), this, SLOT(incProgress(int)));
    resetProgress(2);
    Measurements.DataDescription.remove("Citation");
    Measurements.DataDescription.remove("Other version");
    qDebug() << "bsrndata read from stringlist" << t.elapsed() << Measurements.size();


    // Check which parameter are available
    //   Essential parameter
    if (Measurements.size() <= 0) return NODATAFOUND;
    if (Measurements.Parameters.contains("Date/Time") == false) return DATETIMENOTFOUND;
    if (std::isnan(Measurements.Longitude)) return LONGITUDENOTFOUND;
    if (std::isnan(Measurements.Latitude) ) return LATITUDENOTFOUND;

    //   Parameter needed for "Solpos with Refraction"
    if (s_AuxiliaryDataAlgorithm == "Solpos with Refraction")
    {
        if (Measurements.Values.contains("PoPoPoPo") == false) return POPOPOPONOTFOUND;
        if (Measurements.Values.contains("T2"      ) == false) return T2NOTFOUND;
    }

    //   Optional parameter
    if (b_CheckPhysicallyPossibleLimits || b_CheckExtremelyRareLimits || b_CheckComparisons)
    {
        QString MissingList;
        if (Measurements.Values.contains("SWD") == false                      ) MissingList.append("<li>SWD</li>");
        if (Measurements.Values.contains("DIF") == false                      ) MissingList.append("<li>DIF</li>");
        if (Measurements.Values.contains("DIR") == false                      ) MissingList.append("<li>DIR</li>");
        if (Measurements.Values.contains("SWU") == false                      ) MissingList.append("<li>SWU</li>");
        if (Measurements.Values.contains("LWD") == false                      ) MissingList.append("<li>LWD</li>");
        if (Measurements.Values.contains("LWU") == false                      ) MissingList.append("<li>LWU</li>");
        if (Measurements.Values.contains("T2" ) == false && b_CheckComparisons) MissingList.append("<li>T2</li>");

        if (MissingList.size() > 0)
        {
            QString Message;
            //Message += "<b>" + FileNameIn + "</b>";
            Message += tr("<p>Not all selected quality checks will be performed due to missing parameter.</p>");
            Message += tr("<p>The following parameter are missing:</p>");
            Message += tr("<ul>");
            Message += MissingList;
            Message += tr("</ul>");
            ErrorMessage->showMessage(Message);
            QApplication::processEvents();
        }
    }
    qDebug() << "bsrndata availability checks performed" << t.elapsed();


    // Calculate auxiliary data
    initProgress(2, FileNameIn, tr("Calculating auxiliary data"), Measurements.size());

    AuxiliaryDataRecommendedV20 AuxData (Measurements);
    connect(&AuxData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
    AuxData.calculate(s_AuxiliaryDataAlgorithm);
    disconnect(&AuxData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
    qDebug() << "auxdata calculated" << t.elapsed();

    // Quality checks
    initProgress(2, FileNameIn, tr("Performing quality checks"), int(b_CheckPhysicallyPossibleLimits + b_CheckExtremelyRareLimits + b_CheckComparisons) * Measurements.size());
    Measurements.toSiUnits();

    //   Initialise QcCodes
    BsrnData QcCodes(Measurements);
    QStringList List;
    List << "Date/Time" << "SWD" << "DIR" << "DIF" << "LWD" << "SWU" << "LWU" << "T2";
    QcCodes.removeOther(List);
    List.clear();
    List << "SWD" << "DIR" << "DIF" << "LWD" << "SWU" << "LWU" << "T2";
    QcCodes.appendToAbbreviations("Qc", List);
    QcCodes.appendToParameters(" Quality code", List);
    QcCodes.setUnits("", List);
    QcCodes.set(0);

    //   "Physically Possible Limits"
    if (b_CheckPhysicallyPossibleLimits == true)
    {
        for (int i = 0; i < Measurements.size(); i++)
        {
            //     "Global SWdn"
            if (Measurements.Values.contains("SWD"))
            {
                if (Measurements.Values["SWD"][i] < -4                                                                      ) QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["SWD"][i] > AuxData.Values["Sa"][i] * 1.5 * qPow(AuxData.Values["Mu0"][i],1.2) + 100) QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }
            //     "Diffuse SW"
            if (Measurements.Values.contains("DIF"))
            {
                if (Measurements.Values["DIF"][i] < -4                                                                      ) QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["DIF"][i] > AuxData.Values["Sa"][i] * 0.95 * qPow(AuxData.Values["Mu0"][i],1.2) + 50) QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }
            //     "Direct Normal SW"
            if (Measurements.Values.contains("DIR"))
            {
                if (Measurements.Values["DIR"][i] < -4                     ) QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["DIR"][i] > AuxData.Values["Sa"][i]) QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }
            //     "SWup"
            if (Measurements.Values.contains("SWU"))
            {
                if (Measurements.Values["SWU"][i] < -4                                                                     ) QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["SWU"][i] > AuxData.Values["Sa"][i] * 1.2 * qPow(AuxData.Values["Mu0"][i],1.2) + 50) QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }
            //     "LWdn"
            if (Measurements.Values.contains("LWD"))
            {
                if (Measurements.Values["LWD"][i] <  40) QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["LWD"][i] > 700) QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }
            //     "LWup"
            if (Measurements.Values.contains("LWU"))
            {
                if (Measurements.Values["LWU"][i] <  40) QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << LT_PHYSICAL_POSSIBLE));
                if (Measurements.Values["LWU"][i] > 900) QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << GT_PHYSICAL_POSSIBLE));
            }

            incProgress(i);
        }
        QcCodes.DataDescription["Applied quality checks"] << "BSRN Recommended V2.0 - Physical Possible Limits";
        qDebug() << "quality checks physical possible performed" << t.elapsed();
    }

    //   "Extremely Rare Limits"
    if (b_CheckExtremelyRareLimits == true)
    {
        for (int i = 0; i < Measurements.size(); i++)
        {
            //     "Global SWdn"
            if (Measurements.Values.contains("SWD"))
            {
                if (Measurements.Values["SWD"][i] < -2                                                                     ) QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["SWD"][i] > AuxData.Values["Sa"][i] * 1.2 * qPow(AuxData.Values["Mu0"][i],1.2) + 50) QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << GT_EXTREMELY_RARE));
            }
            //     "Diffuse SW"
            if (Measurements.Values.contains("DIF"))
            {
                if (Measurements.Values["DIF"][i] < -2                                                                      ) QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["DIF"][i] > AuxData.Values["Sa"][i] * 0.75 * qPow(AuxData.Values["Mu0"][i],1.2) + 30) QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_EXTREMELY_RARE));
            }
            //     "Direct Normal SW"
            if (Measurements.Values.contains("DIR"))
            {
                if (Measurements.Values["DIR"][i] < -2                                                                      ) QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["DIR"][i] > AuxData.Values["Sa"][i] * 0.95 * qPow(AuxData.Values["Mu0"][i],0.2) + 10) QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << GT_EXTREMELY_RARE));
            }
            //     "SWup"
            if (Measurements.Values.contains("SWU"))
            {
                if (Measurements.Values["SWU"][i] < -2                                                                     ) QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["SWU"][i] > AuxData.Values["Sa"][i] * 1.2 * qPow(AuxData.Values["Mu0"][i],1.2) + 50) QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << GT_EXTREMELY_RARE));
            }
            //     "LWdn"
            if (Measurements.Values.contains("LWD"))
            {
                if (Measurements.Values["LWD"][i] <  60) QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["LWD"][i] > 500) QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << GT_EXTREMELY_RARE));
            }
            //     "LWup"
            if (Measurements.Values.contains("LWU"))
            {
                if (Measurements.Values["LWU"][i] <  60) QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << LT_EXTREMELY_RARE));
                if (Measurements.Values["LWU"][i] > 700) QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << GT_EXTREMELY_RARE));
            }

            incProgress(int(b_CheckPhysicallyPossibleLimits) * Measurements.size() + i);
        }
        QcCodes.DataDescription["Applied quality checks"] << "BSRN Recommended V2.0 - Extremely Rare Limits";
        qDebug() <<"quality checks extremely rare performed" << t.elapsed();
    }

    //   "Comparisons"
    if (b_CheckComparisons == true)
    {
        for (int i = 0; i < Measurements.size(); i++)
        {
            //     "Ratio of Global over Sum SW"
            if (Measurements.Values.contains("SWD"))
            {
                if (Measurements.Values["SWD"][i]/AuxData.Values["SumSW"][i] < 0.92 &&                                  AuxData.Values["SZA"][i] < 75 && AuxData.Values["SumSW"][i] > 50)
                {
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_COMPARISON));
                }
                if (Measurements.Values["SWD"][i]/AuxData.Values["SumSW"][i] > 1.08 &&                                  AuxData.Values["SZA"][i] < 75 && AuxData.Values["SumSW"][i] > 50)
                {
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << LT_COMPARISON));
                }
                if (Measurements.Values["SWD"][i]/AuxData.Values["SumSW"][i] < 0.75 && 75 < AuxData.Values["SZA"][i] && AuxData.Values["SZA"][i] < 93 && AuxData.Values["SumSW"][i] > 50)
                {
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_COMPARISON));
                }
                if (Measurements.Values["SWD"][i]/AuxData.Values["SumSW"][i] > 1.15 && 75 < AuxData.Values["SZA"][i] && AuxData.Values["SZA"][i] < 93 && AuxData.Values["SumSW"][i] > 50)
                {
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << LT_COMPARISON));
                }
            }
            //     "Diffuse Ratio"
            if (Measurements.Values.contains("DIF") && Measurements.Values.contains("SWD"))
            {
                if (Measurements.Values["DIF"][i]/Measurements.Values["SWD"][i] >= 1.05 &&                                  AuxData.Values["SZA"][i] < 75 && Measurements.Values["SWD"][i] > 50)
                {
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_COMPARISON));
                }
                if (Measurements.Values["DIF"][i]/Measurements.Values["SWD"][i] >= 1.10 && 75 < AuxData.Values["SZA"][i] && AuxData.Values["SZA"][i] < 93 && Measurements.Values["SWD"][i] > 50)
                {
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_COMPARISON));
                }
            }
            //     "Swup Comparison"
            if (Measurements.Values.contains("SWU") && Measurements.Values.contains("SWD"))
            {
                if (Measurements.Values["SWU"][i] > AuxData.Values["SumSW"][i] &&  AuxData.Values["SumSW"][i] > 50 && !std::isnan(AuxData.Values["SumSW"][i]) && QcCodes.Values["DIR"][i] == 0 && QcCodes.Values["DIF"][i] == 0)
                {
                    QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["DIF"][i] = (qreal) ((quint32) QcCodes.Values["DIF"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["DIR"][i] = (qreal) ((quint32) QcCodes.Values["DIR"][i] | (1 << LT_COMPARISON));
                }
                if (Measurements.Values["SWU"][i] > Measurements.Values["SWD"  ][i] && Measurements.Values["SWD"  ][i] > 50 &&  std::isnan(AuxData.Values["SumSW"][i]) && QcCodes.Values["SWD"][i] == 0                               )
                {
                    QcCodes.Values["SWU"][i] = (qreal) ((quint32) QcCodes.Values["SWU"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["SWD"][i] = (qreal) ((quint32) QcCodes.Values["SWD"][i] | (1 << LT_COMPARISON));
                }
            }
            //     "LWdn to Air Temperature comparison
            if (Measurements.Values.contains("LWD") && Measurements.Values.contains("T2"))
            {
                if (Measurements.Values["LWD"][i] <= 0.4 * AuxData.Sigma * qPow(Measurements.Values["T2"][i], 4) && QcCodes.Values["T2"][i] == 0)
                {
                    QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["T2" ][i] = (qreal) ((quint32) QcCodes.Values["T2" ][i] | (1 << GT_COMPARISON));
                }
                if (Measurements.Values["LWD"][i] >=       AuxData.Sigma * qPow(Measurements.Values["T2"][i], 4) + 25 && QcCodes.Values["T2"][i] == 0)
                {
                    QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["T2" ][i] = (qreal) ((quint32) QcCodes.Values["T2" ][i] | (1 << LT_COMPARISON));
                }
            }
            //     "LWup to Air Temperature comparison
            if (Measurements.Values.contains("LWU") && Measurements.Values.contains("T2"))
            {
                if (Measurements.Values["LWU"][i] <= AuxData.Sigma * qPow(Measurements.Values["T2"][i] - 15, 4) && QcCodes.Values["T2"][i] == 0)
                {
                    QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << LT_COMPARISON));
                    QcCodes.Values["T2" ][i] = (qreal) ((quint32) QcCodes.Values["T2" ][i] | (1 << GT_COMPARISON));
                }
                if (Measurements.Values["LWU"][i] >= AuxData.Sigma * qPow(Measurements.Values["T2"][i] + 25, 4) && QcCodes.Values["T2"][i] == 0)
                {
                    QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["T2" ][i] = (qreal) ((quint32) QcCodes.Values["T2" ][i] | (1 << LT_COMPARISON));
                }
            }
            //     "LWdn to LWup comparison
            if (Measurements.Values.contains("LWD") && Measurements.Values.contains("LWU"))
            {
                if (Measurements.Values["LWD"][i] >= Measurements.Values["LWU"][i] +  25)
                {
                    QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << LT_COMPARISON));
                }
                if (Measurements.Values["LWD"][i] <= Measurements.Values["LWU"][i] - 300)
                {
                    QcCodes.Values["LWD"][i] = (qreal) ((quint32) QcCodes.Values["LWD"][i] | (1 << GT_COMPARISON));
                    QcCodes.Values["LWU"][i] = (qreal) ((quint32) QcCodes.Values["LWU"][i] | (1 << LT_COMPARISON));
                }
            }

            incProgress(int(b_CheckPhysicallyPossibleLimits + b_CheckExtremelyRareLimits) * Measurements.size() + i);
        }
        QcCodes.DataDescription["Applied quality checks"] << "BSRN Recommended V2.0 - Comparisons";
        qDebug() << "quality checks comparisons performed" << t.elapsed();
    }
    qDebug() << "quality checks performed" << t.elapsed();


    // Write results
    //   Preliminaries
    Measurements.toBsrnUnits();
    QFileInfo FileInfoIn (FileNameIn);
//  QString FileNameOut; -> Rainer Sieger, 2016-06-09
    BsrnData MeasurementsCleaned (Measurements);
    MeasurementsCleaned.set(NAN,QcCodes);
    MeasurementsCleaned.DataDescription["Applied quality checks"] = QcCodes.DataDescription["Applied quality checks"];
    QcCodes.appendToKeys("Qf",List);
    BsrnData CombinedData;
    if (b_OutputOriginalValues) CombinedData.appendHorizontally(Measurements);
    if (b_OutputCleanedValues) CombinedData.appendHorizontally(MeasurementsCleaned);
    if (b_OutputCodes) CombinedData.appendHorizontally(QcCodes);
    if (b_OutputAuxiliaryData) CombinedData.appendHorizontally(AuxData);
    CombinedData.setSortedOrder();

    //   File with auxiliary data
    if (b_OutputAuxiliaryData == true && b_OutputOneFile == false)
    {
        initProgress(2, FileNameIn, tr( "Writing auxiliary data..." ), AuxData.size());
        FileNameOut = (FileInfoIn.absolutePath() + "/" + FileInfoIn.baseName() + "_QC_auxdata_temp.txt"); // .tab -> _temp.txt (Rainer Sieger - 2016-06-09 )
        connect(&AuxData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        int Error = AuxData.writeToFile(FileNameOut);
        disconnect(&AuxData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        if (Error != _NOERROR_) return Error;
        qDebug() << "aux data file written" << t.elapsed();
    }

    //   File with cleaned data (erroneous values removed)
    if (b_OutputCleanedValues == true && b_OutputOneFile == false)
    {
        initProgress(2, FileNameIn, tr("Writing cleaned data..."), MeasurementsCleaned.size());
        FileNameOut = (FileInfoIn.absolutePath() + "/" + FileInfoIn.baseName() + "_QC_cleaned_temp.txt"); // .tab -> _temp.txt (Rainer Sieger - 2016-06-09 )
        connect(&MeasurementsCleaned, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        int Error = MeasurementsCleaned.writeToFile(FileNameOut);
        disconnect(&MeasurementsCleaned, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        if (Error != _NOERROR_) return Error;
        qDebug() << "corrected file written" << t.elapsed();
    }

    //   File with error codes
    if (b_OutputCodes == true && b_OutputOneFile == false)
    {
        initProgress(2, FileNameIn, tr( "Writing QC codes..." ), QcCodes.size());
        FileNameOut = (FileInfoIn.absolutePath() + "/" + FileInfoIn.baseName() + "_QC_codes_temp.txt"); // .tab -> _temp.txt (Rainer Sieger - 2016-06-09 )
        connect(&QcCodes, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        int Error = QcCodes.writeToFile(FileNameOut);
        disconnect(&QcCodes, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        if (Error != _NOERROR_) return Error;
        qDebug() << "qc codes file written" << t.elapsed();
    }

    //   Combined file
    if (b_OutputOneFile == true)
    {
        initProgress(2, FileNameIn, tr( "Writing combined data..." ), CombinedData.size());
        FileNameOut = (FileInfoIn.absolutePath() + "/" + FileInfoIn.baseName() + "_QC_combined_temp.txt"); // .tab -> _temp.txt (Rainer Sieger - 2016-06-09 )
        connect(&CombinedData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        int Error = CombinedData.writeToFile(FileNameOut);
        disconnect(&CombinedData, SIGNAL(lineProcessed(int)), this, SLOT(incProgress(int)));
        if (Error != _NOERROR_) return Error;
        qDebug() << "combined file written" << t.elapsed();
    }

    resetProgress(2);
    return(_NOERROR_);
}

// **********************************************************************************************
// **********************************************************************************************
// **********************************************************************************************

/*! @brief Progressdialog hochzaehlen.
*
*   @param i_Step Anzahl der verarbeiteten Schritte.
*
*   @return Errorcode.
*/

int MainWindow::incProgress( const int i_Step )
{
    int status = _ERROR_;
//    if ( ProgressDialog )
//        status = incProgress(1, i_Step);
    if ( ProgressBar )
        status = incProgress(2, i_Step);
    return( status );
}

// *************************************************************************************
// *************************************************************************************
// *************************************************************************************

void MainWindow::doQualityCheckRecommendedV20()
{
    int		i				= 0;
    int		err				= 0;
    int		stopProgress	= 0;

    QString s_FilenameIn    = "";
    QString s_FilenameOut   = "";

// *************************************************************************************

    structParameter	*Parameter_0001_ptr	= NULL;
    structParameter	*Parameter_0009_ptr	= NULL;

    Parameter_0001_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];
    Parameter_0009_ptr	= new structParameter[MAX_NUM_OF_PARAMETER+1];

// **********************************************************************************************

    if ( gsl_FilenameList.isEmpty() == true )
        chooseFiles();

    if ( gsl_FilenameList.isEmpty() == false )
    {
        err = doQualityCheckRecommendedV20OptionsDialog( gb_CheckPhysicallyPossibleLimits, gb_CheckExtremelyRareLimits, gb_CheckComparisons, gs_AuxiliaryDataAlgorithm, gb_OutputCodes, gb_OutputCleanedValues, gb_OutputOriginalValues, gb_OutputAuxiliaryData, gb_OutputOneFile );

        if ( err == QDialog::Accepted )
        {
            initFileProgress( gsl_FilenameList.count(), gsl_FilenameList.at( 0 ), tr( "Quality check - Recommended V2.0 ..." ) );

            ErrorMessage = new QErrorMessage;
            err          = _NOERROR_;

            while ( ( i < gsl_FilenameList.count() ) && ( err == _NOERROR_ ) && ( stopProgress == 0 ) )
            {
                s_FilenameIn = gsl_FilenameList.at( i );

                if ( s_FilenameIn.endsWith( ".dat" ) == true )
                {
                    err = BasicMeasurementsConverter( false, false, LR0100, s_FilenameIn, s_FilenameOut, Parameter_0001_ptr, Parameter_0009_ptr, g_Method_ptr, g_Staff_ptr, g_Station_ptr, g_Reference_ptr, gb_OverwriteDataset, g_Dataset_ptr, gsl_FilenameList.count() );

                    s_FilenameIn = s_FilenameOut;
                }

                err = QualityCheckRecommendedV20( s_FilenameIn, s_FilenameOut, gb_CheckPhysicallyPossibleLimits, gb_CheckExtremelyRareLimits, gb_CheckComparisons, gs_AuxiliaryDataAlgorithm, gb_OutputCodes, gb_OutputCleanedValues, gb_OutputOriginalValues, gb_OutputAuxiliaryData, gb_OutputOneFile );

                if ( err == _NOERROR_ )
                {
                    s_FilenameIn = s_FilenameOut;
                    s_FilenameOut.replace( "_temp", "" );

                    err = convertFile( s_FilenameIn, s_FilenameOut, gsl_FilenameList.count() );
                }

                stopProgress = incFileProgress( gsl_FilenameList.count(), ++i );
            }

            resetFileProgress( gsl_FilenameList.count() );
        }
        else
        {
            err = _CHOOSEABORTED_;
        }
    }
    else
    {
        err = _CHOOSEABORTED_;
    }

// *************************************************************************************

    endTool( err, stopProgress, gi_ActionNumber, gs_FilenameFormat, gi_Extension, gsl_FilenameList, tr( "Done" ), tr( "Quality check - Recommended V2.0 was canceled" ), false, false );

// **********************************************************************************************

    if ( Parameter_0001_ptr != NULL )
    {
        delete []Parameter_0001_ptr;
        Parameter_0001_ptr = NULL;
    }

    if ( Parameter_0009_ptr != NULL )
    {
        delete []Parameter_0009_ptr;
        Parameter_0009_ptr = NULL;
    }

// **********************************************************************************************

    onError( err );
}
