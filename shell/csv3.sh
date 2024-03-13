#!/bin/bash
echo "Inserisci il nome del file CSV:"
read filename

echo "Inserisci il campo"
read field

echo "Inserisci il valore soglia"
read threshold

awk -F',' "\$$field > $threshold" "$filename"
