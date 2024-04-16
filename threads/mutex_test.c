#include <pthread.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void Pthread_mutex_lock( pthread_mutex_t *mux){
  int err ; 
  if( (err = pthread_mutex_lock(mux)) != 0){
    //gestione errore
    errno = err ; 
    perror("lock");
    // pthread_exit((void*) errno);
  } else
    printf("locked  ");
}

int Pthread_mutex_unlock(pthread_mutex_t *mux){
  int err;
  if ((err = pthread_mutex_unlock(mux)) != 0){
    // gestione errore
    errno = err;
    perror("lock");
    // pthread_exit((void*) errno);
  }
  else
    printf("unlocked  ");
}

//accessi controllati ad una variabile condivisa 
static pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER ;
static int x ; 

static void * t_fun(void * args){
  printf("%s", "test") ; 
  while(1){
    Pthread_mutex_lock(&mux);
    printf("Secondo thread: %d\n" , ++x);
    Pthread_mutex_unlock(&mux) ; 
    sleep(1) ; 
  }
}

int main(){
  pthread_t tid ; 
  int err ; 
  if( err = pthread_create(&tid, NULL , &t_fun , NULL) != 0){
    exit(EXIT_FAILURE);
  } else {
    while(1){
      Pthread_mutex_lock(&mux);
      printf("Primo thread: %d\n", ++x);
      Pthread_mutex_unlock(&mux);
      sleep(1);
    }
  }
}