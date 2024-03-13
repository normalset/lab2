#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

		Graph g = load(argv[1],0);

		/* save(g,"outputG1.txt");

		printf("save completo\n");

		int* dfsRes= dfs(g,0,NULL,NULL);
		//int* dfsRes= dfs_rec(g,5,NULL,NULL);

		printf("Visita dfs: ");
		for(int i=0;i<g.N;i++)
			printf("%d ",g.nodes[dfsRes[i]].val);
		printf("\n");

		printf("Grafo con %d componenti connesse.\n", countConnectedComponents(g));

		int* dfsRes2= dfs_rec(g,0,NULL,NULL);

		printf("Visita dfs: ");
		for(int i=0;i<g.N;i++)
			printf("%d ",g.nodes[dfsRes2[i]].val);
		printf("\n");
		printf("Grafo con %d componenti connesse.\n", countConnectedComponents(g));


		int* bfsRes= bfs(g,0,NULL);

		printf("Visita bfs: ");
		for(int i=0;i<g.N;i++)
			printf("%d ",g.nodes[bfsRes[i]].val);
		printf("\n");

		if(hasCycles(g))
			printf("Grafo con cicli.\n");
		else
			printf("Grafo senza cicli.\n");

		if(isTree(g))
			printf("Grafo e' un albero.\n");
		else
			printf("Grafo non e' un albero.\n");
		 */
		Graph tree=Kruskal(g);

		save(tree, "newg1.txt");

		Graph tree2=Prim(g);

		save(tree2, "newg2.txt");
		/*
		return 0;

		//Graph D_tree = Dijkstra(g, 0);
		save(D_tree, "newg1.txt");
		 */
		return 0;

}
