#!/bin/bash

is_integer_greater_or_equal_to_zero() {
  # Utilizza una espressione regolare per verificare se la stringa è un numero intero maggiore o uguale a zero
  if [[ "$1" =~ ^[0-9]+$ && "$1" -ge 0 ]]; then
    return 0  # È un numero intero maggiore o uguale a zero
  else
    return 1  # Non è un numero intero maggiore o uguale a zero
  fi
}

if [ $# -ne 3 ]; then
    echo "Usage: $0 field_id threshold filename"
    exit -1
fi

if [[ $(is_integer_greater_or_equal_to_zero "$1") -eq 0 && "$1" -lt 4 ]]; then
    field=$1
else
    echo "field parameter should be greater then or equal to 0"
    exit -2
fi

if is_integer_greater_or_equal_to_zero "$1" && [ "$1" -lt 4 ]; then
    field=$1
else
    echo "field parameter should be greater then or equal to 0"
    exit -2
fi

if is_integer_greater_or_equal_to_zero "$2"; then
    threshold=$2
else
    echo "threshold parameter should be greater then or equal to 0"
    exit -2
fi

if [ ! -f $3 ]; then
    echo "filename should be the name of a valid csv file"
    exit -3
else
    filename=$3
fi

awk -F',' "\$$field > $threshold" "$filename"
