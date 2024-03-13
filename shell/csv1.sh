#!/bin/bash
echo "Inserisci il nome del file CSV:"
read filename

awk -F',' '$2 > 6' "$filename"
