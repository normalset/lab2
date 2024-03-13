// Scrivere un programma che esegue ls -la nel processo figlio e tramite pipe e redirezione (dup2) fa scrivere l'output dal padre

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macro.h"
#define BUFFER_SIZE 1024

int main(int argc , char* argv[]){
  //! Ricordare di aprire la pipe prima della fork()
  int pipe_fd[2], retvalue;
  SYSC(retvalue, pipe(pipe_fd), "nella pipe");
  
  pid_t pid;
  SYSC(pid, fork() , "nella fork");


  if(pid){ //se != 0 padre 
    //* processo padre
    char buffer[BUFFER_SIZE];
    //chiudo la pipe in scrittura
    SYSC(retvalue, close(pipe_fd[1]), "nella close");
    //leggo dalla pipe e stampo i dati
    ssize_t n_read;
    SYSC(n_read, read(pipe_fd[0], buffer, BUFFER_SIZE), "nella read");
    SYSC(retvalue, write(STDOUT_FILENO, buffer, n_read), "nella write");

    //chiudo e esco
    SYSC(retvalue, close(pipe_fd[0]), "nella close");
    //aspetta che il figlio termini
    SYSC(retvalue, waitpid(pid,NULL,0), "nella waitpid");
  } else {
    //* processo figlio
    // Chiude il lato di lettura della pipe
    SYSC(retvalue, close(pipe_fd[0]), "nella close");
    //cambio l'output del processo con dup2 dallo stduot a pipe_fd[1] (sezione di scrittura)
    SYSC(retvalue, dup2(pipe_fd[1], STDOUT_FILENO), "nella dup2");

    //processo figlio
    char* args[] = {"ls", "-la", NULL};
    execvp("ls" , args);

    //se la execvp fallisce allora viene eseguito questa parte
    perror("nella execvp");
    // exit(EXIT_FAILURE);
    return 1;
  }



  return 0;
}
