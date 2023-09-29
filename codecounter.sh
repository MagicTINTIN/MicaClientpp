#!/bin/bash

echo Counting files of current directory

shopt -s extglob

existsinA(){
  if [ "$2" != in ]; then
    echo "Incorrect usage."
    echo "Correct usage: exists {key} in {array}"
    return
  fi   
  eval '[ ${'$3'[$1]+exist} ]'  
}

nottextfile=(jpg png kra kra~ gif out )

declare -A fileext
declare -A codefileext

if [[ $# == 1 ]]
then
    FILES=$1
else
    FILES=.
fi

foldernb=0
codefilenb=0
filenb=0
linenb=0
wordnb=0
charnb=0

searchin() {
    for f in $1/*
    do
        if [[ -f "$f" ]]
        then
            
            if [[ " ${nottextfile[*]} " == *" ${f##*.} "* ]]
            then
                let filenb++
                if existsinA ${f##*.} in fileext
                then 
                    let fileext[${f##*.}]++
                else
                    fileext[${f##*.}]=1
                fi
                echo "[F] $f"
            else
                let filenb++
                let codefilenb++
                let linenb+=`wc -l $f` 2>/dev/null
                let wordnb+=`wc -w $f` 2>/dev/null
                let charnb+=`wc -m $f` 2>/dev/null
                if existsinA ${f##*.} in codefileext
                then 
                    let codefileext[${f##*.}]++
                else
                    codefileext[${f##*.}]=1
                fi 
                echo "[C] $f"
            fi
        fi

        if [[ -d "$f" && "$f" != *"includes"* ]]
        then
            let foldernb++
            echo "[D] $f"
            searchin $f
        fi
    done
}

searchin $FILES;

echo -ne "
============= SUM UP =============

$foldernb folders
$codefilenb files of text/code
\n"

for key in "${!codefileext[@]}"; do echo -ne " .$key:${codefileext[$key]}(`echo "scale=2;100*${codefileext[$key]}/$codefilenb"|bc`%) "; done

echo -ne "\n
$filenb files
\n"

for key in "${!codefileext[@]}"; do echo -ne " .$key:${codefileext[$key]}(`echo "scale=2;100*${codefileext[$key]}/$filenb"|bc`%) "; done
echo -ne "\n"
for key in "${!fileext[@]}"; do echo -ne " .$key:${fileext[$key]}(`echo "scale=2;100*${fileext[$key]}/$filenb"|bc`%) "; done

echo -ne "\n
$linenb lines of text/code
$wordnb words
$charnb characters

Size : `du -sh --exclude='node_modules' $FILES`\n"
