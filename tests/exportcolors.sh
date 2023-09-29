#!/usr/bin/bash
styleName=("NORMAL" "BOLD" "THIN" "ITALIC" "UNDERLINED" "BLINK" "BLINK2" "REVERSED" "INVISIBLE" "STRIKED")
colorName=("BLACK" "RED" "GREEN" "YELLOW" "BLUE" "PURPLE" "CYAN" "WHITE")

colorDefineName() {
    if [[ $2 -eq 3 ]]; then
        type="_NORMAL_COLOR"
    elif [[ $2 -eq 4 ]]; then
        type="_NORMAL_BACKGROUND"
    elif [[ $2 -eq 9 ]]; then
        type="_DESAT_COLOR"
    elif [[ $2 -eq 10 ]]; then
        type="_DESAT_BACKGROUND"
    fi
    if [[ $3 -eq 1 ]]; then
        val="\"\e[${2}${1}m\""
    else
        val="\"\""
    fi
    echo "#define ${colorName[$1]}$type $val" >> colors.h 
}

styleDefineName() {
    if [[ $2 -eq 1 ]]; then
        val="\"\e[${1}m\""
    else
        val="\"\""
    fi
    echo "#define ${styleName[$1]}$type $val" >> colors.h 
}

echo -ne "#pragma once

#if defined(__APPLE__) || defined(__linux__)
//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
" > colors.h

for ((j=0; j<10; j++))
do
    styleDefineName $j 1
done

echo -ne "\n" >> colors.h

for k in 3 4 9 10
do
    for ((i=0; i<8; i++))
    do
        colorDefineName $i $k 1
    done
done


echo -ne "\n
// other os
#else
\n" >> colors.h 

for ((j=0; j<10; j++))
do
    styleDefineName $j 0
done

echo -ne "\n" >> colors.h

for k in 3 4 9 10
do
    for ((i=0; i<8; i++))
    do
        colorDefineName $i $k 0
    done
done

echo -ne "
\n
#endif" >> colors.h