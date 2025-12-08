#!/bin/bash
# Info: Noetige PyQt Version: PyQt5==5.10.0

# Xcode Commandline Tools:
# Install: xcode-select --install (da kommt Python 3.7 mit)
# Ort: /Library/Developer/CommandLineTools/
# pip3 --user Verzeichnis: /Users/pkloss/Library/Python/
# Achtung: pip3 user update zerstoert pip3

# Definition
PY_MODULE="PyQt5"
PY_MODULE_PIP="PyQt5"
INFO_INST_MANUELL="\n
* Please install Python3.7 first:\nwww.python.org/downloads\n
* Then install the Xcode CommandLineTools the command line:\nxcode-select --install\n
* Then install the following modules in the command line:\npip3 install --user $PY_MODULE_PIP\n
* When you contact a BSRNtoolbox administrator, please send him the console output that has been opened right now"

# Infos
function info_bsrn(){
echo | tee -a install.log.txt
echo '  ____   _____ _____  _   _   _              _ _               ' | tee -a install.log.txt
echo ' |  _ \ / ____|  __ \| \ | | | |            | | |              ' | tee -a install.log.txt
echo ' | |_) | (___ | |__) |  \| | | |_ ___   ___ | | |__   _____  __' | tee -a install.log.txt
echo ' |  _ < \___ \|  _  /| . ` | | __/ _ \ / _ \| |  _ \ / _ \ \/ /' | tee -a install.log.txt
echo ' | |_) |____) | | \ \| |\  | | || (_) | (_) | | |_) | (_) >  < ' | tee -a install.log.txt
echo ' |____/|_____/|_|  \_\_| \_|  \__\___/ \___/|_|_.__/ \___/_/\_\' | tee -a install.log.txt
echo '=====================================================' | tee -a install.log.txt
echo '                  Install/Repair Script' | tee -a install.log.txt
echo '=====================================================' | tee -a install.log.txt
}

function info_section(){
echo | tee -a install.log.txt
echo '-----------------------------------------------------' | tee -a install.log.txt
echo $* | tee -a install.log.txt
echo '-----------------------------------------------------' | tee -a install.log.txt
echo | tee -a install.log.txt
}

# Dialog
function dialog_info() {
    osascript -e 'display alert "'"$1"'" message "'"$2"'"'
}
function dialog_info_critical() {
    osascript -e 'display alert "'"$1"'" message "'"$2"'" as critical'
}

# Fehler
function fehler(){
	osascript -e 'display notification "installation failed ..." with title "BSRNtoolbox" sound name "submarine"'
	open -a TextEdit install.log.txt
	dialog_info_critical "Automatic installation failed" "Please install the necessary dependencies manually. $INFO_INST_MANUELL"
    exit
}

# Teste Rueckgabewert
function check_prg(){
    eval "$*" >> install.log.txt 2>&1
    if test $? -ne 0; then RETURN=false; else RETURN=true; fi
}

# ----

# Log
rm install.log.txt 2> /dev/null
touch install.log.txt

# Pfade holen, damit Python3 gefunden wird
source /etc/profile 2> /dev/null
source ~/.bash_profile 2> /dev/null

# Wechsel in Verzeichnis indem dieses Programm liegt
DIR="$(dirname "$0")"
cd $DIR

# Intro
clear
info_bsrn
RET=$(osascript -e 'display alert "Install or Repair\nBSRNtoolbox" message "Press OK to install/repair all needed dependencies" buttons {"OK","Cancel"} default button 1')
if test "$RET" = "button returned:Cancel"; then exit; fi

# Setting up Hardware Architecture
info_section "Setting up hardware architechture"
arch_ini=""
PS3="Select your Apple hardware architechture please: "
select lng in Intel Apple
do
case $lng in
"Intel")
echo "Selected hardware architecture: Intel";arch_ini="-x86_64";break;;
"Apple")
echo "Selected hardware architecture: Apple Silicon";arch_ini="-arm64";break;;
*)
echo "Ooops";;
esac
done
echo "Writing Architecture to ini-file: $arch_ini" | tee -a install.log.txt
echo $arch_ini > arch.ini

# Teste Python3
info_section "Testing Python3"
check_prg 'which python3'
if ! $RETURN; then
    # Python3 ist nicht installiert
    echo "Python3 is not installed" | tee -a install.log.txt
    RET=$(osascript -e 'display alert "Python3 is not installed" message "Please install Python3.7+ from www.python.org/downloads" buttons {"OK","Cancel"} default button 1')
    exit
fi
echo "Python3 is installed" | tee -a install.log.txt
echo "Version: " $(python3 -V) | tee -a install.log.txt
echo "User base dir: " $(python3 -c "import site; print(site.getuserbase())") | tee -a install.log.txt
echo "Site packages: " $(python3 -c "import site; print(site.getsitepackages())") | tee -a install.log.txt
echo "User site packages: " $(python3 -c "import site; print(site.getusersitepackages())") | tee -a install.log.txt

# Teste ob Pip3 installiert ist
info_section "Testing Pip3"
check_prg 'which pip3'
if ! $RETURN; then echo "Pip3 is not installed" | tee -a install.log.txt
  fehler
else
  echo "Pip3 is installed" | tee -a install.log.txt
  echo "Version: " $(pip3 -V) | tee -a install.log.txt
fi
RET=$(osascript -e 'display alert "Python3 and Pip3 are installed" message "Now the missing Python modules are determined and installed" buttons {"OK","Cancel"} default button 1')

# Teste und installiere fehlende Python-Module
info_section "Checking for missing Python modules"
echo "Required modules: "$PY_MODULE | tee -a install.log.txt
check_prg 'python3 -c "import '$PY_MODULE'"'
if ! $RETURN; then
    # Installiere fehlde Python Module
    echo "* Some modules are missing -> Start Installation" | tee -a install.log.txt
    pip3 install --user $PY_MODULE_PIP
    # Nochmal testen
    echo "* Checking for missing Python modules again" | tee -a install.log.txt
    echo "Required modules: "$PY_MODULE | tee -a install.log.txt
    check_prg 'python3 -c "import '$PY_MODULE'"'
    if ! $RETURN; then
      # Immer noch ein Problem -> Manuelle Installation
      echo "* Some dependencies are still not fulfilled" | tee -a install.log.txt
      fehler
    exit
fi
fi
echo "All modules are found" | tee -a install.log.txt

# Abschlusstest
info_section "Testing all dependencies again"
echo "Required modules: "$PY_MODULE | tee -a install.log.txt
check_prg 'python3 -c "import '$PY_MODULE'"'
if $RETURN;
    then
        # Ok: Starte Programm
        echo "All dependencies are fulfilled" | tee -a install.log.txt
	    dialog_info "Everything is OK" "All dependencies are fulfilled. BSRNtoolbox will be started now. If this fails, please start manually"
        open -a TextEdit install.log.txt
		open "../../../BSRNtoolbox.app"
        exit
    else
        # Immer noch ein Problem -> Manuelle Installation
        echo "Still not all dependencies are fulfilled" | tee -a install.log.txt
        open -a TextEdit install.log.txt
        fehler
        exit
fi