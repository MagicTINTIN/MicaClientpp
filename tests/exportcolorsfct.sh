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
    echo "  if (color == \"${colorName[$1]}$type\")
        return ${colorName[$1]}$type;
" >> colors.cpp 
}

styleDefineName() {
    echo "  if (color == \"${styleName[$1]}\")
        return ${styleName[$1]};
" >> colors.cpp 
}

echo -ne "#include \"colors.h\"
#include <string>

std::string getColor(std::string color)
{
" > colors.cpp

for ((j=0; j<10; j++))
do
    styleDefineName $j 1
done

echo -ne "\n" >> colors.cpp

for k in 3 4 9 10
do
    for ((i=0; i<8; i++))
    do
        colorDefineName $i $k 1
    done
done


echo -ne "  return \"\";
}
" >> colors.cpp