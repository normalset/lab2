#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//numero di caratteri accettati dalla lookup-table children
#define NUM_CHAR 256

typedef struct trienode{
  struct trinode * children[NUM_CHAR];
  bool terminal ;
} trienode ; 

trienode *createnode(void);

bool trieinsert(trienode ** , char *);

bool isintrie(trienode **, char *);

void printtrie_rec(trienode * , unsigned char * , int);

void printtrie(trienode *);

void generate_letters(char [4][4], int);

void load_trie_fromdict(trienode **, char *);

void load_matrix_fromfile(char [4][4] , FILE *);

bool adjacentSearch(char [4][4], const char *, int , int , int);

bool isinmatrix(char [4][4], const char *);
