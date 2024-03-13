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

#define BUF_SIZE 1024
#define MODE 0666

int main(int argc, char*argv[]){
  int src_fd, dst_fd, n, retvalue;
  char buffer[BUF_SIZE];
  mode_t mode = MODE;

  //controllo argv =4
  char mode_op = argv[1][0];
  SYSC(src_fd, open(argv[2], O_RDONLY), "nella open file sorgente");

  switch(mode_op){
    case 'e':
      SYSC(dst_fd, open(argv[3], O_WRONLY | O_EXCL | O_CREAT), "nella open"); //lo apro in modo esclusivo
      break ;
    case 'a':
      SYSC(dst_fd, open(argv[3], O_WRONLY | O_APPEND | O_CREAT), "nella open"); //lo apro in modo esclusivo
      break ;
    case 'w':
      SYSC(dst_fd, open(argv[3], O_WRONLY | O_TRUNC| O_CREAT), "nella open"); //lo apro in modo esclusivo
      break ;
    default:
    // errore , chiudo fd
  }

  while( (n = read(src_fd, buffer, BUF_SIZE)) > 0){
    SYSC(n, write(dst_fd, buffer, n), "nella write");
  }

  if(n < 0) {
    perror("errore di lettura");
  }
  close(src_fd);
  close(dst_fd);
  return 0;
}