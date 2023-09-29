#!/usr/bin/bash
STYLEName=("NORMAL" "BOLD" "THIN" "ITALIC" "UNDERLINED" "PURPLE" "CYAN" "WHITE_NORMAL_COLOR")
colorName=("BLACK" "RED" "GREEN" "YELLOW" "BLUE" "PURPLE" "CYAN" "WHITE_NORMAL_COLOR")

colorName() {
    if [[$2 -eq 3 ]]; then
        type="_NORMAL_COLOR"
    elif [[$2 -eq 4 ]]; then
        type="_NORMAL_BACKGROUND"
    elif [[$2 -eq 3 ]]; then
        type="_DESAT_COLOR"
    elif [[$2 -eq 3 ]]; then
        type="_DESAT_BACKGROUND"
    fi
    echo "#define ${colorName[$1]}$type"
}

echo -ne "#pragma once

#if defined(__APPLE__) || defined(__linux__)
//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
"

for ((j=0; j<11; j++))
do
    for k in 3 4 9 10
    do
        for ((i=0; i<8; i++))
        do
            #echo -ne " \e[$im$k${i}m \e[${j}m [ \\\e[0;$k${i}m\\\e[${j}m ]\e[0m"
            # if [[ $j -lt 10 ]] && [[ $k -lt 10 ]]; then
            #     echo -ne " \e[0;$k${i}m\e[${j}m[  \\\e[$k${i}m\\\e[${j}m  ]\e[0m"
            # elif [[ $j -lt 10 ]] && [[ $k -eq 10 ]]; then
            #     echo -ne " \e[0;$k${i}m\e[${j}m[ \\\e[$k${i}m\\\e[${j}m  ]\e[0m"
            # elif [[ $j -gt 9 ]] && [[ $k -lt 10 ]]; then
            #     echo -ne " \e[0;$k${i}m\e[${j}m[ \\\e[$k${i}m\\\e[${j}m  ]\e[0m"
            # else
            #     echo -ne " \e[0;$k${i}m\e[${j}m[ \\\e[$k${i}m\\\e[${j}m ]\e[0m"
            # fi
            #codeName $i
            echo $i $j $k
        done
    
        echo -ne "\n"
    done
    echo -ne "\n"
done

exit
# other systems

echo -ne "\n
// other os
#else
\n"


for ((j=0; j<11; j++))
do
    for k in 3 4 9 10
    do
        for ((i=0; i<8; i++))
        do
            if [[ $j -lt 10 ]] && [[ $k -lt 10 ]]; then
                echo -ne " \e[${j}m\e[$k${i}m[  \\\e[${j}m\\\e[$k${i}m  ]\e[0m"
            elif [[ $j -lt 10 ]] && [[ $k -eq 10 ]]; then
                echo -ne " \e[${j}m\e[$k${i}m[ \\\e[${j}m\\\e[$k${i}m  ]\e[0m"
            elif [[ $j -gt 9 ]] && [[ $k -lt 10 ]]; then
                echo -ne " \e[${j}m\e[$k${i}m[ \\\e[${j}m\\\e[$k${i}m  ]\e[0m"
            else
                echo -ne " \e[${j}m\e[$k${i}m[ \\\e[${j}m\\\e[$k${i}m ]\e[0m"
            fi
            
            
        done
    
        echo -ne "\n"
    done
    echo -ne "\n"
done

echo -ne "
\n
#endif" >> colors.h