#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Inizializza una nuova coda
void BFS_initQueue (BFS_Queue* q)
{
	q->head=NULL;
	q->tail=NULL;
}

void printQ(BFS_Queue* q)
{
	BFSQueueNode * temp = (BFSQueueNode*)malloc(sizeof(BFSQueueNode)) ;
	if(temp==NULL){
		printf("Error while allocating memory");
		exit(1);
	}

	temp = q->head;
	while(temp != NULL){
		printf("[%d,%d]" , temp->index[0] , temp->index[1]);
		temp = temp -> next;
	}
	free(temp);
}

// Ritorna 1 se la pila non è vuota , 0 se e' vuota
int BFS_isEmpty(BFS_Queue q)
{
	if(!q.head)
		return 1;
	else
		return 0;
}

// Inserisco un vertice di coordinate [row,col] in testa alla coda 
void BFS_enqueue(BFS_Queue* q, int row, int col)
{
	BFSQueueNode* node=(BFSQueueNode*)malloc(sizeof(BFSQueueNode));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}
	
	node->index[0]=row;
	node->index[1]=col;
	node->next=q->head;
	node->pred=NULL;
	
	// Controllo se la coda è vuota: in questo caso, testa e coda devono puntare allo stesso elemento
	if(BFS_isEmpty(*q))
		q->tail=node;
	else
		// Coda non vuota: aggiorno il puntatore al precedente dell'elemento successivo al primo, in coda
		(q->head)->pred=node;
		
	q->head=node;
}

// Estraggo l'elemento in fondo alla coda e ritorno l'indice del vertice. Errore se la coda e' vuota
int* BFS_dequeue(BFS_Queue* q)
{
	if(BFS_isEmpty(*q))
		perror("trying to dequeue an empty BFS_Queue");
	
	BFSQueueNode* node = q->tail;
	
	int * res = malloc(sizeof(int) * 2)  ;
	res[0] = node->index[0];
	res[1] = node->index[1];
	
	// Aggiorno l'elemento in coda al precedente di quello estratto
	q->tail = node->pred;
	
	// Se la coda è diventata vuota, anche la testa della coda vale NULL
	if(!(q->tail))
		q->head=NULL;

	free(node);
	return res;
}



/// CODA DI PAROLE

// Inizializza una nuova coda
void WORD_initQueue (WORD_Queue* q)
{
	q->head=NULL;
	q->tail=NULL;
}

// Ritorna 1 se la pila non è vuota , 0 se lo e'
int WORD_isEmpty(WORD_Queue q)
{
	if(!q.head)
		return 1;
	else
		return 0;
}


// Inserisco una parola in testa alla coda
void WORD_enqueue(WORD_Queue* q, char * word )
{
	WORDQueueNode* node=(WORDQueueNode*)malloc(sizeof(WORDQueueNode));
	node->word = malloc(sizeof(char) * strlen(word));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}
	
	strcpy(node->word , word);
	node->next=q->head;
	node->pred=NULL;
	
	// Controllo se la coda è vuota: in questo caso, testa e coda devono puntare allo stesso elemento
	if(WORD_isEmpty(*q))
		q->tail=node;
	else
		// Coda non vuota: aggiorno il puntatore al precedente dell'elemento successivo al primo, in coda
		(q->head)->pred=node;
		
	q->head=node;
}

// Estraggo l'elemento in fondo alla coda. Errore se la coda e' vuota 
void WORD_dequeue(WORD_Queue* q)
{
	if(WORD_isEmpty(*q))
		perror("trying to dequeue an empty BFS_Queue");
	
	WORDQueueNode* node = q->tail;
	
	// char * res = malloc(sizeof());
	// strcpy(res , node->word);
	
	// Aggiorno l'elemento in coda al precedente di quello estratto
	q->tail = node->pred;
	
	// Se la coda è diventata vuota, anche la testa della coda vale NULL
	if(!(q->tail))
		q->head=NULL;

	free(node);
	return;
}

//Stampo a video la coda di parole partendo dalla fine e tornando indietro
void WORD_printqueue(WORD_Queue * q){
	WORDQueueNode * node = (WORDQueueNode*)malloc(sizeof(WORDQueueNode)); 
	node = q->tail ; 
	int counter = 1 ;

	while(node != NULL){
		printf("word n%d : %s \n" , counter , node->word);
		node = node->pred ; 
		counter++ ; 
	}
	free(node) ; 
}

//Restituisco la word contenuta nell'ultimo elemento della coda di parole
char * WORD_gettail(WORD_Queue * q){
	return q->tail->word ; 
}