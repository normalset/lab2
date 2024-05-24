// Matrix BFS BFS_Queue
typedef struct nq{
	int index[2]; // Indice del vertice in coda
	struct nq * next; // Puntatore all'elemento successivo della coda
	struct nq * pred; // Puntatore all'elemento precedente della coda
} BFSQueueNode;
 
typedef struct bfs_coda {
		BFSQueueNode* head;
		BFSQueueNode* tail;
} BFS_Queue;

void BFS_initQueue(BFS_Queue*);
int BFS_isEmpty(BFS_Queue);

void BFS_enqueue(BFS_Queue*, int , int);
int* BFS_dequeue(BFS_Queue*);


// Words BFS_Queue
typedef struct word{
	char * word ; // parola in se' della coda
	struct word * next; // Puntatore all'elemento successivo della coda
	struct word * pred; // Puntatore all'elemento successivo della coda
}WORDQueueNode ; 

typedef struct word_coda {
		WORDQueueNode* head;
		WORDQueueNode* tail;
} WORD_Queue;


void WORD_initQueue(WORD_Queue*);
int WORD_isEmpty(WORD_Queue);

void WORD_enqueue(WORD_Queue* , char *);
void WORD_dequeue(WORD_Queue*);
void WORD_printqueue(WORD_Queue*) ; 
char * WORD_gettail(WORD_Queue *);

