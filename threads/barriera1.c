#define _XOPEN_SOURCE 600 /* Richiesta per fare andare la barrier for some reason*/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


/*
Scrivere un programma che crea N threds, N parametro dalla shell
Ogni thread stampa il suo ID poi aspetta che tutti gli altri finiscano di aspettare(barriera)
Termina dopo aver aspettato un numero di secondi aleatorio

Implementare la barriera con pthread_barrier_wait

La barriera fa aspettare tutti i thread che fanno la wait fino a quando non viene raggiunto un numero di thread in attesa settato alla dichiarazione della barriera 
*/

pthread_barrier_t barrier ; 

void *f_thread(void *arg)
{
  printf("Sono il thread : %ld\n" , pthread_self());
  pthread_barrier_wait(&barrier);
  sleep(rand() % 5);
  printf("Thread (%ld) in uscita\n", pthread_self());
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  if(argc != 2){
    perror("wrong args");
    return 1;
  }

  int num_threads = atoi(argv[1]);
  pthread_barrier_init(&barrier, NULL, num_threads);

  pthread_t tids[10];
  for(int i = 0 ; i < num_threads ; i++){
    pthread_create(&tids[i] , NULL , &f_thread , NULL); //qua do l'indirizzo dei tids
  }
  for(int i = 0 ; i < num_threads; i++){
    pthread_join(tids[i], NULL); //qua uso i tids salvati
  }
  printf("Main thread stopped waiting\n");

  pthread_barrier_destroy(&barrier);
  return 0;
}