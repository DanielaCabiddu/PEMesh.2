#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

#GKlib
cd ${SCRIPT_DIR}/../external/GKLib
rm CMakeCache.txt

make config prefix=${SCRIPT_DIR}/../external/GKLib/install-Linux openmp=set
make
make install

#METIS
cd ${SCRIPT_DIR}/../external/METIS
rm CMakeCache.txt

make config shared=1 cc=gcc prefix=${SCRIPT_DIR}/../external/METIS/build-Linux gklib_path=${SCRIPT_DIR}/../external/GKLib/install-Linux
make install
