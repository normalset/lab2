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

void server() {
    int server_fd, client_fd, retvalue;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[BUFFER_SIZE];

    // Creazione del socket
    SYSC(server_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");

    // Inizializzazione della struttura server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding
    SYSC(retvalue, bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "nella bind");

    // Listen
    SYSC(retvalue, listen(server_fd, 10), "nella listen");

    // Accept
    client_addr_len = sizeof(client_addr);
    SYSC(client_fd, accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len), "nella accept");

    // Ricezione del messaggio
    ssize_t n_read;
    SYSC(n_read, read(client_fd, buffer, BUFFER_SIZE), "nella read");

    // Stampa del messaggio
    SYSC(retvalue, write(STDOUT_FILENO, buffer, n_read), "nella write");

    // Chiusura del socket
    SYSC(retvalue, close(client_fd), "nella close");
    SYSC(retvalue, close(server_fd), "nella close");
}

void client() {
    int client_fd, retvalue;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Creazione del socket
    SYSC(client_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");

    // Inizializzazione della struttura server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_LOOPBACK;

    // Connect
    SYSC(retvalue, connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "nella connect");

    // Invio del messaggio
    const char *message = "Hello, server!";
    SYSC(retvalue, write(client_fd, message, strlen(message)), "nella write");

    // Chiusura del socket
    SYSC(retvalue, close(client_fd), "nella close");

}

int main() {
    pid_t pid;
    int retvalue;

    // Creazione del processo figlio
    SYSC(pid, fork(), "nella fork");

    if (pid == 0) {
        // Codice del processo figlio
        client();
    } else {
        // Codice del processo padre
        server();
        SYSC(retvalue, wait(NULL), "nella wait");
    }

    return 0;
}
