#!/bin/bash
# run_bsrn_toolbox.sh

#echo queries:
set -e
set -u

# script to run BSRN_Toolbox on a Linux 64 bit system

# ODVMPHOME=/home/bawagner/ODV4/odv_4.7.10_linux-amd64_ubuntu-14.04
BSRNTOOLBOX=dirname "$0"
echo ${BSRNTOOLBOX}

#-----------------------------------------------------------------------
# nothing to be changed below this line

# set LD_LIBRARY_PATH
if [ -z LD_LIBRARY_PATH ]
then
LD_LIBRARY_PATH=${BSRNTOOLBOX}
else
LD_LIBRARY_PATH=${BSRNTOOLBOX}:${LD_LIBRARY_PATH}
fi

# set PATH
PATH=${PATH}:${BSRNTOOLBOX}

export LD_LIBRARY_PATH
export PATH

exec ${BSRNTOOLBOX}/BSRN_Toolbox
