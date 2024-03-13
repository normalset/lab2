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

int main()
{
  // creo la shared memory e la dimensiono con ftruncate
  int sm, rv;
  SYSC(rv, shm_unlink("/my_shared_memory"), "nella shm_unlink");

  
  SYSC(sm, shm_open("/my_shared_memory", O_CREAT | O_RDWR | O_EXCL, 0666), "nella open");
  SYSC(rv, ftruncate(sm, 2 * sizeof(int)), "nella ftruncate");

  int *shared_memory;
  SYSCN(shared_memory,
        mmap(NULL, (2 * sizeof(int)), PROT_READ | PROT_WRITE | MAP_SHARED, 0, sm, 0), "nella mmap")
  SYSC(rv, close(sm), "nella close");

  // casto la shared memory

  // creo i due semafori
  sem_t sem12;
  sem_t sem23;
  sem_init(&sem12, 1, 0);
  sem_init(&sem23, 1, 0);

  pid_t cp1, cp2, cp3;

  // primo figlio
  SYSC(cp1, fork(), "nella fork1");
  if (cp1 == 0)
  {
    srand(time(NULL));
    int n = rand() % 9 + 1;
    shared_memory[0] = n;
    sem_post(&sem12);
    printf("figlio 1, generato e memorizzato il numero %d\n", n);
    sem_wait(&sem23);
    printf("figlio 1, finito di aspettare il figlio 3\n");
    exit(EXIT_SUCCESS);
  }

  // secondo figlio
  SYSC(cp2, fork(), "nella fork2");
  if (cp2 == 0)
  {
    sem_wait(&sem12);
    shared_memory[1] = shared_memory[0];
    printf("F2, letto il numero %d dal figlio 1\n", shared_memory[0]);
    sem_post(&sem23);
    printf("F2, scritto il numero %d nel secondo spazio di memoria\n", shared_memory[1]);
    exit(EXIT_SUCCESS);
  }

  // terzo figlio
  SYSC(cp3, fork(), "nella fork3");
  if (cp3 == 0)
  {
    sem_wait(&sem23);
    printf("F3, letto il numero %d dal figlio 2\n", shared_memory[1]);
    exit(EXIT_SUCCESS);
  }

  // padre aspetta che finiscano i processi figli
  SYSC(rv, waitpid(cp1, NULL, 0), "nella waitpid");
  SYSC(rv, waitpid(cp2, NULL, 0), "nella waitpid");
  SYSC(rv, waitpid(cp3, NULL, 0), "nella waitpid");

  // Unlink the shared memory object
  SYSC(rv, shm_unlink("/my_shared_memory"), "nella shm_unlink");

  // chiudo i semafori e la memoria
  SYSC(rv, munmap(shared_memory, (2 * sizeof(int))), "nella munmap");
  sem_destroy(&sem12);
  sem_destroy(&sem23);
}
