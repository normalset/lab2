#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 42
static int V[N] = {-1, -18, 36, 4, -2, 6, 17, 19,
		   34, 2, 3, 8, -10, 12, 11, 7,
		   1, -5, 111, -1, -2, 69, 0, 27,
		   54, 35, -13, -11, 7, 28, 106,
                   32, 33, -37, -7, 88, 0, -3,
		   -84, 67, 9, 5};

typedef struct elem {
  int          e;
  struct elem *next;
} elem_t;

// inserimento in ordine crescente
void insertL(elem_t **L, int x);
/* { */
/*   // inserimento in testa */
/*   if (*L==NULL || ((*L)->e >= x)) { */
/*     elem_t *l = malloc(sizeof(elem_t)); */
/*     l->e    = x; */
/*     l->next = *L; */
/*     *L = l; */
/*     return; */
/*   } */
/*   // inserimento in mezzo */
/*   elem_t *s = *L; */
/*   while(s->next != NULL) { */
/*     if (s->next->e >= x) { */
/*       elem_t *l = malloc(sizeof(elem_t)); */
/*       l->e = x; */
/*       l->next = s->next; */
/*       s->next = l; */
/*       return; */
/*     } */
/*     s=s->next; */
/*   } */
/*   // inserimento in fondo */
/*   elem_t *l = malloc(sizeof(elem_t)); */
/*   l->e = x; */
/*   l->next = NULL; */
/*   s->next = l; */
/* } */

// stampa della lista
void printL(elem_t *L);
/* { */
/*   while(L!=NULL) { */
/*     printf("%d ", L->e); */
/*     L=L->next; */
/*   } */
/*   printf("\n"); */
/* } */

// elimina tutti gli elementi a partire da L
void destroyL(elem_t **L);
/* { */
/*   if (L == NULL) return; */
/*   if (*L == NULL) return; */

/*   elem_t *s = *L; */
/*   while(s!=NULL) { */
/*     elem_t *h = s->next; */
/*     free(s); */
/*     s=h; */
/*   } */
/*   *L = NULL; */
/* } */

// per qsort
int compare(const void *p1, const void *p2) {
  const int a = *(int *)p1;
  const int b = *(int *)p2;
  return (a<b)?-1:((a==b)?0:1);
}

int main() {
  elem_t *Lista = NULL;

  for(int i=0;i<N; ++i)
    insertL(&Lista, V[i]); // inserimento ordinato

  printf("Lista ordinata:\n");
  printL(Lista);          // stampa
  destroyL(&Lista);       // liberiamo memoria

  // verifica
  printf("ora ordino con qsort (in place):\n");
  qsort(V, N, sizeof(int), compare);

  for(int i=0;i<N; ++i)
    printf("%d ", V[i]);
  printf("\n");

  return 0;
}
