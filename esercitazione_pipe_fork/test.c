#include <sys/wait.h>
#include <sys/mman.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "macro.h"

#define MEM_SIZE 4096

int main(int argc, char* argv[]){
  //controllo argomenti
  if(argc != 2){
    fprintf(stderr, "Usage: %s <shm_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int retvalue, mem_fd ;
  //creo la shared mem
  SYSC(retvalue, shm_open(argv[1], O_CREAT | O_RDWR, 0666), "nella shem_open");
  SYSC(retvalue, ftruncate(mem_fd, MEM_SIZE), "nella ftruncate");

  //creo un nuovo processo
  pid_t pid ;
  SYSC(pid , fork(), "nella fork()");
  if(pid == 0){
    //figlio
    //? mappo la shared memory all'adress space del processo
    void * ptr;
    SYSCN(ptr, mmap(
      NULL , MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED ,mem_fd, 0
    ), "nella mmap")
    
    //? scrivo nella shared memory
    const char *msg = "hello my shared memory!";
    SYSC(retvalue, sprintf(ptr, "%s", msg),"nella sprintf"); //sprintf formatta la stringa scrivendola in ptr

    //? unmappo la shared mem
    SYSC(retvalue, munmap(ptr, MEM_SIZE), "nella munmap()");
    exit(EXIT_SUCCESS);
  } else{
    //padre
    //? linko il padre alla shared memory
    void * ptr;
    SYSCN(ptr, mmap(
      NULL , MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED ,mem_fd, 0
    ), "nella mmap");

    //aspetto che finisca il figlio
    SYSC(retvalue, waitpid(pid, NULL, 0), "nella waitpid");
    SYSC(retvalue, write(STDOUT_FILENO, ptr, strlen(ptr)), "nella write");
    
    //?unmap e exit
    SYSC(retvalue, munmap(ptr, MEM_SIZE), "nella munmap");
    exit(EXIT_SUCCESS);
  }
}