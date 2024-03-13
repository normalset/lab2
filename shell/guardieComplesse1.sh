#!/bin/bash

is_even() {
  # Controlla se il numero è pari o dispari
  if (( $1 % 2 == 0 )); then
    return 0  
  else
    return 1 
  fi
}

if [ $# -ne 1 ];then
  echo "Numero Parametri Errato: $0 numero"
  exit -1
fi

if is_even $1; then
  echo "Numero Pari"
else
  echo "Numero Dispari"
fi
