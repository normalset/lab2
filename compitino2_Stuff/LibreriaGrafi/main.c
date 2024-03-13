#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc , char** argv)
{
	// Carico il grafo dal path contenuto in argv[1]
	Graph g = load(argv[1]);

	// Scelgo la funzione dall'opzione contenuta in argv[2]
	int option=atoi(argv[2]);

	// Scelgo il vertice da cui iniziare alcuni degli algoritmi, contenuto in argv[3]
	int start=atoi(argv[3]);

	switch(option)
	{
		case 0:
			save(g,"outputGraph.txt");
			break;
		case 1:
			{
			Graph dfsRes= dfs(g,5,NULL,NULL);
			save(dfsRes,"dfsRes.txt");
			break;
			}
		case 2:
			{
			Graph dfsRecRes= dfs_rec(g,start,NULL,NULL);
			save(dfsRecRes,"dfsResRec.txt");
			break;
 			}
		case 3:
			{
			Graph bfsRes= bfs(g,start,NULL);
			save(bfsRes,"dfsResRec.txt");
			break;
			}
		case 4:
			printf("Grafo con %d componenti connesse.\n", countConnectedComponents(g));
			break;
		case 5:
			if(hasCycles(g))
				printf("Grafo con cicli.\n");
			else
				printf("Grafo senza cicli.\n");
			break;
		case 6:
			if(isTree(g))
				printf("Grafo e' un albero.\n");
			else
				printf("Grafo non e' un albero.\n");
			break;
		case 7:
			{
			Graph resKruskal=Kruskal(g);
			save(resKruskal,"outputKruskal.txt");
			break;
			}
		case 8:
			{
			Graph resPrim=Prim(g,start);
			save(resPrim,"outputPrim.txt");
			break;
			}
		default:
			{
			Graph ssptree = Dijkstra(g,start,0);
			save(ssptree,"outputDijkstra.txt");
			break;
			}
	}
	return 0;
}