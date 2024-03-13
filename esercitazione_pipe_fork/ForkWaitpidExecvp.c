//questa volta il figlio esegue la chiamata ad execvp (o la exec che preferite) invocando il comando ls -l

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
    //aspetto che il figlio finisca
    int status ;
    pid_t child_pid;
    SYSC(child_pid, wait(&status), "nella wait");

    if(WIFEXITED(status)){
      printf("Il processo figlio è terminato con codice di uscita: %d\n", WEXITSTATUS(status));
    } else {
      printf("Il processo figlio non è terminato correttamente\n");
    }
  } else { //sono il figlio

    char* args[] = {"ls", "-l", NULL};
    execvp("ls" , args);

    //se la execvp fallisce allora viene eseguito questa parte
    perror("nella execvp");
    // exit(EXIT_FAILURE);
    return 1;
  }
  return 0;
}
