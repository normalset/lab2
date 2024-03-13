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

#define BUFFER_SIZE 10
#define SHARED_MEM_NAME "/shared_memory"
#define SEM_MUTEX_NAME "/sem_mutex"
#define SEM_EMPTY_NAME "/sem_empty"
#define SEM_FULL_NAME "/sem_full"

typedef struct {
    char buffer[BUFFER_SIZE];
    int read_index;
    int write_index;
} SharedData;

void reader(sem_t *sem_mutex, sem_t *sem_empty, sem_t *sem_full, SharedData *shared_data) {
    char data;

    while (1) {
        // Attendi che ci siano dati disponibili nel buffer
        sem_wait(sem_full);
        // Proteggi l'accesso al buffer
        sem_wait(sem_mutex);

        // Leggi dal buffer
        data = shared_data->buffer[shared_data->read_index];
        shared_data->read_index = (shared_data->read_index + 1) % BUFFER_SIZE;

        // Rilascia la protezione del buffer
        sem_post(sem_mutex);
        // Incrementa il numero di spazi vuoti nel buffer
        sem_post(sem_empty);

        // Utilizza il dato letto (in questo caso, stampa a schermo)
        printf("Reader - Read: %c\n", data);

        // Simula un tempo di elaborazione
        usleep(100000);
    }
}

void writer(sem_t *sem_mutex, sem_t *sem_empty, sem_t *sem_full, SharedData *shared_data) {
    char data = 'A';

    while (1) {
        // Attendi che ci siano spazi vuoti nel buffer
        sem_wait(sem_empty);
        // Proteggi l'accesso al buffer
        sem_wait(sem_mutex);

        // Scrivi nel buffer
        shared_data->buffer[shared_data->write_index] = data;
        shared_data->write_index = (shared_data->write_index + 1) % BUFFER_SIZE;

        // Rilascia la protezione del buffer
        sem_post(sem_mutex);
        // Incrementa il numero di dati disponibili nel buffer
        sem_post(sem_full);

        // Utilizza il dato scritto (in questo caso, stampa a schermo)
        printf("Writer - Wrote: %c\n", data);

        // Incrementa il carattere da scrivere (esempio: A, B, C, ...)
        data = (data + 1) > 'Z' ? 'A' : (data + 1);

        // Simula un tempo di elaborazione
        usleep(150000);
    }
}

int main() {
    int shm_fd;
    sem_t *sem_mutex, *sem_empty, *sem_full;
    SharedData *shared_data;

    // Creazione e apertura della memoria condivisa
    SYSC(shm_fd, shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666), "shm_open");

    // Impostazione della dimensione della memoria condivisa
    ftruncate(shm_fd, sizeof(SharedData));

    // Mapping della memoria condivisa
    SYSCN(shared_data, (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0), "mmap");

    // Creazione e inizializzazione dei semafori
    SYSCN(sem_mutex, sem_open(SEM_MUTEX_NAME, O_CREAT | O_EXCL, 0666, 1), "sem_mutex: open");
    SYSCN(sem_empty, sem_open(SEM_EMPTY_NAME, O_CREAT | O_EXCL, 0666, BUFFER_SIZE), "sem_empty: open");
    SYSCN(sem_full, sem_open(SEM_FULL_NAME, O_CREAT | O_EXCL, 0666, 0), "sem_full: open");


    // Inizializzazione del buffer circolare
    shared_data->read_index = 0;
    shared_data->write_index = 0;

    // Creazione di processi lettori e scrittori (da implementare)
    // Creazione di processi lettori e scrittori
    if (fork() == 0) {
        // Processo figlio (lettore)
        reader(sem_mutex, sem_empty, sem_full, shared_data);
    } else {
        // Processo padre (scrittore)
        if (fork() == 0) {
            writer(sem_mutex, sem_empty, sem_full, shared_data);
        }
    }

    // Attesa dei processi figli
    wait(NULL);


    // Chiusura delle risorse
    munmap(shared_data, sizeof(SharedData));
    close(shm_fd);

    sem_close(sem_mutex);
    sem_close(sem_empty);
    sem_close(sem_full);

    sem_unlink(SEM_MUTEX_NAME);
    sem_unlink(SEM_EMPTY_NAME);
    sem_unlink(SEM_FULL_NAME);

    shm_unlink(SHARED_MEM_NAME);

    return 0;
}
