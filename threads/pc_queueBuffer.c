#define _XOPEN_SOURCE 600 /* Richiesta per fare andare la barrier for some reason*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


#define N_MESSAGES 20

#define BUFFER_SIZE 10
#define N_PRODUCERS 4
#define N_CONSUMERS 4

int global_sum;

//mutex and cond
pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

//queue buffer
typedef struct Node {
  int data;
  struct Node* next;
} Node;

typedef struct Queue {
  Node* head;
  Node* tail;
} Queue;

Queue* createQueue() {
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->head = queue->tail = NULL;
  return queue;
}

int isEmpty(Queue* queue) {
  return queue->head == NULL;
}

void enqueue(Queue* queue, int data) {
  // Create a new node
  Node* newNode = (Node*)malloc(sizeof(Node));
  newNode->data = data;
  newNode->next = NULL;

  // If queue is empty, set both head and rear to the new node
  if (isEmpty(queue)) {
    queue->head = queue->tail = newNode;
  } else {
    // Add the new node to the rear of the queue
    queue->tail->next = newNode;
    queue->tail = newNode;
  }
}


int dequeue(Queue* queue) {
  if (isEmpty(queue)) {
    printf("Queue is empty\n");
    return -1; // Or some specific value to indicate error
  }

  // Get the data from the head node
  int data = queue->head->data;

  // Remove the head node
  Node* temp = queue->head;
  queue->head = queue->head->next;

  // If queue becomes empty after dequeue, set both head and rear to NULL
  if (queue->head == NULL) {
    queue->tail = NULL;
  }

  // Free the memory of the removed node
  free(temp);

  return data;
}

void * producer() ;
void * consumer() ;

// creo la queue
Queue *q ;

int main(){
  q = createQueue();

  //threads
  int rv;
  pthread_t producers[N_PRODUCERS];
  pthread_t consumers[N_CONSUMERS];

  //creo i producers
  for (int i = 0; i < N_CONSUMERS; ++i){
    if((rv = pthread_create(&producers[i], NULL, &producer, NULL)) != 0){
      perror("nella pthread create");
      return 1;
    }
  }

  // creo i consumers 
  for(int i = 0; i < N_CONSUMERS; ++i){
    if ((rv = pthread_create(&consumers[i], NULL, &consumer, NULL)) != 0)
    {
      perror("nella pthread create");
      return 1;
    }
  }

  //joino tutto
  for (int i = 0; i < N_PRODUCERS; ++i){
    pthread_join(producers[i] , NULL);
  }
  for (int i = 0; i < N_CONSUMERS; ++i){
    pthread_join(consumers[i] , NULL);
  }

  printf("Global summ : %d\n", global_sum);

  return 0;
}

void *producer()
{
  for (int i = 0; i < N_MESSAGES; i++)
  {
    int n = rand() % 10;
    pthread_mutex_lock(&mux);

    // non devo aspettare nulla nel producer dato che la queue ha spazio illimitato
    enqueue(q, n);
    printf("Wrote : %d\n", n);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mux);
  }
  return NULL;
}

void *consumer()
{
  int personal_sum = 0;
  int n;
  for (int i = 0; i < N_MESSAGES; i++)
  {
    pthread_mutex_lock(&mux);

    // aspetta fino a quando il buffer non ha dati
    while ( isEmpty(q) )
    {
      pthread_cond_wait(&not_empty, &mux);
    }

    // read il dato
    n = dequeue(q);
    printf("tail : %d\n", n);
    personal_sum += (n % 2 == 0 ? n : 0);

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mux);
  }
  global_sum += personal_sum;
  return NULL;
}