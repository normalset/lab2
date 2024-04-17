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
#include <errno.h> //for strerror fun

pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;

int buff = 0;

void *producer(void *arg)
{
  for (int i = 0; i < 10; ++i)
  {
    //get lock
    pthread_mutex_lock(&mux);
    // wait until the buffer is empty
    while (buff != 0)
    {
      pthread_cond_wait(&notfull, &mux);
    }
    // copy the message into the buffer
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    // signal the buffer is not empty
    pthread_cond_signal(&notempty);
    // unlock mutex
    pthread_mutex_unlock(&mux);
  }
  printf("Finito il producer\n");
}

void *consumer(void *arg)
{
  //def variables and counters if needed
  while (1)
  {
    // lock the mutex
    pthread_mutex_lock(&mux);

    // wait until the buff is NOT empty
    while (buff == 0)
    {
      pthread_cond_wait(&notempty, &mux);
    }
    // read the message from the buffer and process it
    >>>>>>>>>>>>>>>>>>>>>>>>

    // signal that the buffer is not full and unlock the mux
    pthread_cond_signal(&notfull);
    pthread_mutex_unlock(&mux);
  }
  return NULL ; 
}

int main(int argc, char *argv[])
{
  // creo i due threads
  int rv;
  pthread_t p, c;
  if ((rv = pthread_create(&p, NULL, &producer, NULL)) != 0)
  {
    fprintf(stderr, "pthread_create : %s\n", strerror(rv));
    return 1;
  }

  if ((rv = pthread_create(&c, NULL, &consumer, NULL)) != 0)
  {
    fprintf(stderr, "pthread_create : %s\n", strerror(rv));
    return 1;
  }

  pthread_join(p, NULL);
  pthread_join(c, NULL);

  pthread_cond_destroy(&notempty);
  pthread_cond_destroy(&notfull);
  pthread_mutex_destroy(&mux);
  return 0;
}