#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int eulerian_path_possible (Graph g);
int eulerian_path_possible_with_threshold (Graph g, int threshold);

int eulerian_path_possible (Graph g){
	int deg[g.N] ;

	//setup deg array a tutto a 0
	for(int i=0; i<g.N; i++){
		deg[i]=0 ;
	}

	for(int n=0; n< g.N; n++){
		Edge* curAdj = g.nodes[n].adjList;

		while(curAdj != NULL){
			int dest = curAdj->dest ; // Prendo la destinazione
			deg[dest]++;	//deg della destinazione +1
			curAdj = curAdj->next ;
		}
	}

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un path euleriano devo avere tutti i gradi pari a parte 2 nodi che devono essere dispari, per avere un ciruito devo avere tutti i nodi con grado pari
	int disp = 0 ;

	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0){
			disp++ ; //tengo conto di quanti dispari ho
		}
	}
	if(disp==0) 
		return 1;
	if(disp == 2)
		return 0 ;
	else
		return -1 ;
}

int eulerian_path_possible_with_threshold (Graph g, int threshold){
	int deg[g.N] ;

	//setup deg array a tutto a 0
	for(int i=0; i<g.N; i++){
		deg[i]=0 ;
	}

	for(int n=0; n< g.N; n++){
		Edge* curAdj = g.nodes[n].adjList;

		while(curAdj != NULL){
			if(curAdj->w > threshold){ //Aggiungo l'arco all'array dei conteggi dei gradi solo se il suo peso > threshold
					int dest = curAdj->dest ; // Prendo la destinazione
					deg[dest]++;	//deg della destinazione +1
			}
			curAdj = curAdj->next ;
		}
	}

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un path euleriano devo avere tutti i gradi pari a parte 2 nodi che devono essere dispari, per avere un ciruito devo avere tutti i nodi con grado pari
	int disp = 0 ;

	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0){
			disp++ ; //tengo conto di quanti dispari ho
		}
	}
	if(disp == 0) //caso circuito 
		return 1;
	if(disp == 2) //caso path
		return 0 ;
	else //else nulla
		return -1 ;
}

int main(int argc , char** argv)
{
	Graph g = load(argv[1]);

	if(eulerian_path_possible(g) == 0){
		printf("OK1\n");
	}
	else {
		printf("Fail1\n");
	}

	if(eulerian_path_possible_with_threshold(g, 5) == -1){
		printf("OK2\n");
	}
	else {
		printf("Fail2\n");
	}
}
