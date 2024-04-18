#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

int NUM_MSG ;
int BUFFER_SIZE ; 
int * buffer ; 
int NUM_PROD ; 
int head = 0;
int tail = 0 ;

//mutex and cond
pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void * producer(void* arg){
    for(int i = 0 ; i < NUM_MSG ; i++){
        int n = rand();
         pthread_mutex_lock(&mux);

        // wait for buffer to empty out
        while ((head + 1) % BUFFER_SIZE == tail)
        {
            pthread_cond_wait(&not_full, &mux);
        }
            buffer[head] = n;
            // printf("Wrote : %d\n", buffer[head]);
            head = (head + 1) % BUFFER_SIZE;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mux);
    }
    return NULL;
}

void * consumer(void * arg){
    pthread_mutex_lock(&mux);

    // aspetta fino a quando il buffer non ha almeno un elemento
    while(head == tail){
        pthread_cond_wait(&not_empty , &mux);
    }
    printf("Read : %d\n", buffer[tail]);
    tail = (tail + 1) % BUFFER_SIZE;

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mux);

    return NULL;
}

int main(int argc, char* argv[]){
    if(argc != 6){
        perror("args sbagliati");
        exit(EXIT_FAILURE);
    }
    
    int rv;
    NUM_MSG = atoi(argv[1]);
    BUFFER_SIZE = atoi(argv[2]);
    int n_cons = atoi(argv[3]);
    int n_prods = atoi(argv[4]);
    int seed = atoi(argv[5]);
    srand(seed);

    //define buffer
    buffer = (int*)calloc(BUFFER_SIZE , sizeof(int));

    //creo i thread
    pthread_t producers[n_prods];
    pthread_t consumers[n_cons];

    //creo i producers
    for (int i = 0; i < n_prods; ++i){
        if((rv = pthread_create(&producers[i], NULL, &producer, NULL)) != 0){
            perror("nella pthread create");
            return 1;
        }
    }

    // creo i consumers 
    for(int i = 0; i < n_cons; ++i){
        if ((rv = pthread_create(&consumers[i], NULL, &consumer, NULL)) != 0)
        {
            perror("nella pthread create");
            return 1;
        }
    }

    //joino tutto
    for (int i = 0; i < n_prods; ++i){
        pthread_join(producers[i] , NULL);
    }
    for (int i = 0; i < n_cons; ++i){
        pthread_join(consumers[i] , NULL);
    }
    exit(EXIT_SUCCESS);
}
