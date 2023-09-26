#!/usr/bin/bash
echo -ne "$(tput setaf 9)"
if [[ $1 == "-quickprog" ]]
then
    slowcompile=" -O3 -flto -funroll-loops -mtune=native -march=native "
else
    slowcompile=""
fi

g++ -o MicaClient.out ./main.cpp $slowcompile -lcurl
if [[ $? == 0 ]]; then
    echo "$(tput setaf 2)$(tput bold)Compilation of MicaClient finished.$(tput sgr0)"
fi

echo -ne "$(tput sgr0)"