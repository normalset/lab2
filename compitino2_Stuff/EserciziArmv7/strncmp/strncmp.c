#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* confranta al piu' i primi n caratteri delle due stringhe passate come
 * argomento,   
 * ritorna 0 se risulta s1 == s2, !=0 se s1 != s2
 */
int myncmp(/* const */ char *s1, /* const */ char *s2, size_t n);
/* { */
/*   size_t s1_len = strlen(s1); */
/*   size_t s2_len = strlen(s2); */
/*   size_t min    = (s1_len<=s2_len)?s1_len:s2_len; */

/*   if ((s1_len != s2_len) && n > min) return 1; // necessariamente diverse */
/*   if (n>min) n = min; */

/*   for(int i=0; i<n; ++i) */
/*     if (s1[i] != s2[i]) return 1; */
/*   return 0; */
/* } */


int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usa: %s stringa1 stringa2 n\n", argv[0]);
    return -1;
  }  
  
  int n = strtol(argv[3], NULL, 10);
  if (n<0) {
    printf("n non e' valido\n");
    return -1;
  }
  printf("%s\n", (myncmp(argv[1], argv[2], n) ==0)?"UGUALI":"DIVERSE");
  printf("%s\n", (strncmp(argv[1], argv[2], n)==0)?"UGUALI":"DIVERSE");


  return 0;
}
