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


//padre crea due figli A e B
//padre passa 15 num al figlio a con pipe1
//wait 2 sec
//figlio A passa a B con pipe2
//figlio b stampa il numero
//quando padre manda -1 termino tutto

int main(int argc, char * ARGV[]){
    int rv;
    int pfd1[2], pfd2[2];
    SYSC(rv , pipe(pfd1), "nella pipe1");
    SYSC(rv , pipe(pfd2), "nella pipe2");

    pid_t a , b;
    //figlio a
    SYSC(a , fork(), "nella fork a");
    if(a == 0){
        SYSC(rv, close(pfd1[1]), "nella close");
        SYSC(rv, close(pfd2[0]), "nella close");
        while(1){
            int n;
            SYSC(rv, read(pfd1[0], &n, sizeof(int)), "nella read");
            SYSC(rv, write(pfd2[1], &n, sizeof(int)), "nella write a");
            if(n == -1){
                SYSC(rv, close(pfd1[0]), "nella close");
                SYSC(rv, close(pfd2[1]), "nella close");
                exit(EXIT_SUCCESS);
            }
        }
    }
    //figlio b
    SYSC(b , fork(), "nella fork b");
    if(b == 0){
        SYSC(rv, close(pfd2[1]), "nella close");
        while(1){
            int n;
            SYSC(rv, read(pfd2[0], &n, sizeof(int)), "nella read");
            printf("Figlio 2 riceve: %d\n", n);
            // SYSC(rv, write(STDOUT_FILENO, &n, sizeof(int)), "nella write b");
            if(n == -1){
                SYSC(rv, close(pfd2[0]), "nella close");
                exit(EXIT_SUCCESS);
            }
        }
    }
    //padre
    SYSC(rv, close(pfd1[0]), "nella close");
    int n;
    for(int i = 0 ; i < 16 ; i++){
        n = i;
        SYSC(rv, write(pfd1[1], &n , sizeof(int)), "nella write padre");
        // sleep(2);
    }
    n = -1 ; 
    SYSC(rv, write(pfd1[1], &n , sizeof(int)), "nella write padre");
    //chiudo tutte le pipe aperte nel processo padre
    SYSC(rv, close(pfd1[1]), "nella close2");
    SYSC(rv, close(pfd2[0]), "nella close3");
    SYSC(rv, close(pfd2[1]), "nella close4");

    exit(EXIT_SUCCESS);
}
