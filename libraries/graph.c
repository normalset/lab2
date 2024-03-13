#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#include "Stack.h"
#include "graph.h"
#include "Queue.h"


// Carico il contenuto del file indicato nell'array File come lista di adiacenza di un grafo G, 
// orientato o non orientato
Graph load(char* file, int orientato )
{
		FILE* in=fopen(file,"r");

		if(in){
			Graph g;

			fscanf(in,"%d",&g.N);
			g.nodes=(Node*)malloc(g.N*sizeof(Node));


			for(int i=0;i<g.N;i++){
				g.nodes[i].val=i;
				g.nodes[i].adjList=NULL;
			}
			int n1,n2;
			float w;
			g.E=0;
			//Leggo nodo 1, nodo 2 ( sorgente e destinatario ) e il peso, un valore floating point
			while(fscanf(in, "%d %d %f",&n1,&n2,&w)==3){
				// Carichiamo il grafo come orientato				
				insert(&g.nodes[n1].adjList,n2,w);
				// Carichiamo il grafo come non orientato, inserendo l'arco nella direzione inversa
				if(!orientato)
					insert(&g.nodes[n2].adjList,n1,w);
				g.E++;
			}
			return g;

		} else {
			perror("Open file to read: ");
			exit(1);
		}	
}

// Salvo il contenuto della lista di adiacenza del grafo G in un file indicato nell'array File
void save(Graph g, char* file)
{
	FILE* out=fopen(file,"w");

	if(out){
		fprintf(out,"%d\n",g.N);
		for(int i=0;i<g.N;i++){
			fprintf(out,"%d ",g.nodes[i].val);
		}
		for(int i=0;i<g.N;i++){
			Edge* temp=g.nodes[i].adjList;
			// Scorro fino alla fine della lista di adiacenza del vertice i-esimo
			while(temp){
				//Salvo questo arco
				if(temp->dest>i)
					fprintf(out,"\n%d %d %f",i,temp->dest, temp->w);
				temp=temp->next;
			}
		}
		fclose(out);	

	} else {
			perror("Open file to write: ");
			exit(1);
		}
}

// Inserisco l'arco con vertice destinazione dest e peso weight alla lista di adiacenza corrente
void insert(Edge** adjList,int dest,float weight)
{
	Edge* e=(Edge*)malloc(sizeof(Edge));
	e->dest=dest;
	e->w=weight;
	e->next=*adjList;
	*adjList=e;
}

// Marco un certo vertice come visitato
int visit(int n, int p, int* prec, int* result, int visitedN)
{
	
	// insercisco il vertice di indice p come padre del vertice di indice n
	prec[n]=p;
	
	// Marco il vertice di indice n come il "visitedN"-esimo visitato dall'algoritmo, inserendolo nell'array contenente il risultato della visita DFS
	//printf("Il padre di %d e\' %d\n",n,p);
	result[visitedN]=n;
	
	// Ritorno il nuovo numero di vertici visitati
	return visitedN+1;
}

// Visita dfs iterativa, utilizzando una pila

int* dfs(Graph g, int start, int* hasCycles, int* nConComp)
{
	// Array contenente il vertice predecessore di un certo vertice ( il padre nell'albero costruito dalla DFS )
	int prec[g.N];
	
	// Array contenente i vertici in ordine di visita secondo la DFS
	int* result=(int*)malloc(g.N*sizeof(int));
	
	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1. Se non ho un padre, o sono il nodo radice ( predecessore
	// speciale -2) o non sono ancora stato visitato
	for(int i=0;i<g.N;i++)
		prec[i]=-1;
	
	// Inizializzo la pila
	Stack s=NULL;
	
	// Inserisco in pila il vertice da cui iniziare la visita. Il nodo radice ha se stesso come padre.
	push(&s, start, start);
	
	// inizializzo il numero di componenti connesse a 1, e se esistono cicli a falso
	if(nConComp) 
		*nConComp=1;
	
	if(hasCycles)
		*hasCycles=0;

	//Visito tutti i vertici in profondità, ottenendo una foresta
	while(visitedN<g.N)
	{
		int parent;
		// Estraggo il vertice in testa alla pila di cui devo visitare ancora i vertici adiacenti
		int v=pop(&s,&parent);
		
		if(prec[v]==-1)
		{
			// Marco il vertice come visitato
			visitedN = visit(v,parent,prec,result, visitedN);

			// Puntatore alla lista di adiacenza del vertice v
			Edge* e=g.nodes[v].adjList;

			// Termino quando la lista di adiacenza di v è vuota, inserendo tutti quelli non ancora
			// visitati nella pila
			while(e!=NULL){
				// Non ho trovato sicuramente un ciclo e non ho ancora visitato il vertice destinazione
				// dell'arco e: continuo a visitare, inserendolo in pila
				if(prec[e->dest]==-1)
				{
					// Inserisco il vertice di destinazione dell'arco e in pila
					push(&s,e->dest,v);
				}
				else
				{
					// Ho gi� visitato in precedenza il vertice di destinazione dell'arco e: controllo se l'arco
					// è un arco all'indietro oppure un arco tra i due vertici in un grafo non diretto
					// ( che sarebbe lo stesso arco ). Se � un arco tra i due vertici, lo ignoro, sltrimenti ho
					// identificato un ciclo. In entrambi i casi, proseguo con il prossimo vertice in lista.
					if(prec[v]!=e->dest)
						if(hasCycles)
							*hasCycles=1;
				}
				e=e->next;
			}



		}

		// La pila è vuota, ma non ho terminato di visitare tutti i vertici: creo un nuovo albero con il
		// successivo vertice del grafo non visitato
		if(s==NULL && visitedN<g.N)
		{
			int i=0;
			while(prec[i]!=-1)
				i++;
			push(&s,i,i);
			if(nConComp)
				(*nConComp)++;
		}
	}
	
	return result;
}

// Procedura interna per eseguire la visita ricorsiva, non esposta nell'interfaccia nel file .h
int dfs_rec_visit(Graph g, int v, int prec[], int visitedN, int result[], int* hasCycles, int* nConComp)
{

	// Puntatore alla lista di adiacenza del vertice v
	Edge* e=g.nodes[v].adjList;
	// Termino quando la lista di adiacenza di v è vuota, inserendo tutti quelli non ancora 
	// visitati nella pila
	while(e!=NULL)
	{
		// Non ho trovato sicuramente un ciclo e non ho ancora visitato il vertice destinazione 
		// dell'arco e: continuo a visitare, inserendolo in pila 
		if(prec[e->dest]==-1)
		{
			// Marco il vertice di destinazione dell'arco e come visitato, indicando v come suo padre 
			visitedN = visit(e->dest,v,prec,result,visitedN);
			// Vado in ricorsione sul vertice destinazione dell'arco e nella lista di adiacenza di v
			visitedN = dfs_rec_visit(g,e->dest,prec,visitedN,result,hasCycles,nConComp);		
		} 
		else
		{
			// Ho già visitato in precedenza il vertice di destinazione dell'arco e: controllo se l'arco
			// è un arco all'indietro oppure un arco tra i due vertici in un grafo non diretto 
			// ( che sarebbe lo stesso arco ). Se è un arco tra i due vertici, lo ignoro, altrimenti ho 
			// identificato un ciclo. In entrambi i casi, proseguo con il prossimo vertice in lista.
			if(prec[v]!=e->dest)
				if(hasCycles) 
					*hasCycles=1;
		}
		e=e->next;
	}
	return visitedN;
}

// Visita dfs ricorsiva

int* dfs_rec(Graph g, int start, int* hasCycles, int* nConComp)
{
	// Array contenente il vertice predecessore di un certo vertice ( il padre nell'albero costruito dalla DFS )
	int prec[g.N]; 
	
	// Array contenente i vertici in ordine di visita secondo la DFS
	int* result=(int*)malloc(g.N*sizeof(int));
	
	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1. Se non ho un padre, o sono il nodo radice  oppure non sono ancora stato visitato
	for(int i=0;i<g.N;i++)
		prec[i]=-1;
	
	// inizializzo il numero di componenti connesse a 1, e se esistono cicli a falso
	if(nConComp) 
		*nConComp=1;
	
	if(hasCycles)
		*hasCycles=0;

	// Visito tutti i vertici del grafo, saltando quelli già visitati
	for(int i=0; i<g.N; i++)
	{
		// Controllo se il vertice è stato o no già visitato da una precedente visita DFS
		if(prec[i]==-1)
		{	
			// Inserisco in pila il vertice da cui iniziare la visita. Il vertice radice ha se stesso come padre.
			visitedN = visit(start, start ,prec,result, visitedN);
			visitedN = dfs_rec_visit(g,i,prec,visitedN,result,hasCycles,nConComp);
		}

	}

	return result;
}

// Visita bfs iterativa, usando una lista concatenata a doppi puntatori come coda
int* bfs(Graph g, int start, int* nConComp)
{
// Array contenente il vertice predecessore di un certo vertice ( il padre nell'albero costruito dalla BFS )
	int prec[g.N]; 
	
	// Array contenente i vertici in ordine di visita secondo la BFS
	int* result=(int*)malloc(g.N*sizeof(int));
	
	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1
	for(int i=0;i<g.N;i++)
		prec[i]=-1;
	
	// Inizializzo la coda
	Queue q;
	initQueue(&q);

	// Inserisco in coda il vertice da cui iniziare la visita
	enqueue(&q,start);
	//printf("visito il nodo %d\n",start);

	// Inserisco in pila il vertice da cui iniziare la visita. Il nodo radice ha se stesso come padre.
	visitedN = visit(start, start,prec,result, visitedN);
	
	// inizializzo il numero di componenti connesse a 1
	if(nConComp) 
		*nConComp=1;

	//Visito tutti i vertici in ampiezza, ottenendo una foresta
	while(!isEmpty(q))
	{
		// Estraggo il vertice in coda di cui devo visitare ancora i vertici adiacenti
		int v=dequeue(&q);
		
		// Puntatore alla lista di adiacenza del vertice v
		Edge* e=g.nodes[v].adjList;
		
		// Termino quando la lista di adiacenza di v è vuota, inserendo tutti quelli non ancora 
		// visitati nella coda
		while(e!=NULL){
			// Non ho ancora visitato il vertice destinazione dell'arco e: continuo a visitare, inserendolo in coda.
			// Altrimenti, proseguo.  
			if(prec[e->dest]==-1)
			{
				printf("visito il nodo %d\n",e->dest);
				// Marco il vertice di destinazione dell'arco e come visitato, indicando v come suo padre 
				visitedN = visit(e->dest,v,prec,result,visitedN);
				
				// Inserisco il vertice di destinazione dell'arco e in coda
				enqueue(&q,e->dest);
			}
			e=e->next;
		}
		
		// La coda è vuota, ma non ho terminato di visitare tutti i vertici: creo un nuovo albero con il 
		// successivo vertice del grafo non visitato
		if(isEmpty(q) && visitedN<g.N)
		{
			int i=0;
			while(prec[i]!=-1)
				i++;
			visitedN = visit(i,i,prec,result,visitedN);
			enqueue(&q,i);
			if(nConComp) 
				(*nConComp)++;
		}
	}
	
	return result;
}

int hasCycles(Graph g){
	int cycles;
	int* temp=dfs(g,0,&cycles,NULL);
	free(temp);
	return cycles;
}

int countConnectedComponents(Graph g){
	int ncc;
	int* temp=dfs(g,0,NULL,&ncc);
	free(temp);
	return ncc;
}

int isTree(Graph g){
	int cycles,ncc;
	int* temp=dfs(g,0,&cycles,&ncc);
	free(temp);
	return (ncc==1 && !cycles);
}

// Torna valore negativo se il primo elemento � minore del secondo, 0 se uguali, positivo altrimenti
int compareEdge(const void* e1,const void* e2){
	return (int)((CompleteEdge*)e1)->w-((CompleteEdge*)e2)->w;
}

void insertEdge(Graph g, CompleteEdge e){
	insert(&(g.nodes[e.n1].adjList), e.n2,e.w);
	insert(&(g.nodes[e.n2].adjList), e.n1,e.w);
}

void remove1 (Edge** adjList, int n){
	if((*adjList)!=NULL){
			if((*adjList)->dest==n){
				Edge* temp=(*adjList);
				(*adjList)=temp->next;
				free(temp);
			} else {
				remove1(&((*adjList)->next),n);
			}
	}
}

void removeEdge(Graph g, CompleteEdge e){
	remove1(&(g.nodes[e.n1].adjList),e.n2);
	remove1(&(g.nodes[e.n2].adjList),e.n1);	
}


// Algoritmo di Kruskal per costruire il Minimum spanning tree di un grafo G connesso non orientato
Graph Kruskal(Graph g){

	Graph tree;
	tree.N=g.N;
	tree.E=0;

	tree.nodes=(Node*)malloc(g.N*sizeof(Node));

	if(!tree.nodes){
		printf("Memory error");
		exit(1);
	}

	int ConComp[g.N];

	// Inizializzo l'albero MST e identifico la componente connessa di ogni vertice come il proprio indice
	for(int i=0;i<tree.N;i++){
		tree.nodes[i].val=g.nodes[i].val;
		tree.nodes[i].adjList=NULL;
		ConComp[i] = i;
	}

	//save(tree,"temp.txt");

	CompleteEdge edges[g.E];


	// Costruisco gli archi completi, dove ho la coppia sia del vertice sorgente che del vertice destinatario,
	// invece che solo il destinatario
	int dim=0;

	// Scorro i vertici
	for(int i=0;i<g.N;i++){
		Edge* al=g.nodes[i].adjList;
		// Scorro la lista di adiacenza di ogni vertice, copio gli archi una volta sola, ad esempio
		// quando il primo vertice � pi� picoclo del secondo
		while(al!=NULL){
			if(i<al->dest){
				CompleteEdge e;
				e.n1=i;
				e.n2=al->dest;
				e.w=al->w;

				// Inserisco tutti i nuovi archi
				edges[dim]=e;
				dim++;
			}
			al=al->next;
		}
	}

	// Ordino gli archi in ordine crescente di peso
	qsort(edges,g.E,sizeof(CompleteEdge),compareEdge);


	// Controllo se l'inserimento del nuovo arco � tra vertici della stessa componente connessa: ottengo un ciclo, quindi non lo inserisco.
	int e=0;
	while(tree.E<tree.N-1)
	{
		// Controllo se sono nella stessa componente connessa o meno: se non lo sono, aggiorno tutti i vertici
		// appartenenti alla componente connessa della destinazione dell'arco a risiedere in quella del primo vertice dell'arco
		if(ConComp[edges[e].n1]!=ConComp[edges[e].n2])
		{
			insertEdge(tree,edges[e]);
			// Aggiorno tutti vertici appartenente alla componente connessa della destinazione dell'arco a risiedere
			// in quella del primo vertice dell'arco
			int ConCompN2 = ConComp[edges[e].n2];
			for(int i=0;i<g.N;i++)
				if(ConComp[i]==ConCompN2)
					ConComp[i] = ConComp[edges[e].n1];
			tree.E++;
		}
		e++;
	}

	return tree;	

}


// Algoritmo di Prim per costruire il Minimum spanning tree di un grafo G connesso non orientato
Graph Prim(Graph g)
{

	float w[g.N];
	int mstSet[g.N];
	int parent[g.N];

	Graph tree;
	tree.N=g.N;
	tree.E=0;

	tree.nodes=(Node*)malloc(g.N*sizeof(Node));

	if(!tree.nodes)
	{
		printf("Memory error");
		exit(1);
	}

	for(int i=0;i<tree.N;i++){
		tree.nodes[i].val=g.nodes[i].val;
		tree.nodes[i].adjList=NULL;
	}

	for (int i = 0; i < g.N; i++)
	{
		w[i] = FLT_MAX;
		mstSet[i] = 0;
		parent[i] = -1;
	}

	w[0] = 0.0;
	parent[0] = 0;

	for (int j=0;j<g.N;j++)
	{
		int minimo_v = -1;

		// Calcolo il minimo peso degli archi tra i vertici nel MST e quelli che non ci sono
		for (int i = 0; i < g.N; i++)
			if (!mstSet[i] && (minimo_v == -1 || w[i] < w[minimo_v]))
				minimo_v = i;

		mstSet[minimo_v] = 1;

		// Inserisco l'arco tra il vertice estratto e suo padre nel MST. Non lo faccio per la radice,
		// il vertice 0, l'unico che ha s� stesso come padre
		if(parent[minimo_v]!=minimo_v)
		{
			insert(&(tree.nodes[parent[minimo_v]].adjList), minimo_v, w[minimo_v]);
			insert(&(tree.nodes[minimo_v].adjList), parent[minimo_v], w[minimo_v]);
		}

		Edge* e=g.nodes[minimo_v].adjList;

		while(e!=NULL)
		{
			int v_dest = e->dest;

			// Controllo se il vertice di destinazione dell'arco e � gi� nel MST,
			// altrimenti controllo se il nuovo arco ha peso minore per raggiungere
			// il vertice di destinazione
			if (!mstSet[v_dest] && e->w < w[v_dest])
			{
				// Assegno il vertice attuale come nuovo padre di v_dest
				parent[v_dest] = minimo_v;
				w[v_dest] = e->w;
			}
			e=e->next;
		}
	}
	return tree;
}

// Algoritmo di Dijkstra per calcolare l'albero dei cammini minimi dalla sorgente a qualsiasi altro vertice
Graph Dijkstra(Graph g, int source)
{
	Graph tree;
	tree.N=g.N;
	tree.E=0;

	int Q[g.N];
	int num_q_element = 1;
	int parent[g.N];
	float path_w[g.N];
	float w[g.N];


	tree.nodes=(Node*)malloc(g.N*sizeof(Node));

	if(!tree.nodes)
	{
		printf("Memory error");
		exit(1);
	}

	for(int i=0;i<tree.N;i++)
	{
		tree.nodes[i].val=g.nodes[i].val;
		tree.nodes[i].adjList=NULL;

		if(i==source)
		{
			parent[source] = source;
			Q[source] = 1;
			path_w[0] = 0;
		}
		else
		{
			path_w[i] = FLT_MAX;
			parent[i] = -1;
			Q[i] = 0;
		}
	}



	while(num_q_element>0)
	{
		// Estraggo da Q il vertice che ha la distanza minore da source
		int minimo_v = -1;
		float min_path_w = FLT_MAX;

		for (int i = 0; i < g.N; i++)
			if (Q[i] && (path_w[i] < min_path_w))
			{
				minimo_v = i;
				min_path_w = path_w[i];
			}

		Q[minimo_v] = 0;
		num_q_element--;

		// Inserisco il vertice estratto da Q nell'albero, inserendo l'arco tra lui  e suo padre, escludendo la radice
		if(parent[minimo_v]!=minimo_v)
		{
			insert(&(tree.nodes[parent[minimo_v]].adjList), minimo_v, w[minimo_v]);
			insert(&(tree.nodes[minimo_v].adjList), parent[minimo_v], w[minimo_v]);
		}

		// Scorro gli archi della lista di adiacenza del vertice minimo_v e aggiorno le distanze da source
		Edge* e=g.nodes[minimo_v].adjList;

		while(e!=NULL)
		{
			int v_dest = e->dest;
			int new_path_w = path_w[minimo_v] + e->w;
			if (path_w[v_dest] > new_path_w)
			{
				// Assegno il vertice attuale come nuovo padre di v_dest
				parent[v_dest] = minimo_v;
				path_w[v_dest] = new_path_w;
				w[v_dest] =  e->w;

				if(Q[v_dest] == 0)
				{
					Q[v_dest] = 1;
					num_q_element++;
				}
			}
			e=e->next;
		}

	}

	return tree;

}
