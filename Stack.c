#include "Stack.h"
#include <stdlib.h>
#include <stdio.h>

// Inserisco un vertice di indice n in pila, con padre p
void push(Stack* s, int n, int p){
	StackNode* node=(StackNode*)malloc(sizeof(StackNode));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}

	node->index=n;
	node->parent=p;
	node->next=*s;
	*s=node;
}

// Estraggo il vertice in testa alla pila

int pop(Stack* s, int* p){
	if(*s==NULL)
		return -1;
	int res=(*s)->index;
	*p = (*s)->parent;
	Stack temp=*s;
	*s=(*s)->next;
	free(temp);
	return res;
}
