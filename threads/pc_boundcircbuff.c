#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


#define N_MESSAGES 20

#define BUFFER_SIZE 10
#define N_PRODUCERS 4
#define N_CONSUMERS 4


//mutex and cond
pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

//bounded buffer, producer legge dalla head, consumer aggiunge alla tail
int head = 0 , tail = 0 ;
int buff[BUFFER_SIZE];
int global_sum = 0;

void * producer(){
  for(int i = 0 ; i < N_MESSAGES ; i++){
    int n = rand() % 10;
    pthread_mutex_lock(&mux);

    // wait for buffer to empty out
    while ((head + 1) % BUFFER_SIZE == tail)
    {
      pthread_cond_wait(&not_full, &mux);
    }
    buff[head] = n;
    printf("Wrote : %d\n", buff[head]);
    head = (head + 1) % BUFFER_SIZE;

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mux);
  }
  return NULL;
}

void * consumer(){
  int personal_sum = 0 ;
  for(int i = 0 ; i < N_MESSAGES; i++){
    pthread_mutex_lock(&mux);

    // aspetta fino a quando il buffer non e' pieno
    while(head == tail){
      pthread_cond_wait(&not_empty , &mux);
    }
    printf("Read : %d\n", buff[tail]);
    personal_sum += (buff[tail] % 2 == 0 ? buff[tail] : 0);
    tail = (tail + 1) % BUFFER_SIZE;

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mux);
  }
  global_sum += personal_sum ; 
  return NULL;
}

int main(){
  int rv;
  pthread_t producers[N_PRODUCERS];
  pthread_t consumers[N_CONSUMERS];

  //creo i producers
  for (int i = 0; i < N_CONSUMERS; ++i){
    if((rv = pthread_create(&producers[i], NULL, &producer, NULL)) != 0){
      perror("nella pthread create");
      return 1;
    }
  }

  // creo i consumers 
  for(int i = 0; i < N_CONSUMERS; ++i){
    if ((rv = pthread_create(&consumers[i], NULL, &consumer, NULL)) != 0)
    {
      perror("nella pthread create");
      return 1;
    }
  }

  //joino tutto
  for (int i = 0; i < N_PRODUCERS; ++i){
    pthread_join(producers[i] , NULL);
  }
  for (int i = 0; i < N_CONSUMERS; ++i){
    pthread_join(consumers[i] , NULL);
  }

  printf("Global summ : %d\n", global_sum);

  return 0;
}
