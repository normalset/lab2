/*

programma che genera 3 processi figli con i quali condivide uno spazio di memoria condivisa della dimensione di 2 interi

il primo figlio genera un numero casuale compreso tra 1 e 9 e lo memorizza nel primo spazioe di memoria condivisa restando in attesa fintanto che il secondo processo non lo abbia letto

il secondo processo dopo averlo letto dal primo spazioe e scritto nel secondo spazio, si mette in attesa fintanto che il terzo processo non lo abbia letto

il terzo processo dopo averlo letto dal secondo spazio lo scrive a video

tutte le chiamate di sistema vanno conotrollate usando macro di macro.h

*/


//!
//!
//! DA FINIRE IT DOESNT WORK :(
//!
//!
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
  //creo la shared memory e la dimensiono con ftruncate
  int sm, rv;
  SYSC(sm, shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666) , "nella open");
  SYSC(rv, ftruncate(sm, 2 * sizeof(int)), "nella ftruncate");

  int * shared_memory;
  SYSCN(shared_memory, 
        mmap(NULL, (2 * sizeof(int)), PROT_READ | PROT_WRITE | MAP_SHARED, 0, sm , 0) 
        ,"nella mmap")
  SYSC(rv, close(sm), "nella close");
  
  //casto la shared memory 

  //creo i due semafori 
  sem_t sem12;
  sem_t sem21;
  sem_t sem23;
  sem_t sem32;
  sem_init(&sem12, 1, 0);
  sem_init(&sem23, 1, 0);
  sem_init(&sem21, 1, 0);
  sem_init(&sem32, 1, 0);
  
  pid_t cp1, cp2, cp3 ;

  //primo figlio
  SYSC(cp1, fork(), "nella fork1");
  if(cp1==0){
    printf("Inizio filio 1");
    int n = rand() % 9 + 1;
    shared_memory[0] = n;
    sem_post(&sem21);
    sem_wait(&sem12);
    printf("figlio 1, finito di aspettare il figlio 2");
    exit(EXIT_SUCCESS);
  }

  //secondo figlio
  SYSC(cp2, fork(), "nella fork2");
  if(cp2 == 0){
    printf("Inizio filio 2");
    sem_wait(&sem21);
    shared_memory[1] = shared_memory[0];
    sem_post(&sem12);
    printf("F2, postato il semaforo 12");
    sem_post(&sem32);
    sem_wait(&sem23);
    printf("F2, finito di aspettare F3");
    exit(EXIT_SUCCESS);
  }

  //terzo figlio
  SYSC(cp3, fork(), "nella fork3");
  if(cp3 == 0){
    printf("Inizio filio 3");
    sem_wait(&sem32);
    int n;
    n = shared_memory[1];
    sem_post(&sem23);
    printf("F3, postato sem23 e ricevuto il numero (%d)", n);
    exit(EXIT_SUCCESS);
  }

  //padre aspetta che finiscano i processi figli
  SYSC(rv, waitpid(cp1, NULL, 0), "nella waitpid");
  SYSC(rv, waitpid(cp2, NULL, 0), "nella waitpid");
  SYSC(rv, waitpid(cp3, NULL, 0), "nella waitpid");

  //chiudo i semafori e la memoria
  // SYSC(rv, munmap(shared_memory, (2 * sizeof(int))), "nella munmap");
  sem_destroy(&sem12);
  sem_destroy(&sem23);
  sem_destroy(&sem21);
  sem_destroy(&sem32);
}