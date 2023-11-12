#!/usr/bin/bash

mkdir build
mkdir debug

makearg=""
cmakearg=""
folder="build"
for arg in "$@"; do
    if [ "$arg" = "--release" ]; then
        cmakearg+=" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=\"-O3\" "
    elif [ "$arg" = "--debug" ]; then
        cmakearg+=" -DCMAKE_BUILD_TYPE=Debug "
        folder="debug"
    elif [ "$arg" = "--fastcomile" ]; then
        makearg+=" -j4 "
    elif [ "$arg" = "--reset" ]; then
        rm -rf build/*
        rm -rf debug/*
    fi
done

cd $folder/
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
