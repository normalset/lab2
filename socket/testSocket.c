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
#include "../macro.h"

#define PORT 2000
#define BUFFER_SIZE 1024

/* 
schema server:
   socket
   inizializzazione della struttura server_addr
   bind
   listen
   accept
   -- ricezione messaggio -- 
   read / write
   close sockets 
*/

/*
schema client:
   socket
   inizializzazione struttura server_addr
   connect
   --invio messaggio-- write
   close
*/

void server(){
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
   ssize_t n_read;
   SYSC(n_read , read(client_fd , buffer, BUFFER_SIZE), "nella read");
   SYSC(rv , write(STDOUT_FILENO , buffer , n_read), "nella write");
   
   //closing
   SYSC(rv , close(server_fd), "nella close server");
   SYSC(rv , close(client_fd), "nella close client");
}

void client(){
   int client_fd , rv ; 
   struct sockaddr_in server_addr;
   char buffer[BUFFER_SIZE];

   //creazione del socket
   SYSC(client_fd , socket(AF_INET , SOCK_STREAM , 0), "nella socket");

   //inizializzazione struttura server_addr
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT); 
   server_addr.sin_addr.s_addr = INADDR_ANY ; 
   //127.0.0.1 usato per comunicare sulla stessa macchina come indirizzo di loopback

   //connect
   SYSC(rv , connect(client_fd , (struct sockaddr *) &server_addr , sizeof(server_addr)), "nella connect");

   //write message
   const char* message = "Hello server!\n";
   SYSC(rv, write(client_fd, message, strlen(message)), "nella write");

   //close socket
   SYSC(rv, close(client_fd), "nella close");
}

int main(){
   pid_t pid ; 
   int rv ; 

   SYSC(pid, fork(), "nella fork") ;

   if(pid == 0){
      //figlio
      client();
   } else {
      //Codice del processo padre
      server();
      SYSC(rv , wait(NULL) , "nella wait");
   }
   return 0 ; 
}