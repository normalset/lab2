// Scrivere un programma che utilizzando le chiamate di sistema viste a lezione apre un file di testo( passato come argomento dalla linea di comando) e ne stampa il contenuto a video 
// Usando open, read, write, close, stat 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"

int main(int argc, char* argv[]){
  //controllo il numero di parametri passati alla funzione
  if(argc != 2){
    printf("Errore nel numero di parametri passati al programma\n");
    exit(EXIT_FAILURE);
  }

  // Assicuriamoci che il file passato sia corretto -> STAT
  int returnvalue ;
  struct stat statbuf ;
  SYSC(returnvalue , stat(argv[1] , &statbuf), "msg") ;
  if(!S_ISREG(statbuf.st_mode)){
    printf("%s non e' file regolare", argv[1]);
    exit(EXIT_FAILURE);
  }

  //? apro il file
  int fd;
  SYSC(fd, open(argv[1], O_RDONLY), "nella open"); //posso passare solo 2 argomenti alla open dato che e' in sola lettura
  char buffer[BUF_SIZE]; //settato nel file macro.h a 256 con una #define BUF_SIZE 256

  //Leggo il file
  while(returnvalue = read(fd , buffer, BUF_SIZE)){
    //dopo aver letto il file scrivo a video
    SYSC(returnvalue , write(STDOUT_FILENO , buffer , returnvalue) , "nella write");
  }

  //chiudo il file
  SYSC(returnvalue , close(fd) , "nella close");

  return 0;
}