#!/bin/bash
echo "Inserisci il nome del file CSV:"
read filename

echo "Inserisci il campo"
read field

awk -F',' "\$$field > 6" "$filename"
