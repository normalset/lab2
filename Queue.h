typedef struct nq{
	int index; // Indice del vertice in coda
	struct nq * next; // Puntatore all'elemento successivo della coda
	struct nq * pred; // Puntatore all'elemento precedente della coda
} QueueNode;

typedef struct coda {
		QueueNode* head;
		QueueNode* tail;
} Queue;

void initQueue (Queue*);
int isEmpty(Queue);
void enqueue(Queue*, int);
int dequeue(Queue*);

