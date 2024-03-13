/*
Crea usando le chiamate di sistema posix unb programma che general due processi figli ai quali passa una sequenza di 9 numeri, uno ogni 5 secondi, tramite una pipe senza nome, generato casualmente nell'intervallo [0 ; 100]

il decino numero passato ad ogni processo e' -1

alla ricezione di un numero maggiore di 0 ogni processo figlio stampa a video un riga formata dal proprio PID seguito da uno spazio e dal numero ricevuto
Alla ricezione del numero -1 il processo figlio termina

il padre dopo aver spedito tutti i numeri resta in attesa della terminazione dei processi figli
tutte le chiamate di sistema vanno controllate usando una delle seguenti macro
*/

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

#include <time.h>

int main(){
  int rv;
  int pfd[2][2];

  SYSC(rv, pipe(pfd[0]),"nella pipe");
  SYSC(rv, pipe(pfd[1]),"nella pipe");

  pid_t cpid1, cpid2;
  SYSC(cpid1, fork(), "nella fork");
  if(cpid1 == 0){
    // figlio
    SYSC(rv, close(pfd[0][1]), "nella close pipe");

    int n;
    pid_t fpid = getpid();
      while (1)
      {
        SYSC(rv, read(pfd[0][0], &n, sizeof(int)), "nella read");

        if (n == -1)
        {
          exit(EXIT_SUCCESS);
        }

        printf("Sono il processo 1 (%d) con il numero %d\n", fpid, n);
      }
      SYSC(rv, close(pfd[0][1]), "nella close pipe");
    }
    //figlio 2
  SYSC(cpid2, fork(), "nella fork");
  if(cpid2 == 0){
    // figlio
    SYSC(rv, close(pfd[1][1]), "nella close pipe");

    int n;
    pid_t fpid = getpid();
      while (1)
      {
        SYSC(rv, read(pfd[1][0], &n, sizeof(int)), "nella read");

        if (n == -1)
        {
          exit(EXIT_SUCCESS);
        }

        printf("Sono il processo 2 (%d) con il numero %d\n", fpid, n);
      }
      SYSC(rv, close(pfd[1][1]), "nella close pipe");
    }

  //padre

  SYSC(rv, close(pfd[0][0]),"nella close pipe");
  SYSC(rv, close(pfd[1][0]),"nella close pipe");

  //scrivo n volte il vaalore rand [0,100] poi -1 in entrambe le pipe
  int n ;
  for(int i = 0 ; i < 9 ; i ++){
    //wait 5 sec
    SYSC(rv, sleep(1), "nella sleep");

    n = rand() % 100;
    SYSC(rv, write(pfd[0][1], &n , sizeof(int)), "nella write");
    n = rand() % 100;
    SYSC(rv, write(pfd[1][1], &n , sizeof(int)), "nella write");
  }
  n = -1;
  SYSC(rv, write(pfd[0][1], &n, sizeof(int)), "nella write");
  SYSC(rv, write(pfd[1][1], &n, sizeof(int)), "nella write");

  //chiude le pipe
  SYSC(rv, close(pfd[0][1]), "nella close pipe");
  SYSC(rv, close(pfd[1][1]), "nella close pipe");
  //wait per i processi figli
  SYSC(rv, waitpid(-1, NULL, 0), "nella waitpid");
  SYSC(rv, waitpid(-1, NULL, 0), "nella waitpid");
  exit(EXIT_SUCCESS);

}