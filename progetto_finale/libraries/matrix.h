#include <stdbool.h>
#include <string.h>
#include <stdio.h>

//Definisco la size dell'alfabeto
#define NUM_CHAR (256)

typedef struct trienode {
  struct trienode *children[NUM_CHAR];
  bool terminal ; 
} trienode ; 

trienode *createnode() ;

bool trieinsert(trienode ** , char *) ; 
bool isintrie(trienode **, char *) ; 

void printtrie_rec(trienode *, unsigned char * , int);

void printtrie(trienode *);

void generate_letters(char [4][4], int );

void load_trie_fromdict(trienode ** , char * );

//dato che devo essere in grado di leggere dallo stesso file in sequenza matrici diverse apro il file prima e passo il fd
void load_matrix_fromfile(char [4][4] , FILE *) ;

void print_matrix(char [4][4]);

bool adjacentSearch(char [4][4], const char *, int , int , int );

bool isinmatrix(char [4][4], const char *);

char * matrix_to_char(char [4][4]);

