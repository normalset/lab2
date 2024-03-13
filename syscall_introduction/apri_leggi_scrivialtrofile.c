// Scrivere un programma che utilizzando le chiamate di sistema viste a lezione apre un file di testo( passato come argomento dalla linea di comando) e ne scrive il contenuto su un secondo file
// Usando open, read, write, close, stat 
//rispetto al apri_leggi_chiudi devo controllare che ci siano 3 elementi passati in line di comando, fare 2x stat, fare 2x open, scrivere sul file al posto che sullo STDOUT_FILENO e chiudere entrambi i file
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"

int main(int argc, char* argv[]){
  //controllo il numero di parametri passati alla funzione devo avere nome programma, file in file out
  if(argc != 3){
    printf("Errore nel numero di parametri passati al programma\n");
    exit(EXIT_FAILURE);
  }

  // Assicuriamoci che il file passato sia corretto -> STAT
  int returnvalue ;
  struct stat statbuf ;
  //controllo che i due file siano file regolari
  SYSC(returnvalue , stat(argv[1] , &statbuf), "msg") ;
  if(!S_ISREG(statbuf.st_mode)){
    printf("%s non e' file regolare", argv[1]);
    exit(EXIT_FAILURE);
  }
  SYSC(returnvalue , stat(argv[2] , &statbuf), "msg") ;
  if(!S_ISREG(statbuf.st_mode)){
    printf("%s non e' file regolare", argv[2]);
    exit(EXIT_FAILURE);
  }

  //? apro il file in read 
  int fdread;
  SYSC(fdread, open(argv[1], O_RDONLY), "nella open di fdread"); //posso passare solo 2 argomenti alla open dato che e' in sola lettura
  char buffer[BUF_SIZE]; //settato nel file macro.h a 256 con una #define BUF_SIZE 256

  //? apro il file in write
  int fdwrite ;
  SYSC(fdwrite , open(argv[2] , O_WRONLY | O_CREAT | O_TRUNC, 0644), "nella open della di fdwrite") ;/* 0644 sono i permessi    
                                                              Il proprietario del file ha il permesso di lettura e scrittura (6).
                                                              Il gruppo del file ha solo il permesso di lettura (4).
                                                              Gli altri utenti hanno solo il permesso di lettura (4). */

  //Leggo il file
  while(returnvalue = read(fdread , buffer, BUF_SIZE)){
    //dopo aver letto il file scrivo a video
    SYSC(returnvalue , write(fdwrite , buffer , returnvalue) , "nella write");
  }

  //chiudo i file
  SYSC(returnvalue , close(fdread) , "nella close");
  SYSC(returnvalue , close(fdwrite) , "nella close");

  return 0;
}