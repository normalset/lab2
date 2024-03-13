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