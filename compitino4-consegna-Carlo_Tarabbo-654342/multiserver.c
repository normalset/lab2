#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>
#include "macro.h"
#include <semaphore.h>

#define NOTZERO(r,c,m)\
  if((r=c) != 0) {perror(m) ; exit(EXIT_FAILURE); }

#define BUFFER_SIZE 1024

void* client_handler(void* arg){
    ssize_t n_read ; 
    //processing , ora il messaggio e' in client_fd, che devo estrarre da arg
    int client_fd = *((int*)arg);
    char buffer[BUFFER_SIZE] ;  

    //leggo , sleep(2) e stampo
    SYSC(n_read , read(client_fd , buffer, BUFFER_SIZE), "nella read");
    sleep(2);
    printf("Ricevuto messaggio : %s\n", buffer); 

    return NULL;
}

void server(int PORT){
    int server_fd , client_fd , rv ;
    struct sockaddr_in server_addr , client_addr ;  
    socklen_t client_addr_len ; 

    //creazione del socket
    SYSC(server_fd , socket(AF_INET , SOCK_STREAM , 0), "nella socket");

    //inizializzazione della struttura server_addr
    server_addr.sin_family = AF_INET ;
    server_addr.sin_port = htons(PORT) ;
    server_addr.sin_addr.s_addr = INADDR_ANY ; //accetta ogni messaggio in arrivo

    //binding
    SYSC(rv , bind(server_fd , (struct sockaddr *) &server_addr, sizeof(server_addr)), "nella bind");

    //listening
    SYSC(rv, listen(server_fd, 10), "nella listen");

    //accepting
    client_addr_len = sizeof(client_addr);

    // loop di ascolto del server
    while(1){

        SYSC(client_fd, accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len), "nella accept");     

        printf("New client connected %d!\n", client_fd);

        //creo un nuovo thread per il client
        pthread_t handler_tid ; 
        if( pthread_create(&handler_tid , NULL , &client_handler, (void*)&client_fd) != 0){
            perror("nella pthread create");
            close(client_fd);
            continue;
        }
        printf("Created thread\n");
        NOTZERO(rv , pthread_join(handler_tid, NULL), "nella join");

    }

    //closing server, i client sono chiusi dal thread che li gestisce 
    SYSC(rv , close(server_fd), "nella close server");

}

void client(int PORT){
    for(int r = 0 ; r < 20 ; r++){
        int client_fd , rv ; 
        struct sockaddr_in server_addr;

        // creazione del socket
        SYSC(client_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");

        // inizializzazione struttura server_addr
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        // connect
        SYSC(rv, connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)), "nella connect");

        // write message
        char message[256] = "HELLO !\n"; 
        SYSC(rv, write(client_fd, message, strlen(message)), "nella write");

        // close socket
        SYSC(rv, close(client_fd), "nella close");

        sleep(1);
    }
}

int main(int argc, char * ARGV[]){
    if(argc != 2){
        perror("args sbagliati");
        exit(EXIT_FAILURE);
    }

    int port = atoi(ARGV[1]);

    pid_t pid ; 
    int rv ; 

    SYSC(pid, fork(), "nella fork") ;

    if(pid == 0){
        //figlio
        client(port);
    } else {
        //Codice del processo padre
        server(port);
        SYSC(rv , wait(NULL) , "nella wait");
    }
    exit(EXIT_SUCCESS) ; 
}

