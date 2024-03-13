#!/bin/bash
while true; do
    echo "Scegli un'opzione:"
    echo "1. Visualizza file"
    echo "2. Crea file"
    echo "3. Naviga tra le directory"
    echo "4. Esci"

    read choice

    case $choice in
        1) ls ;;
        2) echo "Inserisci il nome del nuovo file:"; read newfile; touch $newfile ;;
        3) echo "Inserisci il percorso della directory:"; read path; cd $path ;;
        4) exit ;;
        *) echo "Scelta non valida. Riprova." ;;
    esac
done
