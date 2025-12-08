#!/bin/bash

function info_section(){
echo | tee -a wrapper.log.txt
echo '-----------------------------------------------------' | tee -a wrapper.log.txt
echo $* | tee -a wrapper.log.txt
echo '-----------------------------------------------------' | tee -a wrapper.log.txt
echo | tee -a wrapper.log.txt
}

# Standardkodierung definieren (wichtig für das Lesen von Dateien)
export LANG=de_DE.UTF-8

# Wechsel in Verzeichnis indem dieses Programm liegt
DIR="$(dirname "$0")"
cd $DIR

# Log
touch wrapper.log.txt
echo > wrapper.log.txt

# Checking Hardware
info_section "Checking Hardware"

# Get ini file
if [ -f arch.ini ]; then
    echo 'Architecture ini-file found -> loading' | tee -a wrapper.log.txt
else
    echo 'Architecture ini-file no found -> create it (default: x86_64)' | tee -a wrapper.log.txt
    touch arch.ini
    echo "-x86_64" > arch.ini
fi
arch_ini=$(cat arch.ini)
echo "Architecture:" $arch_ini  | tee -a wrapper.log.txt

# Startibng Starter
info_section "Launching Starter"
echo "Woooosh ..." | tee -a wrapper.log.txt
arch $arch_ini ./start.sh 

