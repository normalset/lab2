#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"

#define BUFFER_SIZE 1024 

typedef struct{
  char buffer[BUFFER_SIZE];
  int read_index;
  int write_index;
}SharedData;


int main(){
  
  int  shm_fd;
  SharedData *shared_data;
  sem_t *mutex, *empty, *full;

  return 0;
}