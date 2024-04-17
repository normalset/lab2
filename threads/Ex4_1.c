#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 10
#define N_PRODUCERS 4
#define N_CONSUMERS 4
#define N_MESSAGES 20

// Circular buffer
char buffer[BUFFER_SIZE][256];
int head = 0, tail = 0;

// Mutex and condition variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

// Function prototypes
void *producer(void *arg);
void *consumer(void *arg);

int main() {
    pthread_t producers[N_PRODUCERS], consumers[N_CONSUMERS];
    int i, retvalue;

    // Seed the random number generator
    srand(42);

    // Create producer threads
    for (i = 0; i < N_PRODUCERS; ++i) {
        if ((retvalue = pthread_create(&producers[i], NULL, producer, NULL)) != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(retvalue));
            exit(EXIT_FAILURE);
        }
    }

    // Create consumer threads
    for (i = 0; i < N_CONSUMERS; ++i) {
        if ((retvalue = pthread_create(&consumers[i], NULL, consumer, NULL)) != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(retvalue));
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to terminate
    for (i = 0; i < N_PRODUCERS; ++i) {
        if ((retvalue = pthread_join(producers[i], NULL)) != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(retvalue));
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < N_CONSUMERS; ++i) {
        if ((retvalue = pthread_join(consumers[i], NULL)) != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(retvalue));
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void *producer(void *arg) {
    int i;
    char message[256];

    for (i = 0; i < N_MESSAGES; ++i) {
        // Generate a random message
        sprintf(message, "Message %d from producer %ld\n", i, pthread_self());

        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait until the buffer is not full
        while ((head + 1) % BUFFER_SIZE == tail) {
            pthread_cond_wait(&not_full, &mutex);
        }

        // Write the message in the buffer
        strcpy(buffer[head], message);
        head = (head + 1) % BUFFER_SIZE;

        // Signal that the buffer is not empty
        pthread_cond_signal(&not_empty);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Sleep for a random time
        usleep((rand() % 100) * 1000);
    }

    return NULL;
}

void *consumer(void *arg) {
    int i;
    char message[256];

    for (i = 0; i < N_MESSAGES; ++i) {
        // Lock the mutex
        pthread_mutex_lock(&mutex);

        // Wait until the buffer is not empty
        while (head == tail) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        // Read the message from the buffer
        strcpy(message, buffer[tail]);
        tail = (tail + 1) % BUFFER_SIZE;

        // Signal that the buffer is not full
        pthread_cond_signal(&not_full);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

        // Print the message
        printf("%s", message);

        // Sleep for a random time
        usleep((rand() % 100) * 1000);
    }

    return NULL;
}