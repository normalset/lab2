#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Riceve in input un intero con segno e stampa la posizione dei
// bit ad 1 partendo dal bit piu' significativo (da  sinistra
// verso destra). Il bit piu' significativo e' quello in posizione 0,
// il bit meno significativo e' quello in posizione 31.
//
// Esempio: printbitad1(12345) stampa 18 19 26 27 28 31 perche'
// binary32(12345) -> 00000000000000000011000000111001 cioe' i bit
// ad 1, da sinistra a destra, sono in posizione 18 19 26 27 28 e 31.
//
void printbitad1(int b);
/* { */
/*   unsigned int k = 1<<31; */
/*   for(int i=0;i<32;++i) { */
/*     if (b & k) printf("%d ", i); */
/*     k >>= 1; */
/*   } */
/* } */


int main() {
  char buf[16];
    
  printf("Inserisci un numero:\n");
  if (fgets(buf, 16, stdin) == NULL) {
    return -1;
  }

  printbitad1(atoi(buf));
  putchar('\n');
  return 0;
}
