# BSRN Toolbox - a tool to create quality checked output files from BSRN datasets and station-to-archive files
# Holger Schmithuesen, Rainer Sieger, Gert Koenig-Langlo
# Alfred Wegener Institute, Bremerhaven, Germany
# last change: 2014-10-04

macx {
    # creating cache file
    cache()

    # Set SDK
    QMAKE_MAC_SDK = macosx10.14 # Xcode 9 - 2017-09-20

    # Only Intel binaries are accepted so force this
    CONFIG += x86_64

    # Minimum OS X version for submission is 10.7
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

    # Icon has been downloaded from http://www.large-icons.com/stock-icons/free-large-torrent-icons.htm
    ICON = ./Resources/icon/Application.icns

    # Replace default Info.plist
    QMAKE_INFO_PLIST = ./Resources/Info.plist


    CONFIG += release

    CONFIG += app_bundle

    SUBDIRS = Frameworks Plugins Resources
    app.depends = ./Plugins
    app.depends = ./Frameworks
    app.depends = ./Resources

    STRIP_BITCODE_FROM_COPIED_FILES=NO

    QMAKE_CFLAGS += -gdwarf-2
    QMAKE_CXXFLAGS += -gdwarf-2
    QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}

win32 {
    # Icon
    RC_FILE = ./Resources/Application.rc
}

linux-g++ {
}

TARGET = BSRN_Toolbox
TEMPLATE = app

QT += widgets network

INCLUDEPATH += ./Headers

HEADERS = ./Headers/Application.h \
    ./Headers/AstroData.h \
    ./Headers/AuxiliaryDataRecommendedV20.h \
    ./Headers/BsrnData.h \
    ./Headers/Globals.h \
    ./Headers/solpos00.h \
    ./Headers/simplecrypt.h \
    ./Forms/AboutDialog/AboutDialog.h \
    ./Forms/ConcatenateOptionsDialog/ConcatenateOptionsDialog.h \
    ./Forms/DownloadManagerDialog/DownloadManagerDialog.h \
    ./Forms/FormatFormattedOptionsDialog/FormatFormattedOptionsDialog.h \
    ./Forms/FormatUnformattedOptionsDialog/FormatUnformattedOptionsDialog.h \
    ./Forms/QualityCheckOptionsDialogs/QualityCheckRecommendedV20OptionsDialog.h \
    ./Forms/SelectParametersDialog/SelectParametersDialog.h

SOURCES = ./Sources/ApplicationInit.cpp \
    ./Sources/ApplicationMainWindow.cpp \
    ./Sources/ApplicationCreateMenu.cpp \
    ./Sources/ApplicationErrors.cpp \
    ./Sources/ApplicationPreferences.cpp \
    ./Sources/ApplicationTools.cpp \
    ./Sources/AssignmentConverter.cpp \
    ./Sources/AstroData.cpp \
    ./Sources/AuxiliaryDataRecommendedV20.cpp \
    ./Sources/BasicMeasurementsConverter.cpp \
    ./Sources/BsrnData.cpp \
    ./Sources/ConvertEOL.cpp \
    ./Sources/CheckStationToArchiveFiles.cpp \
    ./Sources/ConcatenateFiles.cpp \
    ./Sources/CompressFiles.cpp \
    ./Sources/ConvertFiles.cpp \
    ./Sources/CreateReferenceImportFile.cpp \
    ./Sources/CreateReplaceDatabase.cpp \
    ./Sources/DownloadStationToArchiveFiles.cpp \
    ./Sources/doAll.cpp \
    ./Sources/ExpandedMeasurementsConverter.cpp \
    ./Sources/FileIDConverter.cpp \
    ./Sources/MessagesConverter.cpp \
    ./Sources/OtherMeasurementsAtXmConverter.cpp \
    ./Sources/OtherMinuteMeasurementsConverter.cpp \
    ./Sources/OzoneEquipmentConverter.cpp \
    ./Sources/OzoneMeasurementsConverter.cpp \
    ./Sources/QualityCheckRecommendedV20.cpp \
    ./Sources/RadiationInstrumentsConverter.cpp \
    ./Sources/RadiosondeEquipmentConverter.cpp \
    ./Sources/RadiosondeMeasurementsConverter.cpp \
    ./Sources/ScientistIDConverter.cpp \
    ./Sources/solpos.cpp \
    ./Sources/StationDescriptionConverter.cpp \
    ./Sources/StationHistoryConverter.cpp \
    ./Sources/SYNOPConverter.cpp \
    ./Sources/UltraVioletMeasurementsConverter.cpp \
    ./Sources/simplecrypt.cpp \
    ./Sources/decompressFiles.cpp \
    ./Forms/AboutDialog/AboutDialog.cpp \
    ./Forms/ConcatenateOptionsDialog/ConcatenateOptionsDialog.cpp \
    ./Forms/DownloadManagerDialog/DownloadManagerDialog.cpp \
    ./Forms/FormatFormattedOptionsDialog/FormatFormattedOptionsDialog.cpp \
    ./Forms/FormatUnformattedOptionsDialog/FormatUnformattedOptionsDialog.cpp \
    ./Forms/QualityCheckOptionsDialogs/QualityCheckRecommendedV20OptionsDialog.cpp \
    ./Forms/SelectParametersDialog/SelectParametersDialog.cpp

FORMS = ./Forms/AboutDialog/aboutdialog.ui \
    ./Forms/ConcatenateOptionsDialog/concatenateoptionsdialog.ui \
    ./Forms/DownloadManagerDialog/downloadmanagerdialog.ui \
    ./Forms/FormatFormattedOptionsDialog/formatformattedoptionsdialog.ui \
    ./Forms/FormatUnformattedOptionsDialog/formatunformattedoptionsdialog.ui \
    ./Forms/SelectParametersDialog/selectparametersdialog.ui
