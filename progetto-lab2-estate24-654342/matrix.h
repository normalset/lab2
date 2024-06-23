#include <stdbool.h>
#include <string.h>
#include <stdio.h>

//Definisco la size dell'alfabeto
#define NUM_CHAR (256)

typedef struct trienode {
  struct trienode *children[NUM_CHAR];
  bool terminal ; 
} trienode ; 

//Funzione per creare un nodo all'interno della Trie
trienode *createnode(void) ;

/*
 * Funzione per inserire una stringa nella trie.
 * Return True se la parola viene inserita, false se c'e' gia' 
 */
bool trieinsert(trienode ** , char *) ;

/*
 * Funzione per controllare una stringa nella trie.
 * Return True se la parola e' presente, false se non c'e'.
 */
bool isintrie(trienode **, char *) ; 

void printtrie_rec(trienode *, unsigned char * , int);

void printtrie(trienode *);

/*
* Carica su una trie un dizionario(da un file parola \n parola ...).
* ARGS : head della trie , nome del file 
*/
void load_trie_fromdict(trienode ** , char * );

/*
* Genera randomicamente con il seed una matrice 4x4 di caratteri
*/
void generate_letters(char [4][4], int );

/*
* Carica una matrice da un file
* ARGS : matrice di gioco  , file descriptor del file contenente le matrici.
* Dato che devo essere in grado di leggere dallo stesso file in sequenza matrici diverse apro il file prima e passo il fd
*/
void load_matrix_fromfile(char [4][4] , FILE *) ;

//* Stampa a video la matrice 
void print_matrix(char [4][4]);

bool adjacentSearch(char [4][4], const char *, int , int , int );

bool isinmatrix(char [4][4], const char *);

//* Converto la matrice in una lista di caratteri
char * matrix_to_char(char [4][4]);

