#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
#include "macros.h"

#include <string.h>

#define BUF_SIZE 1024

// argv prog modo file1 file2 file_dest

int main(int argc, char * ARGV[]){

    if(argc != 5){
        perror(" argomenti sbagliati");
        exit(EXIT_FAILURE);
    }

    int rv;
//apri file 1 read only
    int file1;
    SYSC(file1, open(ARGV[2], O_RDONLY, 0666), "nella open");
//apri file 2 write only
    int file2;
    SYSC(file2, open(ARGV[3], O_RDONLY, 0666 ), "nella open2");
//check che file 1 e 2 siano grandi uguali con fstat se no errore
    struct stat stat1 , stat2;
    SYSC(rv, fstat(file1, &stat1), "nella fstat");
    SYSC(rv, fstat(file2, &stat2), "nella fstat");
    if(stat1.st_size != stat2.st_size){
        perror("File di dimensioni diverse");
        exit(EXIT_FAILURE);
    }

// check parametro modo
    int file_dest;
    if(strcmp(ARGV[1], "e")){
        if((rv = open("argc", O_WRONLY)) != -1){
            perror("dest esiste");
            exit(EXIT_FAILURE);
        }
        SYSC(file_dest, open(ARGV[4], O_CREAT | O_WRONLY | O_APPEND, 0666), "nella open dest");
    } else if(strcmp(ARGV[1] , "o")){
        SYSC(file_dest, open(ARGV[4], O_TRUNC| O_CREAT | O_WRONLY | O_APPEND, 0666), "nella open dest");
    }

    char buf1[BUF_SIZE], buf2[BUF_SIZE];
    while(rv != 0){ //se scrivo zero byte ho finito il file e esco dal loop
        SYSC(rv, read(file1, buf1, BUF_SIZE), "nella read1");
        SYSC(rv, read(file2, buf2, BUF_SIZE), "nella read1");
        for(int i = 0 ; i < rv ; i++){ 
            char c = buf1[i] ^ buf2[i];
            SYSC(rv, write(file_dest, &c , sizeof(char)), "nella write");
        }
    }
    exit(EXIT_SUCCESS);
}
