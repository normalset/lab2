typedef struct e{
	int dest; // Vertice di destinazione dell'arco, in un grafo orientato
	float w; // Peso di un arco
	struct e * next; // Puntatore al prossimo elemento della lista di adiacenza degli archi
} Edge;

typedef struct {
	int val; // Indice del vertice
	Edge* adjList; // Puntatore alla testa della lista di adiacenza degli archi uscenti
} Node;

typedef struct {
	int N; // Numero di vertici del grafo
	int E; // Numero di archi del grafo
	Node* nodes; // Array contenente i vertici del grafo
} Graph;

typedef struct Edge {
	int n1, n2;
	float w;
} CompleteEdge;

Graph load(char*, int );
void save(Graph, char*);

void insert(Edge** ,int ,float );

int* dfs(Graph, int, int*, int*);

int* dfs_rec(Graph, int, int*,int*);

int* bfs(Graph, int, int*);

int hasCycles(Graph);

int countConnectedComponents(Graph);

int isTree(Graph);

Graph Kruskal(Graph);

Graph Prim(Graph);

Graph Dijkstra(Graph, int);
