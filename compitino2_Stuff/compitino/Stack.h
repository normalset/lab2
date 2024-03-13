#include "graph.h"

typedef struct ns{
	int index; // Indice del vertice in pila
	Edge* parent_edge; // Arco con il vertice che lo ha scoperto per primo. Utile per l'algoritmo dfs iterativo
	struct ns * next; // Puntatore all'elemento successivo della pila
} StackNode;

typedef StackNode* Stack;

// Inserisce in pila
void push(Stack*, int, Edge*);

// Estrae dalla pila
int pop(Stack*, Edge**);

 
