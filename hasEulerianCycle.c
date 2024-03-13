#include "graph.h"
#include "graph.c"

//Assuming that the graphs are non-directed and the tree is connected
int hasEulerianCycleND(Graph g){ //? 1 = true 0 = false
	int deg[g.N] ;

	//setup deg array a tutto a 0
	for(int i=0; i<g.N; i++){
		deg[i]=0 ;
	}

	for(int n=0; n< g.N; n++){
		Edge* curAdj = g.nodes[n].adjList;

		while(curAdj != NULL){
			deg[n]++ ; //* deg Nodo di partenza +1
			int dest = curAdj->dest ; // Prendo la destinazione
			deg[dest]++;	//deg della destinazione +1

			//todo eventuale conto del peso con curAdj->w

			curAdj = curAdj->next ;
		}
	}

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un ciclo euleriano devo avere tutti i gradi pari
	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0) return 0;
	}
	return 1;
}


int hasEulerianPathND(Graph g){ //? 1 = true 0 = false
	int deg[g.N] ;

	//setup deg array a tutto a 0
	for(int i=0; i<g.N; i++){
		deg[i]=0 ;
	}

	for(int n=0; n< g.N; n++){
		Edge* curAdj = g.nodes[n].adjList;

		while(curAdj != NULL){
			deg[n]++ ; //* deg Nodo di partenza +1
			int dest = curAdj->dest ; // Prendo la destinazione
			deg[dest]++;	//deg della destinazione +1

			//todo eventuale conto del peso con curAdj->w

			curAdj = curAdj->next ;
		}
	}

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un path euleriano devo avere tutti i gradi pari a parte 2 nodi che devono essere dispari
	int disp = 0 ;

	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0){
			disp++ ; //tengo conto di quanti dispari ho
		}
	}
	if(disp == 2)
		return 1 ;
	else
		return 0 ;
}

// Separando il calcolo del grado dei nodi
int* getDegreeNotDirected(Graph g){
	int* degrees = (int*)malloc(g.N * sizeof(int)) ;

	if(degrees == NULL) return NULL ; //! Se ho un errore nell'allocazione di memoria mi fermo

	//setup deg array a tutto a 0
	for(int i=0; i<g.N; i++){
		degrees[i]=0 ;
	}

	for(int n=0; n< g.N; n++){
		Edge* curAdj = g.nodes[n].adjList;

		while(curAdj != NULL){
			degrees[n]++ ; //* deg Nodo di partenza +1
			int dest = curAdj->dest ; // Prendo la destinazione
			degrees[dest]++;	//deg della destinazione +1

			//todo eventuale conto del peso con curAdj->w

			curAdj = curAdj->next ;
		}
	}
	return degrees ;
}

//? se il grafo e' orientato invece
int** getDegreeDirected(Graph g){

		//definisco l'array risultato come array di coppie [in ; out]
		int** degrees = (int**)malloc(g.N * sizeof(int*));

		if(degrees == NULL) return NULL ; //! Se ho errore nell'allocazione di memoria esco

		for(int i =0; i < g.N ; i++){
			degrees[i] = (int*)malloc(2 * sizeof(int));
			degrees[i][0] = 0 ;
			degrees[i][1] = 0 ;
		}

		for(int n=0; n< g.N; n++){
			Edge* curAdj = g.nodes[n].adjList;

			while(curAdj != NULL){
				degrees[n][1]++; //deg uscente del nodo n + 1
				int dest = curAdj->dest ; // Prendo la destinazione
				degrees[dest][0]++;	//deg della destinazione in entrata +1

				//todo eventuale conto del peso con curAdj->w

				curAdj = curAdj->next ;
			}
		}
	return degrees ;
}

//? riscrivendo lu funzioni usando la funzione esterna per il grado
int hasEulerianCycleND2(Graph g){ //? 1 = true 0 = false
	int* deg = getDegreeNotDirected(g) ;

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un ciclo euleriano devo avere tutti i gradi pari
	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0){
			free(deg);
			return 0;
		}
	}
	free(deg);
	return 1;
}

int hasEulerianPathND2(Graph g){
	int * deg = getDegreeNotDirected(g) ;
	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un path euleriano devo avere tutti i gradi pari a parte 2 nodi che devono essere dispari
	int disp = 0 ;

	for(int i=0 ; i < g.N ; i++){
		if(deg[i] % 2 != 0){
			disp++ ; //tengo conto di quanti dispari ho
		}
	}
	if(disp == 2){
		free(deg);
		return 1 ;
	}
	else{
		free(deg);
		return 0 ;
	}
}

//? Implementazione con anche eventuali grafi orientati
int hasEulerianCycleD(Graph g){ //? 1 = true 0 = false
	int** deg = getDegreeDirected(g) ;

	//* Ho ora un arrai con tutti i gradi dei diversi nodi, per avere un ciclo euleriano devo avere tutti i gradi in uscita e entrata uguali
	for(int i=0 ; i < g.N ; i++){
		if(deg[i][0] != deg[i][1]){
			//dealloco memoria
			for(int j=0 ; j < g.N ; j++){
				free(deg[j]);
			}
			free(deg);
			return 0;
		}
	}
	for(int j=0 ; j < g.N ; j++){
		free(deg[j]);
	}
	free(deg);
	return 1;
}

//! DA CONTROLLARE
int hasEulerianPathD(Graph g){
	int ** deg = getDegreeDirected(g);

	int src = 0;
	int dest = 0;
	for(int i = 0 ; i < g.N ; i++){
		if(deg[i][0] != deg[i][1]){
			if(deg[i][0]==0 && deg[i][1])
				src++ ; // Ho una sorgente senza entrata e una sola uscita
			else if(deg[i][0] == 1 && deg[i][1] == 0) // Ho un pozzo con solo una entrata e nessun uscita
				dest++;
			else{ // se non ho entrate e uscite uguali e non ho una sorgente 
			for(int j=0 ; j < g.N ; j++){
				free(deg[j]);
			}
			free(deg);
			return 0 ;
			}
		}
	}
	//dealloco memoria
	for(int j=0 ; j < g.N ; j++){
		free(deg[j]);
	}
	free(deg);

	if(src > 1 || dest > 1) //se ho piu' di una source o well non ho un path
		return 0 ;
	else	
		return 1 ;
}