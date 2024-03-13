#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

// Inizializza una nuova coda
void initQueue (Queue* q)
{
	q->head=NULL;
	q->tail=NULL;
}

// Ritorna 1 se la coda è vuota
int isEmpty(Queue q)
{
	if(!q.head)
		return 1;
	else
		return 0;
}

// Inserisco un vertice di indice n in testa alla coda e ritorno la coda aggiornata
 void enqueue(Queue* q, int n)
{
	QueueNode* node=(QueueNode*)malloc(sizeof(QueueNode));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}
	
	node->index=n;
	node->next=q->head;
	node->pred=NULL;
	
	// Controllo se la coda è vuota: in questo caso, testa e coda devono puntare allo stesso elemento
	if(isEmpty(*q))
		q->tail=node;
	else
		// Coda non vuota: aggiorno il puntatore al precedente dell'elemento successivo al primo, in coda
		(q->head)->pred=node;
		
	q->head=node;
}

// Estraggo l'elemento in fondo alla coda e ritorno l'indice del vertice. Torno -1 se la coda è vuota
int dequeue(Queue* q)
{
	if(isEmpty(*q))
		return -1;
	
	QueueNode* node = q->tail;
	
	int res = node->index;
	
	// Aggiorno l'elemento in coda al precedente di quello estratto
	q->tail = node->pred;
	
	// Se la coda è diventata vuota, anche la testa della coda vale NULL
	if(!(q->tail))
		q->head=NULL;

	free(node);
	return res;
}
