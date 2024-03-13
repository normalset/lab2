typedef struct ns{
	int index; // Indice del vertice in pila
	int parent;
	struct ns * next; // Puntatore all'elemento successivo della pila
} StackNode;

typedef StackNode* Stack;

void push(Stack*, int, int);
int pop(Stack*, int*);

