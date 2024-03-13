#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
#include "macro.h"

#define MAX_FILE_NAME 1024
#define BUFFER_SIZE 32768
#define SHARED_MEM_NAME "/shared_memory"
#define SEM_MUTEX_NAME "/sem_mutex"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"


// Esercizio 1:
// Utilizzando le chiamate POSIX scrivi un programma che copia un file sorgente in uno destinazione usando buffer di dimensioni pari al 10 % della dimensione del file sorgente e in ogni caso maggiore di 16 bytes e minore di 32Kbyte.

int main(int argc, char*argv[]){
  if(argc != 3){
    perror("wrong arguments");
    exit(EXIT_FAILURE);
  }

  int rv, rfd, wfd;
  SYSC(rfd, open(argv[1], O_RDONLY), "nella open");
  SYSC(wfd, open(argv[2], O_CREAT | O_WRONLY | O_APPEND, 0666), "nella open");

  //get file size
  struct stat fileStat;
  SYSC(rv, fstat(rfd, &fileStat), "nella fstat");
  int buffersize = fileStat.st_size / 10 ;  

  char* buffer[buffersize] ; 
  while(read(rfd, buffer, buffersize)){
    SYSC(rv, write(wfd, buffer, buffersize), "nella write");
  }

  SYSC(rv, close(rfd), "nella close");
  SYSC(rv, close(wfd), "nella close");
  return 0;
}