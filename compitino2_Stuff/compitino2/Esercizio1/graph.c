#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "graph.h"


// Carico il contenuto del file indicato nel path 'file' come lista di adiacenza di un grafo g
// Il grafo si suppone sempre orientato, per grafi non orientati, ogni arco va rappresentato due volte nel file
Graph load(char* file)
{
		FILE* in=fopen(file,"r");

		if(in){
			int num_vertici;
			fscanf(in,"%d",&num_vertici);
			Graph g = create_graph(num_vertici);

			int n1,n2;
			float w;
		
			//Leggo nodo 1, nodo 2 ( sorgente e destinatario ) e il peso, un valore floating point
			while(fscanf(in, "%d %d %f",&n1,&n2,&w)==3)
			{
				// Inseriamo l'arco nel grafo 'g'				
				insert(&g,n1,n2,w);
			}
			return g;

		} else {
			perror("Open file to read: ");
			exit(1);
		}	
}


// Creo e inizializzo un nuovo grafo con numero di vertici uguale a 'num_vertici'
Graph create_graph(int num_vertici)
{
	Graph g;
	g.N = num_vertici;
	g.nodes = (Node*)malloc(g.N*sizeof(Node));

	for(int i=0;i<g.N;i++)
	{
		g.nodes[i].index=i;
		g.nodes[i].adjList=NULL;
	}
	g.N = num_vertici;
	g.E=0;
	return g;
}

// Libero tutta la memoria allocata per il grafo g
void delete_graph(Graph g)
{
	for(int i=0;i<g.N;i++)
	{
		Edge* e = g.nodes[i].adjList;
		while(e!=NULL)
		{
			Edge* next_e = e->next;
			free(e);
			e = next_e;
		}
	}
}

// Salvo il contenuto della lista di adiacenza del grafo 'g' in un file indicato nel path 'file'.
// Il grafo si suppone sempre orientato

void save(Graph g, char* file)
{
	FILE* out=fopen(file,"w");

	if(out){
		fprintf(out,"%d\n",g.N);
		for(int i=0;i<g.N;i++)
		{
			Edge* temp=g.nodes[i].adjList;
			// Scorro fino alla fine della lista di adiacenza del vertice i-esimo
			while(temp)
			{
				//Salvo questo arco
				if(temp->next == NULL && i==g.N-1)
					fprintf(out,"%d %d %f",i,temp->dest, temp->w);
				else
					fprintf(out,"%d %d %f\n",i,temp->dest, temp->w);
				temp=temp->next;
			}
		}
		fclose(out);	

	} else {
			perror("Open file to write: ");
			exit(1);
		}
}

// Inserisco l'arco con vertice sorgente 'source', destinazione 'dest' e peso 'weight' nel grafo 'g'
void insert(Graph* g,int source, int dest, float weight)
{
	Edge* e = (Edge*)malloc(sizeof(Edge));

	e->source = source;
	e->dest=dest;
	e->w=weight;
	e->next=(g->nodes[source]).adjList;
	(g->nodes[source]).adjList=e;
	g->E++;
}

// Cancello l'arco con vertice sorgente 'source' e destinazione 'dest' dal grafo 'g'
void delete (Graph* g, int source, int dest)
{
	Edge* attuale = (g->nodes[source]).adjList;
	Edge* prec = attuale;

	while(attuale!=NULL)
	{
			if(attuale->dest==dest)
			{
				if(prec == (g->nodes[source]).adjList)
					(g->nodes[source]).adjList = attuale -> next;
				else
					prec->next = attuale->next;
				free(attuale);
				g->E--;
			}
			else
			{
				prec = attuale;
				attuale = attuale->next;
			}
	}
}