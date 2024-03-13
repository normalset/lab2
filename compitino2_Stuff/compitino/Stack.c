#include "Stack.h"
#include <stdlib.h>
#include <stdio.h>

// Inserisco un vertice di indice 'n' in pila
void push(Stack* s, int n, Edge* parent_edge){
	StackNode* node=(StackNode*)malloc(sizeof(StackNode));
	if(node==NULL){
		printf("Memory error\n");
		exit(1);
	}
	node->parent_edge=parent_edge;
	node->index=n;
	node->next=*s;
	*s=node;
}

// Estraggo il vertice in testa alla pila

int pop(Stack* s, Edge** parent_edge){
	if(*s==NULL)
		return -1;
	int res=(*s)->index;
	Stack temp=*s;
	*s=(*s)->next;
	*parent_edge = temp->parent_edge;
	free(temp);
	return res;
}


