// Il processo padre crea un figlio e passa ad esso la seguente stringa usando una pipe
// const char * data_to_send = "ciao, caro processo figlio!" ;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "macro.h"
#include <errno.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fd[2], retvalue;
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Creazione della pipe
    SYSC(retvalue, pipe(pipe_fd), "nella pipe");

    // Creazione di un nuovo processo figlio
    SYSC(pid, fork(), "nella fork");

    if (pid == 0) { // Codice del processo figlio

        // Chiude il lato di scrittura della pipe
        SYSC(retvalue, close(pfd[1]) , "nella close);

        // Legge dalla pipe e stampa i dati ricevuti
        ssize_t n_read, n_written; // = read(pipe_fd[0], buffer, BUFFER_SIZE);
        SYSC(n_read, read(pipe_fd[0], buffer, BUFFER_SIZE), "nella read");
        SYSC(n_written, write(STDOUT_FILENO, buffer, n_read), "nella write");

        // Chiude il lato di lettura della pipe
        SYSC(retvalue, close(pipe_fd[0]), "nella close");
        exit(EXIT_SUCCESS);

    } else { // Codice del processo padre

        // Chiude il lato di lettura della pipe
        SYSC(retvalue, close(pipe_fd[0]), "nella close");

        // Scrive dati nella pipe
        ssize_t n_written;
        const char *data_to_send = "Ciao, caro processo figlio!";
        SYSC(n_written, write(pipe_fd[1], data_to_send, strlen(data_to_send)), "nella write");

        // Chiude il lato di scrittura della pipe nel processo padre
        SYSC(retvalue, close(pipe_fd[1]), "nella close");

        pid_t pid_c;
        // Attendi che il processo figlio termini
        SYSC(pid_c, waitpid(pid, NULL, 0), "nella waitpid");
    }

    return 0;
}
