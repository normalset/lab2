#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*
Scrivere un programma che crea N thread, N dalla linea di comando 

Ogni thread stampa il suo ID e termina dopo aver aspettare un numero di secondi casuale
*/

void* f_thread(void* arg){
  sleep(rand() % 5);
  printf("Sono il thread : %ld\n", pthread_self());
}

int main(int argc, char* argv[]){
  if(argc != 2){
    perror("Args sbagliati");
    return 1 ; 
  }

  for(int i = 0 ; i < atoi(argv[1]) ; i++ ){
    pthread_t tid ; 
    pthread_create(&tid , NULL , &f_thread , NULL);
    pthread_join(tid, NULL);
  }
  printf("Finito di aspettare tutti i thread, thread padre %ld\n", pthread_self());
  return 0 ; 
}