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

//argv file N1 N2 N3 N4 N5
//padre genera 4 figli
// shared mem di tre numeri interi

//primo processo scrive i numeri nella prima locazione della shared mem
//rimane in attesa che il secondo processo lo abbia letto

//secondo legge dalla prima e scrive nella seconda
//aspetta che il terzo la legga

//terzo legge dalla seconda posizione e lo scrive nella terza
//aspetta che il 4 lo legga

//quarto legge il numero e lo stampa a video

int main(int argc, char * ARGV[]){
    if(argc != 6){
        perror(" argomenti sbagliati");
        exit(EXIT_FAILURE);
    }
    int rv;
    //creo la shared memory
    int sm_fd;
    SYSC(sm_fd , shm_open("/shared_mem", O_CREAT | O_RDWR | O_TRUNC, 0666), "nella shm_open");
    SYSC(rv, ftruncate(sm_fd, 3 * sizeof(int)), "nella ftruncate");

    int* shared_mem;
    SYSCN(shared_mem, mmap(NULL , 3 * sizeof(int), PROT_READ | PROT_WRITE , MAP_SHARED, sm_fd, 0), "nella mmap");

    //creo i semafori
    sem_t* done1;
    SYSCN(done1, sem_open("/done1", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");
    sem_t* read2;
    SYSCN(read2, sem_open("/read2", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");

    sem_t* done2;
    SYSCN(done2, sem_open("/done2", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");
    sem_t* read3;
    SYSCN(read3, sem_open("/read3", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");

    sem_t* done3;
    SYSCN(done3, sem_open("/done3", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");
    sem_t* read4;
    SYSCN(read4, sem_open("/read4", O_CREAT | O_EXCL, 0666, 0), "nella sem_open");

    pid_t p1 , p2, p3, p4;
    //processo 1
    SYSC(p1, fork(), "nella fork");
    if(p1 == 0){
        for(int i = 1 ; i < 6 ; i++){
            shared_mem[0] = atoi(ARGV[i]);
            sem_post(done1);
            sem_wait(read2);
        }
        exit(EXIT_SUCCESS);
    }
    //processo 2
    SYSC(p2, fork(), "nella fork");
    if(p2 == 0){
        for(int i = 1 ; i < 6 ; i++){
            sem_wait(done1);
            shared_mem[1] = shared_mem[0];
            sem_post(read2);
            sem_post(done2);
            sem_wait(read3);
        }
        exit(EXIT_SUCCESS);
    }
    //processo 3
    SYSC(p3, fork(), "nella fork");
    if(p3 == 0){
        for(int i = 0 ; i < 6 ; i++){
            sem_wait(done2);
            shared_mem[2] = shared_mem[1];
            sem_post(read3);
            sem_post(done3);
            sem_wait(read4);
        }   
        exit(EXIT_SUCCESS);
    }
    //processo 2
    SYSC(p4, fork(), "nella fork");
    if(p4 == 0){
        for(int i = 0 ; i < 6 ; i ++){
            sem_wait(done3);
            int n = shared_mem[2];
            printf("Processo 4 ha ricevuto: %d\n", n);
            //SYSC(rv, write(STDOUT_FILENO, &n , sizeof(int)), "nella write processo 4");
            sem_post(read4);
        }
        exit(EXIT_SUCCESS);
    }

    //chiudo e unlinko 
    shm_unlink("/shared_mem");
    sem_unlink("/read2");
    sem_unlink("/read3");
    sem_unlink("/read4");
    sem_unlink("/done1");
    sem_unlink("/done2");
    sem_unlink("/done3");
    SYSC(rv, munmap(shared_mem, 3 * sizeof(int)), "nella munmap");
    exit(EXIT_SUCCESS);
}
