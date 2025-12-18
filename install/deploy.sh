#!/bin/bash

set -e # Skript-Fehler -> Abbruch

# --- CLI Auswerten
# Test Parameter 
if [ $# -lt 2 ]; then echo -e "Usage: $(basename $0) QUELLE ZIEL\nFehler: Zu wenig Parameter.\nAbbruch ..."; exit; fi

# --- Haupt-Variablen
NAME=bsrn_toolbox
QUELLE=$1
ZIEL=$2

# --- Tests: Haupt-Variablen
# Quelle/Ziel/version.txt existiert nicht -> Ende
if ! test -e $QUELLE; then echo Quelle $QUELLE existiert nicht.; echo Abbruch...; exit ; fi
if ! test -e $ZIEL; then echo ZIEL $ZIEL existiert nicht.; echo Abbruch...; exit ; fi

# --- Arbeits-Variablen vorbereiten
# Version aus Git holen und in Versionsstring umwandeln
cd $QUELLE
VERSION=$(git describe --long);
VERSION=${VERSION%-*}; 
echo $VERSION > version_app.txt;
VER=$(echo $VERSION | tr -d . | tr - _) # kuerzen

# Unterverzeichnisse
TMP=tmp_$(date +"%y%m%d-%H%M%S")

# Temp + Timestamp -vorsichtshalber-> da Temp wieder geloescht wird
TMP=$TMP"_"$(date +"%y%m%d-%H%M%S")

# Temporäre Zielverzeichnisse
BUILDBASIS=$ZIEL/$TMP/basis
BUILDWIN=$ZIEL/$TMP/win
BUILDMAC=$ZIEL/$TMP/mac
BUILDLIN=$ZIEL/$TMP/linux

# Endverzeichnisse
FINAL=$ZIEL/final
BAK=$ZIEL/bak/$(date +"%y%m%d-%H%M%S")

# --- Info
echo "* Create BSRN Toolbox Apps"
echo "Name:        "$NAME
echo "Version:     "$VERSION
echo "Quelle:      "$QUELLE
echo "Ziel lokal:  "$ZIEL
read -p "Starten? [j/n]: " EIN
if [ "$EIN" != "j" ]; then echo "Abbruch..."; exit 1; fi

# Stelle korrekte Verzeichnissstruktur fuer Deploy sicher
# ------------------------------------------------------------------------------
echo "* Checke Deploy-Verzeichnisse"
# tmp
echo "- Erzeuge temporaeres Verzeichnis (info)";
mkdir -p $ZIEL/$TMP
echo "- Erzeuge temporaeres Verzeichnis (data)";
mkdir -p $ZIEL/$TMP

# old
if test -e $FINAL; then echo "- Verzeichnis zur Sicherung von alten Deploys (data) wird erzeugt"; mkdir -p $BAK; fi

# aktuell
# Backup eines evtl. bestehenden Deploy-Ordners
if test -e $FINAL; then echo "- Ein aktuelles Deploy (data) existiert bereits -> Verschieben in Sicherungsverzeichnis"; mv $FINAL $BAK; fi
echo "- Verzeichnis fuer aktuelles Deploy wird erzeugt";
mkdir -p $FINAL;

# Erstelle Basis in Aufbauverzeichnis
# ------------------------------------------------------------------------------
cd $QUELLE
echo "* Hole Basis"
# Ordner
mkdir $BUILDBASIS
mkdir $BUILDBASIS/data
mkdir $BUILDBASIS/logik

#cp _www/update/index.html $FINAL_data
#cp _www/update/*.png $FINAL_data
#cp _www/update/*.jpg $FINAL_data
cp *.py *.pyw  $BUILDBASIS
cp logik/*.py $BUILDBASIS/logik
cp data/*.* $BUILDBASIS/data
cp README.md $BUILDBASIS
# --- Erstelle Linux-Installation
echo "* Erzeuge Linux-Intstallation"
# Verzeichnisse
mkdir -p $BUILDLIN
# Basis
echo "- Basis"
cp -r $BUILDBASIS/* $BUILDLIN
# Info
echo "- Info"
cd $QUELLE
cp install/readme_linux.txt $BUILDLIN/readme.txt

# --- Erstelle Windows-Installation
echo "* Erzeuge Windows-Installation"
# Verzeichnisse
mkdir -p $BUILDWIN
# Basis
echo "- Basis"
cp -r $BUILDBASIS/* $BUILDWIN
# Info
echo "- Info"
cd $QUELLE
cp install/readme_win.txt $BUILDWIN/readme.txt
cp install/install_win.bat $BUILDWIN/install.bat

# --- Erstelle Mac-Installation
echo "* Erzeuge Mac-Installation"
# Verzeichnisse
echo "- App"
mkdir -p $BUILDMAC/$NAME.app/Contents/MacOS
mkdir -p $BUILDMAC/$NAME.app/Contents/Resources
# App
cp -r $BUILDBASIS/* $BUILDMAC/$NAME.app/Contents/MacOS
cp install/mac_bundle_prg_info_plist.txt $BUILDMAC/$NAME.app/Contents/Info.plist
cp install/mac_bundle_prg_starter.sh $BUILDMAC/$NAME.app/Contents/MacOS/start.sh
cp install/mac_bundle_prg_install.sh $BUILDMAC/$NAME.app/Contents/MacOS/install.sh
cp install/mac_bundle_prg_wrapper.sh $BUILDMAC/$NAME.app/Contents/MacOS/wrapper.sh
cp data/logo.icns $BUILDMAC/$NAME.app/Contents/Resources/icon.icns
# Info
echo "- Infos"
cd $QUELLE
cp install/readme_mac.txt $BUILDMAC/readme.txt

# --- Zip
echo "* Zip Packages"
cd $ZIEL/$TMP/
echo "- MacOs"
cd mac
zip -rq $FINAL/$NAME"_mac".zip ./*
cd ..
echo "- Windows"
cd win
zip -rq $FINAL/$NAME"_win".zip ./*
cd ..
echo "- Linux"
cd linux
zip -rq $FINAL/$NAME"_linux".zip ./*
cd ..

# --- Aufraeumen
echo "* Aufraeumen"
echo "- Loesche temporaeres Verzeichnis (info)";
rm -rf $ZIEL/$TMP
# --- Info
echo "* Zeige Deploy-Verzeichnis (info)"
ls -lah $FINAL
echo "* Zeige Deploy-Verzeichnis (data)"
ls -lah $FINAL



