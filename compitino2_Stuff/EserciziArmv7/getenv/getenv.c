#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *mystrtok(char *s, int separator);
int myncmp(const char *s1, const char *s2, size_t n);

// funzione che emula getenv, prende in input la stringa
// da cercare e l'ambiente di esecuzione (formato key=value)
// WARNING: modifica l'ambiente dato che usa mystrtok!!!
// Funziona soltato in Linux
char *mygetenv(const char *str, char *env[]);
/* { */
/*   if (!str) return NULL; */
/*   if (!env) return NULL; */
/*   int len = strlen(str); */
/*   int i=0; */
/*   while(env[i] != NULL) { */
/*     char *left  = mystrtok(env[i], '='); */
/*     assert(left != NULL); */
/*     char *right = mystrtok(NULL, '='); */
/*     if (myncmp(str, left, len) == 0) */
/*       return right; */
    
/*     ++i; */
/*   } */
/*   return NULL; */
/* } */


int main(int argc, char *argv[]) {
  extern char **environ;  // man 7 environ
  
  if (argc != 2) {
    fprintf(stderr, "use: %s env-var-to-print\n", argv[0]);
    return -1;
  }
  // NOTA: devo chiamare prima la versione con getenv e poi con mygetenv! Perche'?
  printf("uso getenv\n");
  char *str = getenv(argv[1]);
  if (str)
    fprintf(stdout, "%s: %s\n", argv[1], str);
  else
    fprintf(stdout, "%s: not found\n", argv[1]);
  
  printf("uso mygetenv\n");
  char *str2 = mygetenv(argv[1], environ);
  if (str2)
    fprintf(stdout, "%s: %s\n", argv[1], str2);
  else
    fprintf(stdout, "%s: not found\n", argv[1]);

  return 0;

}
