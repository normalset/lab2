#include <stdlib.h>
#include <stdio.h>

// Prototipo della funzione 'merge'
void merge(int a[], int p, int q, int r);

// Prototipo della funzione 'Mergesort' da implementare in assembly Armv7.
// NOTA: la funzione non ha alcun parametro di ritorno
// Chiama ricorsivamente l'algoritmo Mergesort su due sottoarray di 'a1' = a[p,r]
// I due sottoarray di 'a1' sono 'a11' = [p,q] e 'a12' = [q+1,r] con 'q' = (p+r)/2 indice di mezzo dell'array 'a'
// Ritorna in 'a' l'array finale ordinato 
void Mergesort(int a[], int p, int r);
/*
 {
  // se 'p'>='r', ho il caso base della ricorsione: l'array 'a' = a[p,r] è già ordinato, perché privo di elementi o di un solo elemento
  // In questo caso ritorno senza modificare 'a'
  if (p < r) 
  {
    // Calcolo l'indice di mezzo dell'array 'a' = a[p,r]
    // NOTA: la divisione è intera
    int q = (p+r)/2;

    // Ricorro sul primo sottoarray 'a1' = a[p,q]
    Mergesort(a, p, q);
    // Ricorro sul secondo sottoarray 'a2' = a[q+1,r]
    Mergesort(a, q+1, r);
    // Faccio la fusione ordinata dei due sottoarray 'a1' e 'a2'. La trovo al termine della funzione 'merge' in a[p,r] 
    merge(a, p, q, r);
  }
  return;
}
*/

// Funzione che effettua la fusione ( merge ) ordinata di due diversi sottoarray di 'a' 
// in un array di appoggio temporaneo 'b'.
// 'a': array da cui prendere i due sottoarray da fondere. I due sottoarray sono 'a1' = [p,q] e 'a2' = [p+1,r],
// con p <= q <= r
// 'p': indice in 'a' del primo elemento del primo sottoarray
// 'q': indice in 'a' dell'ultimo elemento del primo sottoarray
// 'r': indice in 'a' dell'ultimo elemento del secondo sottoarray
// Ritorna in a[p,r] l'array dato dalla fusione di 'a1' e 'a2'  
void merge(int a[], int p, int q, int r)
{
  // 'b', grande r-p+1 elementi, è l'array di appoggio dove si calcola la fusione degli array 'a1' e 'a2', in ordine
  int i, j, k=0, b[r-p+1];
  i = p;
  j = q+1;

  // Parto a confrontare i due sottoarray 'a1' e 'a2' dai primi elementi di 'a1' e 'a2', li confronto 
  // e metto in 'b[0]' il più piccolo di essi. 
  // Successivamente incremento 'i' se ho copiato 'a[p]' in 'b[0]' o incremento 'j' se ho copiato 'a[q+1]' in 'b[0]'.  
  // Termino quando uno dei due sottoarray non ha più elementi da confrontare con l'altro, cioè o 'i'='q' o 'j'='r'
  while (i<=q && j<=r) {
    if (a[i]<a[j]) {
      b[k] = a[i];
      i++;
    } else {
      b[k] = a[j];
      j++;
    }
    k++;
  }

  // In questo caso ho necessariamente che 'j'>'r': ho terminato il secondo sottoarray. 
  // Copio il resto degli elementi del primo sottoarray, 'a1', da 'i+1' a 'q' in 'b'. 
  // NOTA: gli elementi restanti di 'a1' sono già in ordine, e sono tutti più grandi 
  // di quelli che ho già copiato in 'b' finora   
  while (i <= q) {
    b[k] = a[i];
    i++;
    k++;
  }

  // In questo caso ho necessariamente che 'i'>'q': ho terminato il primo sottoarray. 
  // Copio il resto degli elementi del secondo sottoarray, 'a2', da 'j+1' a 'r' in 'b'. 
  // NOTA: gli elementi restanti di 'a2' sono già in ordine, e sono tutti più grandi 
  //       di quelli che ho già copiato in 'b' finora 
  while (j <= r) {
    b[k] = a[j];
    j++;
    k++;
  }

  // Copio il contenuto di 'b' in 'a' tra gli indici 'p' e 'r'
  for (i=p; p<=r; p++)
    a[p] = b[p-i];
  return;
}

// Funzione che stampa tutti gli elementi di 'a' su una unica riga dello stdout
void print_array(int a[], int n)
{
  for (int i=0; i<n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}


// Funzione main: contiene alcuni casi di test su svariati array di dimensioni diverse e li stampa sullo stdout
int main(void) {
  int V1[] = {1,34,564,-45,0,-34,75,12,0,5,-8,34,1,45,678,-1020};
  Mergesort(V1, 0, 15);
  print_array(V1,16);
  int V2[] = {-2,12,12,0,0,-1,-1};
  Mergesort(V2, 0, 6);
  print_array(V2,7);
  int V3[]={1,1};
  Mergesort(V3, 0, 1);
  print_array(V3,2);
  int V4[] = {-10};
  Mergesort(V4, 0, 0);
  print_array(V4,1);
  return 0;
}