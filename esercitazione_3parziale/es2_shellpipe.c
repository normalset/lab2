/*
Esercizio 2:
Utilizzando le chiamate POSIX scrivi un programma che riceve dalla shell linee di testo che passa ad un processo figlio tramite una pipe. I programmi terminano quando l'utente passa alla shell una linea vuota.
*/

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

#include <string.h>

#define BUF_SIZE 1024

int main()
{
  int rv;
  int pfd[2];
  SYSC(rv, pipe(pfd), "nella pipe");

  pid_t pid;
  SYSC(pid, fork(), "nella fork");
  if (pid)
  {
    // padre
    SYSC(rv, close(pfd[0]), "nella close pfd[0]");
    char cmd[BUF_SIZE];
    while (1)
    {
      SYSC(rv, read(STDIN_FILENO, cmd, BUF_SIZE), "nella read");
      SYSC(rv, write(pfd[1], cmd, strlen(cmd)-1), "nella write pfd[1]");
      memset(cmd, 0, BUF_SIZE);
      if (rv == 0 | cmd[0] == '\n')
      {
        SYSC(rv, close(pfd[1]), "nella close pfd[1]");
        exit(EXIT_SUCCESS);
      }
    }
  }
  else
  {
    // figlio
    SYSC(rv, close(pfd[1]), "nella close pfd[1]");
    char msg[BUF_SIZE];
    while (1)
    {
      SYSC(rv, read(pfd[0], msg, BUF_SIZE), "nella read");

      if (rv == 0 || msg[0] == 0)
      {
        SYSC(rv, close(pfd[0]), "nella close pfd[0]");
        exit(EXIT_SUCCESS);
      }

      printf("> %s\n", msg);
      memset(msg, 0, BUF_SIZE);
    }
  }
}