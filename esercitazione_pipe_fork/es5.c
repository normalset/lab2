//Scriviamo una shell semplificata che permetta l'esecuizone di programmi sia in background(terminando con il comando '&') che in foreground

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 1024

int main(){
  char input[MAX_INPUT_SIZE];
  char *args[MAX_ARGS];
  int background;

  while(1){

  }
  return 0;
}

void execute_command(char * args, int background){
  int retvalue;
  pid_t pid;
  SYSC(pid, fork(), "nella fork");

  if(pid == 0){
    execvp(args[0], args);
    //se fallisce
    perror("nella execvp");
    exit(EXIT_FAILURE);
  } else {
    //attende la terminazione del processo figlio solo se ho background=0
    if(background){
      waitpid(pid, NULL, 0);
    } else {
      //scrive a video il pid del processo figlio
      printf("Background process with pid %d\n", pid);
    }
  }
}
