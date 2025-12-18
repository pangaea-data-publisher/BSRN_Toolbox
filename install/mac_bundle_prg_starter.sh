#!/bin/bash

# Info: Noetige PyQt Version: PyQt5==5.10.0

# Standardkodierung definieren (wichtig für das Lesen von Dateien)
export LANG=de_DE.UTF-8

# Definitionen
PY_MODULE="PyQt5"
PY_MODULE_PIP="PyQt5"
INFO_INST_MANUELL="Please install Python3.7 first:\nhttps://www.python.org/downloads\n\nThen install the following modules in the command line:\npip3 install --user $PY_MODULE_PIP"

function info_section(){
echo | tee -a start.log.txt
echo '-----------------------------------------------------' | tee -a start.log.txt
echo $* | tee -a start.log.txt
echo '-----------------------------------------------------' | tee -a start.log.txt
echo | tee -a start.log.txt
}

# Teste Rueckgabewert
function check_prg(){
    eval "$*" >> start.log.txt 2>&1
    if test $? -ne 0; then RETURN=false; else RETURN=true; fi
}

# Pfade holen, damit Python3 gefunden wird
source /etc/profile 2> /dev/null
source ~/.bash_profile 2> /dev/null

# Wechsel in Verzeichnis indem dieses Programm liegt
DIR="$(dirname "$0")"
cd $DIR

# Notification
afplay /System/Library/Sounds/Frog.aiff

# Log
touch start.log.txt
echo > start.log.txt
echo | tee -a start.log.txt
echo '  ____   _____ _____  _   _   _              _ _               ' | tee -a start.log.txt
echo ' |  _ \ / ____|  __ \| \ | | | |            | | |              ' | tee -a start.log.txt
echo ' | |_) | (___ | |__) |  \| | | |_ ___   ___ | | |__   _____  __' | tee -a start.log.txt
echo ' |  _ < \___ \|  _  /| . ` | | __/ _ \ / _ \| |  _ \ / _ \ \/ /' | tee -a start.log.txt
echo ' | |_) |____) | | \ \| |\  | | || (_) | (_) | | |_) | (_) >  < ' | tee -a start.log.txt
echo ' |____/|_____/|_|  \_\_| \_|  \__\___/ \___/|_|_.__/ \___/_/\_\' | tee -a start.log.txt
echo '=====================================================' | tee -a start.log.txt
echo '                  Start up ...' | tee -a start.log.txt
echo '=====================================================' | tee -a start.log.txt

# ---

# Teste Hardware Architektur
info_section "Checking Hardware"
# Get ini file
arch_ini=""
if [ -f arch.ini ]; then
    echo 'Architecture ini-file found -> loading' | tee -a start.log.txt
else
    echo 'Architecture ini-file no found -> create it (default: x86_64)' | tee -a start.log.txt
    touch arch.ini
    echo "-x86_64" > arch.ini
fi
arch_ini=$(cat arch.ini)
echo "Loaded architecture from ini-file:" $arch_ini  | tee -a start.log.txt
echo "Currently used architecture: "$(arch) | tee -a start.log.txt

# Teste Python
info_section "Checking Python"
echo "Environment:" $(env)  | tee -a start.log.txt
echo "Python Version:" $(python3 -V) | tee -a start.log.txt
echo "User base dir:" $(python3 -c "import site; print(site.getuserbase())") | tee -a start.log.txt
echo "Site packages:" $(python3 -c "import site; print(site.getsitepackages())") | tee -a start.log.txt
echo "User site packages:" $(python3 -c "import site; print(site.getusersitepackages())") | tee -a start.log.txt

# Teste Abhaengigkeiten
info_section "Checking dependencies"
check_prg 'python3 -c "import '$PY_MODULE'"'

# *** Alles OK (Start)
if $RETURN; then
    afplay /System/Library/Sounds/Glass.aiff
    echo 'All components found' | tee -a start.log.txt
    info_section "Summary"
    echo 'Everything is fine' | tee -a start.log.txt
    echo 'Starting Collector App ...' | tee -a start.log.txt
    python3 start.pyw
    exit 0
fi

# *** Fehler (Repair/Manual)
afplay /System/Library/Sounds/Submarine.aiff

echo "Missing components"
info_section "Summary"
echo 'Something went wrong' | tee -a start.log.txt

# Frage (Repair/Manual)
RET=$(osascript -e 'display alert "Missing components" message "Do you want to install the missing components manually or should I start the repair tool?" as critical buttons {"Repair","Install manually"} default button 1')
echo $RET

# Manual Installation
if test "$RET" = "button returned:Install manually"; then
    osascript -e 'display alert "Manual installation" message "'"$INFO_INST_MANUELL"'"'
    exit 0
fi

# Repair Script



open -a Terminal "install.sh"
