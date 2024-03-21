#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t barrier;
sem_t barrier_sem;
int counter = 0;

void *thread_fun(void *arg)
{
  int num_thread = (int)(long)arg;

  // Stampa il tid del thread
  printf("Thread TID = %ld\n", (long)pthread_self());

  // Acquisisce il mutex per aggiornare la variabile count in modo sicuro
  pthread_mutex_lock(&barrier);
  counter++;

  // Se non tutti i thread hanno raggiunto la barriera rilascia il mutex e attende
  if (counter < num_thread)
  {
    pthread_mutex_unlock(&barrier);
    sem_wait(&barrier_sem); // Aspetto che il semaforo venga svegliato
  }
  else
  {
    // Se tutti i thread hanno raggiunto la barriera sblocco tutti i semafori
    for (int i = 0; i < num_thread; i++)
    {
      sem_post(&barrier_sem);
    }
    pthread_mutex_unlock(&barrier);
  }
  sleep(rand() % 5);

  printf("Thread TID : %ld finished\n", (long)pthread_self());
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    perror("argomenti sbagliati");
    exit(EXIT_FAILURE);
  }
  int num_threads = atoi(argv[1]);

  pthread_t threads[num_threads];

  // Inizializzo mutex e il semaforo
  pthread_mutex_init(&barrier, NULL);
  sem_init(&barrier_sem, 0, 0);

  // creo i threads
  int rv;
  for (int i = 0; i < num_threads; i++)
  {
    rv = pthread_create(&threads[i], NULL, thread_fun, (void *)(long)num_threads);
    if (rv != 0)
    {
      perror("nella pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  // Attende la terminazione dei thread
  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  return 0;
}