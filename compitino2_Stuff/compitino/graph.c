
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

// Marco un certo vertice come visitato e inserisco l'arco con il padre nel grafo 'result'
int visit(Edge* parent_edge, Graph* result, int* parent, int visitedN)
{
	if(parent_edge != NULL)
	{
		// Inserisco il vertice sorgente dell'arco 'parent_edge' come padre del vertice destinazione dell'arco,
		// marcandolo implicitamente come visitato
		parent[parent_edge->dest]=parent_edge->source;		

		// Inserisco nel grafo 'result' il nuovo arco tra il vertice destinazione e 
		// il vertice sorgente dell'arco 'parent_edge' ( dal figlio al padre ), creando un grafo orientato
		// rappresentante una foresta di alberi di visita. 
		// Solo se 'parent_edge' è diverso da NULL ( il vertice destinazione non è radice di un nuovo albero di visita )
		insert(result,parent_edge->dest,parent_edge->source,parent_edge->w);
	}

	// Ritorno il nuovo numero di vertici visitati
	return visitedN+1;
}

// Visita dfs iterativa, usando una pila, del grafo 'g'  a partire dal vertice 'start'.
// 'hasCycles', inizializzata con qualunque valore, ritorna 0 se non ci sono cicli nel grafo 'g', 1 altrimenti
// 'nConComp', inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'. 
// Se ad 'hasCycles' o 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e restano a NULL. 
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento 
// col padre in ordine di visita. Può essere non connesso, cioè una foresta di alberi di visita dfs.

Graph dfs(Graph g, int start, int* hasCycles, int* nConComp)
{
	Graph result_forest = create_graph(g.N);

	int parent[g.N]; // array contenente il vertice padre di un certo vertice nell'albero costruito dalla dfs 

	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1. Se non ho un padre, non sono ancora stato visitato.
	// Il vertice 'start' indica se stesso come proprio padre
	for(int i=0;i<g.N;i++)
		if(i==start)
			parent[i]=i;
		else
			parent[i]=-1;
	
	// Inizializzo la pila
	Stack s=NULL;
	
	// Inserisco in pila il vertice da cui iniziare la visita
	push(&s, start, NULL);
	
	// inizializzo il numero di componenti connesse a 1, e se esistono cicli a falso
	if(nConComp) 
		*nConComp=1;
	
	if(hasCycles)
		*hasCycles=0;

	//Visito tutti i vertici in profondità, ottenendo potenzialmente una foresta
	while(visitedN<g.N)
	{
		Edge* parent_edge;
		// Estraggo il vertice in testa alla pila di cui devo visitare ancora i vertici adiacenti
		int v=pop(&s,&parent_edge);
		
		if((parent[v]==-1)||(parent[v]==v))
		{
			// Marco il vertice come visitato e inserisco l'arco nel grafo finale di visita dfs
			visitedN = visit(parent_edge,&result_forest, parent,visitedN);

			// Puntatore alla lista di adiacenza del vertice 'v'
			Edge* e=g.nodes[v].adjList;

			// Termino quando la lista di adiacenza di 'v' è vuota, inserendo tutti quelli non ancora
			// visitati nella pila
			while(e!=NULL){
				// Non ho trovato sicuramente un ciclo e non ho ancora visitato il vertice destinazione
				// dell'arco 'e': continuo a visitare, inserendolo in pila
				if(parent[e->dest]==-1)
					// Inserisco il vertice di destinazione dell'arco 'e' in pila
					push(&s,e->dest,e);
				else
				{
					// Ho gi� visitato in precedenza il vertice di destinazione dell'arco 'e': controllo se l'arco
					// � un arco all'indietro oppure un arco tra i due vertici in un grafo non diretto
					// ( che sarebbe lo stesso arco ). Se è un arco tra i due vertici, lo ignoro, altrimenti ho
					// identificato un ciclo. In entrambi i casi, proseguo con il prossimo vertice in lista.
					if(parent[v]!=e->dest)
						if(hasCycles)
							*hasCycles=1;
				}
				e=e->next;
			}
		}

		// La pila � vuota, ma non ho terminato di visitare tutti i vertici: creo un nuovo albero con il
		// successivo vertice del grafo non visitato
		if(s==NULL && visitedN<g.N)
		{
			int i=0;
			while(parent[i]!=-1)
				i++;
			push(&s,i,NULL);
			if(nConComp)
				(*nConComp)++;
		}
	}
	return result_forest;
}

// Procedura interna per eseguire la visita ricorsiva, non esposta nell'interfaccia nel file .h
int dfs_rec_visit(Graph g, int v, int parent[], Graph* result_forest, int visitedN, int* hasCycles, int* nConComp)
{
	// Puntatore alla lista di adiacenza del vertice 'v'
	Edge* e=g.nodes[v].adjList;
	// Termino quando la lista di adiacenza di 'v' è vuota, inserendo tutti quelli non ancora 
	// visitati nella pila
	while(e!=NULL)
	{
		// Non ho trovato sicuramente un ciclo e non ho ancora visitato il vertice destinazione 
		// dell'arco 'e': continuo a visitare, inserendolo in pila 
		if(parent[e->dest]==-1)
		{
			// Marco il vertice di destinazione dell'arco 'e' come visitato 
			visitedN = visit(e,result_forest,parent,visitedN);
			// Vado in ricorsione sul vertice destinazione dell'arco 'e' nella lista di adiacenza di 'v'
			visitedN = dfs_rec_visit(g,e->dest,parent,result_forest,visitedN,hasCycles,nConComp);
		} 
		else
		{
			// Ho già visitato in precedenza il vertice di destinazione dell'arco 'e': controllo se l'arco
			// è un arco all'indietro oppure un arco tra i due vertici in un grafo non diretto 
			// ( che sarebbe lo stesso arco ). Se è un arco tra i due vertici, lo ignoro, altrimenti ho 
			// identificato un ciclo. In entrambi i casi, proseguo con il prossimo vertice in lista.
			if(parent[v]!=e->dest)
				if(hasCycles) 
					*hasCycles=1;
		}
		e=e->next;
	}
	return visitedN;
}

// Visita dfs ricorsiva del grafo 'g' a partire dal vertice 'start'.
// 'hasCycles', inizializzata con qualunque valore, ritorna 0 se non ci sono cicli nel grafo 'g', 1 altrimenti
// 'nConComp', inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'. 
// Se ad 'hasCycles' o 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e restano a NULL. 
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento 
// col padre in ordine di visita. Pu� essere non connesso, cioè una foresta di alberi di visita dfs.
Graph dfs_rec(Graph g, int start, int* hasCycles, int* nConComp)
{
	// Array contenente il vertice padre di un certo vertice nella foresta finale costruita dalla dfs
	int parent[g.N]; 
	
	// Grafo contenente la foresta ottenuta dalla visita DFS
	Graph result_forest = create_graph(g.N);
	
	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1. Se non ho un padre, non sono ancora stato visitato. 
	// Il vertice 'start' indica se stesso come proprio padre
	for(int i=0;i<g.N;i++)
	if(i==start)
		parent[i] = i;
	else
		parent[i]=-1;
	
	// inizializzo il numero di componenti connesse a 1, e se esistono cicli a falso
	if(nConComp) 
		*nConComp=1;
	
	if(hasCycles)
		*hasCycles=0;

	// Visito tutti i vertici del grafo, saltando quelli gi� visitati
	for(int i=0; i<g.N; i++)
	{
		// Controllo se il vertice � stato o no gi� visitato da una precedente visita dfs
		if((parent[i]==-1)||(parent[i]==i))
		{	
			// Marco come visitato il vertice radice di un nuovo albero di visita dfs. Il vertice radice ha se stesso come padre.
			visitedN = visit(NULL,&result_forest,parent,visitedN);
			visitedN = dfs_rec_visit(g,i,parent,&result_forest,visitedN,hasCycles,nConComp);
		}

	}
	return result_forest;
}

// Visita bfs iterativa, usando una lista concatenata a doppi puntatori come coda, del grafo 'g' a partire dal vertice 'start'
// nConComp, inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'.
// Se ad 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e resta a NULL.
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento 
// col padre in ordine di visita. Può essere non connesso, cioè una foresta di alberi di visita bfs. 

Graph bfs(Graph g, int start, int* nConComp)
{
	Graph result_forest = create_graph(g.N);

	// Array contenente il vertice predecessore di un certo vertice ( il padre nell'albero costruito dalla BFS )
	int parent[g.N]; 
	
	// Numero dei vertici visitati ad un certo punto della visita
	int visitedN=0;

	// Inizializzo tutti i predecessori a -1
	//  Il vertice 'start' ha se stesso come padre. 
	for(int i=0;i<g.N;i++)
		if(i==start)
			parent[i]=i;
		else
			parent[i]=-1;
	
	// Inizializzo la coda
	Queue q;
	initQueue(&q);

	// Inserisco in coda il vertice da cui iniziare la visita
	enqueue(&q,start);

	// Marco come visitato il vertice da cui iniziare la visita. Il vertice 'start' ha se stesso come padre. 
	visitedN = visit(NULL, &result_forest, parent, visitedN);
	
	// inizializzo il numero di componenti connesse a 1
	if(nConComp) 
		*nConComp=1;

	//Visito tutti i vertici in ampiezza, ottenendo una foresta
	while(!isEmpty(q))
	{
		// Estraggo il vertice in coda di cui devo visitare ancora i vertici adiacenti
		int v=dequeue(&q);
		
		// Puntatore alla lista di adiacenza del vertice 'v'
		Edge* e=g.nodes[v].adjList;
		
		// Termino quando la lista di adiacenza di 'v' è vuota, inserendo tutti quelli non ancora 
		// visitati nella coda
		while(e!=NULL){
			// Non ho ancora visitato il vertice destinazione dell'arco 'e': continuo a visitare, inserendolo in coda.
			// Altrimenti, proseguo.  
			if(parent[e->dest]==-1)
			{
				// Marco il vertice di destinazione dell'arco 'e' come visitato,
				// e inserisco l'arco tra lui e suo padre nella foresta finale
				visitedN = visit(e,&result_forest,parent,visitedN);
				
				// Inserisco il vertice di destinazione dell'arco 'e' in coda
				enqueue(&q,e->dest);
			}
			e=e->next;
		}
		
		// La coda è vuota, ma non ho terminato di visitare tutti i vertici: creo un nuovo albero con il 
		// successivo vertice del grafo non visitato
		if(isEmpty(q) && visitedN<g.N)
		{
			int i=0;
			while(parent[i]!=-1)
				i++;
			visitedN = visit(NULL,&result_forest,parent,visitedN);
			enqueue(&q,i);
			if(nConComp) 
				(*nConComp)++;
		}
	}
	
	return result_forest;
}

// Ritorna 1 se il grafo 'g' ha almeno un ciclo, altrimenti ritorna 0
int hasCycles(Graph g){
	int cycles;
	Graph temp=dfs(g,0,&cycles,NULL);
	delete_graph(temp);
	return cycles;
}

// Ritorna il numero di componenti connesse diverse nel grafo 'g'
int countConnectedComponents(Graph g){
	int ncc;
	Graph temp=dfs(g,0,NULL,&ncc);
	delete_graph(temp);
	return ncc;
}

// Ritorna 1 se il grafo 'g' è un albero, 0 altrimenti. 
int isTree(Graph g){
	int cycles,ncc;
	Graph temp=dfs(g,0,&cycles,&ncc);
	delete_graph(temp);
	return (ncc==1 && !cycles);
}

// Torna valore negativo se il primo elemento è minore del secondo, 0 se uguali, positivo altrimenti
int compareEdge(const void* e1,const void* e2){
	return (int)((*((Edge**)e1))->w-((*(Edge**)e2)->w));
}

// Algoritmo di Kruskal per costruire il Minimum spanning tree di un grafo 'g' connesso non orientato
// Ritorna l'albero MST non orientato
Graph Kruskal(Graph g){

	Graph result_mst = create_graph(g.N);

	int ConComp[g.N];

	// Identifico la componente connessa di ogni vertice come il proprio indice
	for(int i=0;i<result_mst.N;i++)
		ConComp[i] = i;

	// Array contenente gli archi del grafo originale 'g' una sola volta, supponendolo NON orientato
	Edge* archi[g.E/2];


	// Costruisco gli archi completi, dove ho la coppia sia del vertice sorgente che del vertice destinatario,
	// invece che solo il destinatario
	int dim=0;

	// Scorro i vertici
	for(int i=0;i<g.N;i++){
		Edge* al=g.nodes[i].adjList;
		// Scorro la lista di adiacenza di ogni vertice, copio gli archi una volta sola, ad esempio
		// quando il primo vertice è più piccolo del secondo. Suppongo il grafo NON orientato
		while(al!=NULL){
			if(i<al->dest)
				// Inserisco tutti i nuovi archi
				archi[dim++]=al;
			
			al=al->next;
		}
	}

	// Ordino gli archi in ordine crescente di peso
	qsort(archi,g.E/2,sizeof(Edge*),compareEdge);

	// Controllo se il nuovo arco è tra vertici della stessa componente connessa: ottengo un ciclo, quindi non lo inserisco.
	int e=0;
	while(e<g.E/2)
	{
		// Controllo se sono nella stessa componente connessa o meno: se non lo sono, aggiorno tutti i vertici
		// appartenenti alla componente connessa della destinazione dell'arco a risiedere in quella del primo vertice dell'arco
		if(ConComp[archi[e]->source]!=ConComp[archi[e]->dest])
		{
			insert(&result_mst,archi[e]->dest,archi[e]->source,archi[e]->w);
			insert(&result_mst,archi[e]->source,archi[e]->dest,archi[e]->w);


			// Aggiorno tutti vertici appartenente alla componente connessa della destinazione dell'arco a risiedere
			// in quella del primo vertice dell'arco
			int ConComp_n2 = ConComp[archi[e]->dest];

			for(int i=0;i<g.N;i++)
				if(ConComp[i]==ConComp_n2)
					ConComp[i] = ConComp[archi[e]->source];
		}
		e++;
	}
	return result_mst;	
}


// Algoritmo di Prim per costruire il Minimum spanning tree di un grafo 'g' connesso non orientato iniziando dal vertice 'start'
// Ritorna l'albero MST non orientato
Graph Prim(Graph g, int start)
{

	float w[g.N]; // array temporaneo che tiene il peso dell'arco di costo minimo per raggiungere il vertice di indice 'i'
	int mstSet[g.N]; 
	int parent[g.N]; 

	Graph result_mst = create_graph(g.N);

	// Inizializzo il vertice 'start' come padre di sè stesso per indicarlo già visitato.
	// Setto il costo del suo arco migliore verso il padre a 0.
	// Inizializzo tutti gli altri con il padre a -1 ( significa, non visitati ) e il costo del 
	// loro arco di costo minimo per raggiungerli al massimo valore floating point rappresentabile
	for (int i = 0; i < g.N; i++)
	{
		mstSet[i] = 0;
		if(i==start)
		{
			w[start] = 0.0;
			parent[start] = start;
		}
		else
		{
			parent[i] = -1;
			w[i] = FLT_MAX;
		}
		
	}

	for (int j=0;j<g.N;j++)
	{
		int minimo_v = -1;

		// Calcolo il minimo peso degli archi tra i vertici nel MST e quelli che non ci sono
		for (int i = 0; i < g.N; i++)
			if (!mstSet[i] && (minimo_v == -1 || w[i] < w[minimo_v]))
				minimo_v = i;

		mstSet[minimo_v] = 1;

		// Inserisco l'arco tra il vertice estratto e suo padre nel MST. Non lo faccio per la radice,
		// il vertice 0, l'unico che ha sé stesso come padre
		// Il grafo finale è orientato dal figlio verso il padre nel MST finale

		if(parent[minimo_v]!=minimo_v)
		{
			insert(&result_mst, minimo_v, parent[minimo_v], w[minimo_v]);
			insert(&result_mst, parent[minimo_v],  minimo_v, w[minimo_v]);

		}
		Edge* e=g.nodes[minimo_v].adjList;

		while(e!=NULL)
		{
			int v_dest = e->dest;

			// Controllo se il vertice di destinazione dell'arco è già nel MST,
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
	return result_mst;
}

// Algoritmo di Dijkstra per calcolare l'albero dei cammini minimi dal vertice sorgente 'source' a qualsiasi altro vertice.
// Valido per grafi orientati o meno: 'orientato' = 1 indica che il grafo è orientato, 0 se è non orientato
// Ritorna l'albero dei cammini minimi.
Graph Dijkstra(Graph g, int source, int orientato)
{
	Graph result_ssp = create_graph(g.N);

	int Q[g.N];
	int num_q_element = 1;
	int parent[g.N]; // array temporaneo che mantiene i padri di ogni nodo nell'albero finale dei cammini minimi
	float path_w[g.N]; // array temporaneo che mantiene il peso del percorso di costo minimo fino al vertice di indice 'i'
	float w[g.N]; // array temporaneo che tiene il peso dell'arco migliore con il vertice padre nell'array 'parent'

	// Inizializzo il vertice 'source' come padre di sè stesso per indicarlo già visitato.
	// Setto il costo del suo percorso migliore verso sé stesso per convenzione a 0.
	// Inizializzo tutti gli altri con il padre a -1 ( significa, non visitati ) e il costo del 
	// loro percorso migliore per raggiungerli da 'source' al massimo valore floating point rappresentabile
	for(int i=0;i<result_ssp.N;i++)
	{
		if(i==source)
		{
			parent[source] = source;
			// Inserisco il vertice 'source' nell'array 'Q'
			Q[source] = 1;
			path_w[source] = 0;
		}
		else
		{
			path_w[i] = FLT_MAX;
			parent[i] = -1;
			// Non inserisco nessun altro vertice in 'Q'
			Q[i] = 0;
		}
	}

	while(num_q_element>0)
	{
		// Estraggo da 'Q' il vertice che ha la distanza minore da 'source'
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

		// Inserisco il vertice estratto da 'Q' nell'albero, inserendo l'arco tra lui e suo padre, escludendo la radice. 
		// Il grafo finale è orientato dal figlio verso il padre
		if(parent[minimo_v]!=minimo_v)
		{
			insert(&result_ssp, minimo_v, parent[minimo_v], w[minimo_v]);
			// Aggiungo l'arco inverso se il grafo non è orientato
			if(!orientato)
				insert(&result_ssp, parent[minimo_v], minimo_v, w[minimo_v]);
		}

		// Scorro gli archi della lista di adiacenza del vertice 'minimo_v' e aggiorno le distanze da 'source'
		Edge* e=g.nodes[minimo_v].adjList;

		while(e!=NULL)
		{
			int v_dest = e->dest;
			int new_path_w = path_w[minimo_v] + e->w;
			if (path_w[v_dest] > new_path_w)
			{
				// Assegno il vertice attuale come nuovo padre di 'v_dest'
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
	return result_ssp;
}
