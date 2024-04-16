#define _XOPEN_SOURCE 600 /* Richiesta per fare andare la barrier for some reason*/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include "../macro.h"

#include <sys/mman.h>
#include <fcntl.h>

/*
Programma che implementi un producer consumer con un buffer di una posizione
Creare un thread produttore e 1 thread consumer:
  il produttore produce interi positivi in modo random
  il consumer fa la somma degli interi PARI
  il produttore segnala al consumatore la fine della lista dei numeri producendo il numero -1
  il consumer stampa la somma ottenuta
*/

sem_t producer_done ; 
sem_t consumer_done ;

pthread_t c;

int nums[10] = {1,2,3,4,5,6,7,8,9,-1};
int buff = 0 ;
int somma;

void *producer(void *arg){
  for(int i = 0 ; i < 10 ; ++i){
    sem_wait(&consumer_done);
    buff = nums[i] ;
    sem_post(&producer_done);
  }
  printf("Finito il producer");
  pthread_cancel(c);
}

void *consumer_closing(void* arg)
{
  printf("Somma : %d\n", somma);
  pthread_exit((void *)0);
  return NULL;
}

void *consumer(void *arg){
  while(1){
    sem_wait(&producer_done); 
    somma = somma + (buff % 2 == 0 ? buff : 0 );
    buff = 0 ; 
    sem_post(&consumer_done);
  }
}

int main(int argc, char *argv[]){
  sem_init(&producer_done , 1 , 0);
  sem_init(&consumer_done , 1 , 1);
  //creo i due threads
  pthread_t p ; 
  pthread_create(&p,NULL, &producer, NULL);
  // pthread_t c; messo come globale
  pthread_create(&c, NULL, &consumer, NULL);

  pthread_cleanup_push(&consumer_closing, NULL);

  pthread_join(p, NULL);
  pthread_join(c, NULL);

  pthread_cleanup_pop(1) ; 
  sem_destroy(&producer_done);
  sem_destroy(&consumer_done);
  return 0 ; 
}