#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 29
static int V[N] = {-21, -18, -16, -10, -9, -5, -1,
		   0, 2, 3, 4, 6, 7, 8, 9,
		   11, 12, 17, 19, 27, 41, 76,
		   78, 89, 111, 114, 200, 301, 382};

/* cerca x in V (V è ordinato)
 * ritorna -1 se x non è stato trovato 
 * altrimenti ritorna l'indice dell'entry del vettore che contiene x
 */
int cerca(int x, int *v, int i, int j);  // prototipo funzione assembler
int cercaC(int x, int *v, int i, int j) {
  if (i>j) return -1;   // caso base
  int m = (i+j) >> 1;
  if (v[m] == x) return m;
  if (v[m] < x)
    return cercaC(x, v, m+1, j); // cerco a sinistra
  else
    return cercaC(x, v, i, m-1); // cerco a destra
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("V=[%d", V[0]);
    for(int i=1;i<N;++i) printf(", %d",V[i]);
    printf("]\n");
    printf("usa %s valore-da-cercare\n", argv[0]);
    return -1;
  }
  int x= strtol(argv[1], NULL, 10);

  int res;
  if ((res=cerca(x,V,0,N)) <0 )
    printf("Non trovato\n");
  else
    printf("Trovato %d in posizione %d\n", x, res);

  if ((res=cercaC(x,V,0,N)) <0 )
    printf("Non trovato\n");
  else
    printf("Trovato %d in posizione %d\n", x, res);

  return 0;
}
