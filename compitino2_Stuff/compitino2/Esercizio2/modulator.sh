#/bin/bash

#$1 field id in [1-3]
#$2 0 pari 1 disp
#$3 file

if [ $1 -lt 1 ]; then
  echo "field id non e' nel range permesso [1-3]"
  exit -1
else
  fieldID=$1
fi

if [ $1 -gt 3 ]; then
  echo "field id non e' nel range permesso [1-3]"
  exit -2
else
  fieldID=$1
fi

if [ $2 != 0 ]; then
  if [ $2 != 1 ]; then
    echo "Campo 2 sbagliato, deve essere 0 o 1"
    exit -3
  fi
fi
mod=$2

if [ ! -d $3 ]; then
  echo "il terzo parametro deve essere un file"
  exit -3
else
  file=$3
fi

#prendo il campo $1 controllo se sia pari o dispari (mod == 0 o 1) e lo applico sul file $3
awk -F',' '($fieldID % 2 == $mod)' "$file" 
