//il padre crea il processo figlio, il quale si limita a stampare a video il proprio PID
//il padre attende la terminazione del figlio, poi ne stampa a video il pid e lo stato di uscita
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"

int main(int argc , char* argv[]){
  pid_t pid1;
  //creo il nuovo processo figlio
  SYSC(pid1, fork() , "nella fork");

  if(pid1){ //se != 0 padre
    printf("sono pid padre: %d\n", getpid());

    //attende la terminazione del processo figlio
    int status ;
    pid_t child_pid;
    SYSC(child_pid, wait(&status), "nella wait");
    if(WIFEXITED(status)){
      printf("Il processo figlio con PID %d che avevo creato come %d è terminato con stato %d\n", child_pid, pid1, WEXITSTATUS(status));
    } else {
      printf("Il processo figlio con pid: %d non e' terminato correttamente\n" , child_pid);
    }
  } else { //sono il figlio
    printf("sono pid figlio: %d\n", getpid());
    exit(EXIT_SUCCESS);
  }
  return 0;
}
