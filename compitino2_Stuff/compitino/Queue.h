typedef struct nq{
	int index; 		  // Indice del vertice in coda
	struct nq * next; // Puntatore all'elemento successivo della coda
	struct nq * pred; // Puntatore all'elemento precedente della coda
} QueueNode;

typedef struct coda {
		QueueNode* head;
		QueueNode* tail;
} Queue;

// Inizializza una nuova coda vuota
void initQueue (Queue*);

// Ritorna 1 se la coda è vuota
int isEmpty(Queue);

// Inserisco un vertice di indice 'n' in testa alla coda e ritorno la coda aggiornata
void enqueue(Queue*, int);

// Estraggo l'elemento in fondo alla coda e ritorno l'indice del vertice. Torno -1 se la coda è vuota
int dequeue(Queue*);


 
