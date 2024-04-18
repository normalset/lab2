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

#define BUFFER_SIZE 1024

void server(int PORT , int n_iter){
    int server_fd , client_fd , rv ;
    struct sockaddr_in server_addr , client_addr ;  
    socklen_t client_addr_len ; 
    char buffer[BUFFER_SIZE];

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
    SYSC(client_fd, accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len), "nella accept");

    //processing , ora il messaggio e' in client_fd
    for(int i = 0 ; i < n_iter; i++){
        ssize_t n_read;
        SYSC(n_read , read(client_fd , buffer, BUFFER_SIZE), "nella read");
        printf("Server ha ricevuto : %s\n", buffer);
        SYSC(rv , write(client_fd , buffer , n_read), "nella write");
    }

    //closing
    SYSC(rv , close(server_fd), "nella close server");
    SYSC(rv , close(client_fd), "nella close client");
}

void client(int PORT, int n_iter, char* word){
    int client_fd , rv ; 
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    //creazione del socket
    SYSC(client_fd , socket(AF_INET , SOCK_STREAM , 0), "nella socket");

    //inizializzazione struttura server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); 
    server_addr.sin_addr.s_addr = INADDR_ANY ; 

    //connect
    SYSC(rv , connect(client_fd , (struct sockaddr *) &server_addr , sizeof(server_addr)), "nella connect");

    //write message
    SYSC(rv, write(client_fd, word, strlen(word)), "nella write");
    for(int i = 0 ; i < n_iter-1 ; i++){
        ssize_t n_read;
        SYSC(n_read , read(client_fd , buffer, BUFFER_SIZE), "nella read");
        SYSC(rv , write(client_fd , buffer , n_read), "nella write");
    }

    //close socket
    SYSC(rv, close(client_fd), "nella close");
}

int main(int argc, char * ARGV[]){
    if(argc != 4){
        perror("arguments sbagliati");
        exit(EXIT_FAILURE);
    }
    
    int port = atoi(ARGV[1]);
    char word[1024] ;
    strcpy(word , ARGV[2]);
    int n_iter = atoi(ARGV[3]);
    
    pid_t pid;
    SYSC(pid, fork() , "nella fork");

    if(pid == 0){
        //figlio
        client(port, n_iter, word); 
    } else {
        //padre
        server(port, n_iter);
    }
    exit(EXIT_SUCCESS);
}
