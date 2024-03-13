 #!/bin/bash

 #Script che prende due argomenti, una directory D e una parola W e stmpa i nomi di tutti i file della directory D che contengono la parola W.

 #Controllo sul numero di parametri 
 if [ $# -ne 2 ]; then
    echo "Usa: $(basename $0) dir word"
    #con lo string matching
    echo "Usa: $(0##*/) dir word"
    exit -1
fi

if [ ! -d $1 ]; then                # se ! not -d directory $1 elemento 1 passato allo script
    echo "$1 non e' una directory"
    exit -2
fi

#mi sposto dentro la directory ricordandomi da dove sono partito
pushd $1 > /dev/null                 #pushd carica in una stack un comando rieseguendolo quando si fa una pop popd
                                     #> ridirige la stampa di quel comando in /dev/null che e' un void che cancella l'output
for file in ./* do
    if [ ! -d $file ]; then         #controllo che non sia una directory 
        if grep -q $2 $file ; then  #con -q non stampa il risultato, prende un file e controlla se c'e' la stringa che sto cercando
            echo $file
        fi
    fi
done

