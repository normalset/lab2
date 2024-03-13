typedef struct e{
	int source; // Vertice di partenza dell'arco, aggiunto per comodità per alcuni algoritmi
	int dest; // Vertice di destinazione dell'arco, in un grafo orientato
	float w; // Peso di un arco
	struct e * next; // Puntatore al prossimo elemento della lista di adiacenza degli archi
} Edge;

typedef struct {
	int index; // Indice del vertice
	Edge* adjList; // Puntatore alla testa della lista di adiacenza degli archi uscenti
} Node;

typedef struct {
	int N; 		 // Numero di vertici del grafo
	int E; 		 // Numero di archi del grafo, sono contati due volte per i grafi non orientati
	Node* nodes; // Array contenente i vertici del grafo
} Graph;


// Creo e inizializzo un nuovo grafo con numero di vertici uguale a 'num_vertici'
Graph load(char*);

// Salvo il contenuto della lista di adiacenza del grafo 'g' in un file indicato nel path 'file'.
// Il grafo si suppone sempre orientato
void save(Graph, char*);

Graph create_graph(int);

// Libero tutta la memoria allocata per il grafo g
void delete_graph(Graph );

// Inserisco l'arco con vertice sorgente 'source', destinazione 'dest' e peso 'weight' nel grafo 'g'
void insert(Graph*, int ,int ,float );

// Cancello l'arco con vertice sorgente 'source' e destinazione 'dest' dal grafo 'g'
void delete (Graph*, int, int);

// Visita dfs iterativa, usando una pila, del grafo 'g'  a partire dal vertice 'start'.
// 'hasCycles', inizializzata con qualunque valore, ritorna 0 se non ci sono cicli nel grafo 'g', 1 altrimenti
// 'nConComp', inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'.
// Se ad 'hasCycles' o 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e restano a NULL.
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento
// col padre in ordine di visita. Può essere non connesso, cioè una foresta di alberi di visita dfs.
Graph dfs(Graph, int, int*, int*);

// Visita dfs ricorsiva del grafo 'g' a partire dal vertice 'start'.
// 'hasCycles', inizializzata con qualunque valore, ritorna 0 se non ci sono cicli nel grafo 'g', 1 altrimenti
// 'nConComp', inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'.
// Se ad 'hasCycles' o 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e restano a NULL.
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento
// col padre in ordine di visita. Pu� essere non connesso, cioè una foresta di alberi di visita dfs.
Graph dfs_rec(Graph , int , int* , int* );

// Visita bfs iterativa, usando una lista concatenata a doppi puntatori come coda, del grafo 'g' a partire dal vertice 'start'
// nConComp, inizializzato con qualunque valore, ritorna il numero di componenti connesse del grafo 'g'.
// Se ad 'nConComp' viene passato NULL, non viene effettuato il relativo controllo e resta a NULL.
// Ritorna il grafo rappresentante l'ordine di visita dei nodi, dove un arco rappresenta il collegamento
// col padre in ordine di visita. Può essere non connesso, cioè una foresta di alberi di visita bfs.
Graph bfs(Graph, int, int*);

// Ritorna 1 se il grafo 'g' ha almeno un ciclo, altrimenti ritorna 0
int hasCycles(Graph);

// Ritorna il numero di componenti connesse diverse nel grafo 'g'
int countConnectedComponents(Graph);

// Ritorna 1 se il grafo 'g' è un albero, 0 altrimenti.
int isTree(Graph);

// Algoritmo di Kruskal per costruire il Minimum spanning tree di un grafo 'g' connesso non orientato
// Ritorna l'albero MST non orientato
Graph Kruskal(Graph);

// Algoritmo di Prim per costruire il Minimum spanning tree di un grafo 'g' connesso non orientato iniziando dal vertice 'start'
// Ritorna l'albero MST non orientato
Graph Prim(Graph, int);

// Algoritmo di Dijkstra per calcolare l'albero dei cammini minimi dal vertice sorgente 'source' a qualsiasi altro vertice.
// Valido per grafi orientati o meno: 'orientato' = 1 indica che il grafo � orientato, 0 se � non orientato
// Ritorna l'albero dei cammini minimi.
Graph Dijkstra(Graph, int, int);
