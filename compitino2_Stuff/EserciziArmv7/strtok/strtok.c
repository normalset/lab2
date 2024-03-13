#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>
#include <assert.h>


// emulazione della funzione strtok, gestisce un solo carattere di separazione
char *mystrtok(char *s, int separator);
/* { */
/*   static char *stato = NULL; */
  
/*   if (separator == 0) return NULL;  // sanity check */
/*   if (s==NULL) { */
/*     if (stato == NULL) return NULL; // non c'e' nulla da fare */
/*     else s = stato; // ricomincio da dove ero rimasto... */
/*   } */

/*   while (*s == separator) s++; // finche vedo un separatore vado avanti */
  
/*   if (*s == '\0') {  // potrei avere una stringa di soli separatori ";;;;;" */
/*     stato = NULL; */
/*     return NULL; */
/*   } */
/*   // ho un carattere non separatore, devo cercare il prossimo */
/*   int i=0; */
/*   while(s[i] != separator) { */
/*     if (s[i] == '\0') { */
/*       stato = NULL; */
/*       return s; */
/*     } */
/*     ++i; */
/*   } */
/*   s[i]='\0'; */
/*   stato=&s[i+1]; // da dove ricomincero' alla prossima chiamata */
/*   return s; */
/* } */

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "usa: %s separatore stringa [stringa]\n", argv[0]);
    return -1;
  }
  char separator = argv[1][0]; // prendo il primo carattere

  for(int i=2; argv[i]; ++i) {
    char *token = mystrtok(argv[i], separator);
    while(token) {
      puts(token);
      token = mystrtok(NULL, separator);
    }
    // verifico che anche la successiva chiamata a miatok restituisca NULL
    assert(mystrtok(NULL, separator)==NULL);
  }

  return 0;
}

