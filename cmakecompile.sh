#!/usr/bin/bash

mkdir build

makearg=""
cmakearg=""
for arg in "$@"; do
    if [ "$arg" = "--quickprog" ]; then
        cmakearg+=" -DCMAKE_BUILD_TYPE=Release "
    elif [ "$arg" = "--fastcomile" ]; then
        makearg+=" -j4 "
    elif [ "$arg" = "--reset" ]; then
        rm -rf build/*
    fi
done
cd build/
cmake $cmakearg ..
if [[ $? != 0 ]]; then
    err=$?
    echo -ne "$(tput setaf 9)"
    echo "Compilation of MicaClient went wrong.$(tput sgr0)"
    cd ../
    exit $err
fi
make $makearg
if [[ $? != 0 ]]; then
    err=$?
    echo -ne "$(tput setaf 9)"
    echo "Compilation of MicaClient went wrong.$(tput sgr0)"
    cd ../
    exit $err
elif [[ $? == 0 ]]; then
    echo "$(tput setaf 2)$(tput bold)Compilation of MicaClient finished.$(tput sgr0)
    Execute MicaClient++ with build/MicaClient"
fi

cd ../
echo -ne "$(tput sgr0)"
