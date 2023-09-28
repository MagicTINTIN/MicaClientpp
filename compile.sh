#!/usr/bin/bash
echo -ne "$(tput setaf 9)"
if [[ $1 == "-quickprog" ]]
then
    slowcompile=" -O3 -flto -funroll-loops -mtune=native -march=native "
else
    slowcompile=""
fi

g++ ./main.cpp -o MicaClient.out ./internal/message.cpp ./internal/messagememory.cpp ./internal/aes.c ./internal/requests.cpp ./internal/tools.cpp $slowcompile -lcurl
if [[ $? == 0 ]]; then
    echo "$(tput setaf 2)$(tput bold)Compilation of MicaClient finished.$(tput sgr0)"
fi

echo -ne "$(tput sgr0)"
