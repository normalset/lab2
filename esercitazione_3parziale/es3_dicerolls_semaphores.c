/*
Esercizio 3:
Utilizzando le chiamate POSIX, scrivi un programma che genera 4 processi figlio, ogni figlio genera un numero casuale compreso tra 1 e 6 (il lancio di un dado), lo scrive su una variabile condivisa, dopo essersi sincronizzato su di un semaforo per acquisire la mutua esclusione sulla variabile condivisa, si assicura cha il padre abbia letto, e poi esce. Il processo padre legge, calcola il massimo risultato e lo scrive a video.
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

int main(){
  int rv;
  //creazione della memoria condivisa
  int sm_fd;
  SYSC(sm_fd , shm_open("/shared_mem", O_CREAT | O_RDWR | O_TRUNC, 0666), "nella shm_open");
  SYSC(rv, ftruncate(sm_fd, sizeof(int)), "nella ftruncate");

  int* shared_mem;
  SYSCN(shared_mem, mmap(NULL , sizeof(int), PROT_READ | PROT_WRITE , MAP_SHARED, sm_fd, 0), "nella mmap");

  //creazione del semaforo
  sem_t* sem;
  SYSCN(sem, sem_open("/my_semaphore", O_CREAT | O_EXCL, 0666, 1), "nella sem_open");
  sem_t *sem2;
  SYSCN(sem2, sem_open("/my_semaphore2", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");

  //fork per i
  pid_t pid;
  for(int i = 0 ; i < 4 ; i++){
    SYSC(pid, fork(), "nella fork");
    if(pid == 0){
      //setto il randomizer con il nuovo pid del processo
      srand(getpid());
      sem_wait(sem);
      int n = rand() % 6 + 1;
      printf("il n=%d\n", n);
      *shared_mem = n;
      printf("il figlio (%d : %d) ha lanciato un %d\n", i, getpid(), *shared_mem);
      sem_post(sem2);
      exit(EXIT_SUCCESS);
    }
  }
  //padre
  int max = 0;
  for(int i = 0 ; i < 4 ; i++){
    sem_wait(sem2);
    printf("Il padre ha ricevuto un: %d\n", *shared_mem);
    if(*shared_mem > max){
      max = *shared_mem ;
    }
    sem_post(sem);
  }

  printf("Il roll piu' alto e' stato: %d\n", max);

  //chiudo
  SYSC(rv, munmap(shared_mem, sizeof(int)), "nella munmap");
  shm_unlink("/shared_mem");
  sem_unlink("/my_semaphore");
  sem_unlink("/my_semaphore2");
  return 0;
}