#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#include "./libraries/macro.h"  
// #include "./libraries/Queue.h"  
#include "./libraries/Queue.c"  

#define for_all_matrix( command )\
  for(int r=0;r<4;r++){for(int c=0;c<4;c++){command;}}

//Macro per convertire il carattere in un index, uso i caratteri da a -> z
#define char_int(c) ((int)c - (int)'a')

//Definisco la size dell'alfabeto
#define NUM_CHAR (256)



//Controllo se la posizione [row,col] e' nel range della matrice e se non e' ancora stato visitato
bool isSafe(int row, int col, bool visited[4][4]){
  return (row >= 0 && col >= 0 && row < 4 && col < 4 && !visited[row][col]);
}

typedef struct trienode {
  struct trienode *children[NUM_CHAR];
  bool terminal ; 
} trienode ; 

trienode *createnode(){
  trienode *newnode = malloc(sizeof(*newnode));
  
  for(int i = 0 ; i < NUM_CHAR ; i++){
    newnode->children[i] = NULL;
  }
  newnode->terminal = false ; 
  return newnode ; 
}

bool trieinsert(trienode ** root , char *signedtext){
  if(*root == NULL)
    *root = createnode() ;

  unsigned char *text = (unsigned char*)signedtext ;
  trienode *temp = *root;
  int length = strlen(signedtext);

  for(int i=0 ; i < length ; i++){
    if(temp->children[text[i]] == NULL){
      //creo un nuovo nodo
      temp->children[text[i]] = createnode();
    }
    temp = temp->children[text[i]];
  }
  if(temp->terminal){
    return false;
  } else {
    temp->terminal = true ; 
    return true ; 
  }
}

bool isintrie(trienode ** root, char *signedtext){
  if(*root == NULL) return false ; 

  unsigned char *text = (unsigned char*)signedtext ;
  trienode *temp = *root;
  int length = strlen(signedtext);

  for(int i=0 ; i < length ; i++){
    if(temp->children[text[i]] == NULL){
      // se non ho nulla vuol dire che la parola non c'e'
      return false ;
    }
    temp = temp->children[text[i]];
  }
  if(temp->terminal){
    return false;
  } else {
    return true ; 
  }
}

void printtrie_rec(trienode *node , unsigned char * prefix, int length){
  unsigned char newprefix[length + 2];
  memcpy(newprefix , prefix , length);
  newprefix[length+1] = 0 ;

  if(node->terminal){
    printf("WORD: %s\n", prefix);
  }
  for(int i=0 ; i < NUM_CHAR ; i++){
    if(node->children[i] != NULL){
      newprefix[length] = i ; 
      printtrie_rec(node->children[i] , newprefix , length+1);
    }
  }
}

void printtrie(trienode * root){
  if(root == NULL){
    return ;
  }

  printtrie_rec(root , NULL , 0);
}

void generate_letters(char M[4][4], int seed) {
  srand(seed);

  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      // Prendo un numero tra 0 e 26
      int random_number = rand() % 26;

      // aggiungendolo al carattere a ottengo lettere random da a -> z
      char lettera = 'a' + random_number;
      M[r][c] = lettera ; 
    }
  }
}

void load_trie_fromdict(trienode ** root , char * filename){
  FILE * dict_file ; 
  SYSCN(dict_file , fopen(filename , "r") , "nella fopen");

  char * buffer = malloc(sizeof(char) * 50);
  while(!feof(dict_file)){
    fgets(buffer , sizeof(char) * 50 , dict_file);
    // printf("read from file : %s" , buffer);
    trieinsert(root , buffer);
  }
  int * rv; 
  fclose(dict_file) ; 
}

//dato che devo essere in grado di leggere dallo stesso file in sequenza matrici diverse apro il file prima e passo il fd
void load_matrix_fromfile(char M[4][4] , FILE * fd){
  char ch ;
  char buffer[40];
  fgets(buffer , sizeof(char) * 40 , fd);
  // printf("buffer : %s" , buffer);

  int char_read = 0 ; 
  int index = 0 ;
  while(char_read < 16){
    ch = tolower(buffer[index]) ; 
    index++ ; 
    // printf("%c" , tolower(ch));
    int row = char_read % 4 ; 
    int col = char_read - 4*(char_read % 4) ; 
    if(ch == 'q'){
      M[row][col] = 'q';
      index++ ; 
      char_read++ ; 
    } else if(ch != ' '){
      M[row][col] = ch;
      char_read++ ; 
    }
  }
}

void print_matrix(char M[4][4]) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      if(M[r][c] == 'q'){
        printf("|qu");
      }else{
        printf("|%c", M[r][c]);
      }
    }
    printf("|\n");
  }
}

// dichiaro dei vettori direzione
int dRow[] = { -1, 0, 1, 0 };
int dCol[] = { 0, 1, 0, -1 };


bool adjacentSearch(char matrix[4][4], const char *find, int i, int j, int index) {
    if (find[index] == '\0')
        return true;

    if (i < 0 || j < 0 || i >= 4 || j >= 4 || matrix[i][j] != find[index])
        return false;
    
    // printf("exploring cella [%d,%d]: %c\n" , i , j , find[index]);
    matrix[i][j] = '*';
    bool found = (adjacentSearch(matrix, find, i + 1, j, index + 1) ||
                  adjacentSearch(matrix, find, i - 1, j, index + 1) ||
                  adjacentSearch(matrix, find, i, j - 1, index + 1) ||
                  adjacentSearch(matrix, find, i, j + 1, index + 1));
    matrix[i][j] = find[index];
    return found;
}

bool isinmatrix(char matrix[4][4], const char *find) {
    // dato che la q non puo' essere senza q, tolgo la u dalla parola da cercare dentro la matrice
    int len = strlen(find);
    char * newstring = malloc(sizeof(char) * 20);
    int qus = 0 ;
    for(int i = 0 ; i < len ; i++){
      if(find[i] != 'q'){
        newstring[i - qus] = find[i];
      }else{
        newstring[i-qus] = find[i];
        qus++ ; 
        i++;
      }
    }

    // printf("newstring : %s\n" , newstring);

    if (len > 4 * 4)
        return false;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (adjacentSearch(matrix, newstring, i, j, 0)) {
              // printf("trovato un match per %s!\n" , newstring);
              return true;
            }
        }
    }
    return false;
}


int main(int argc, char *argv[]) {
  // definisco la variabile matrice 4 x 4 x 3 (dua char + null terminator)
  char M[4][4];

  // genero la matrice
  // generate_letters(M, 1);
  // print_matrix(M);
  // generate_letters(M, 2);
  // print_matrix(M);

  FILE * matrix_file ;
  char * filename = "matrix.txt" ; 
  SYSCN(matrix_file , fopen(filename , "r") , "nella fopen");
  load_matrix_fromfile( M , matrix_file) ; 
  print_matrix(M) ;

  fclose(matrix_file);   

  trienode * root = NULL;

  load_trie_fromdict(&root , "dict.txt");

  printf("%d %d" , isintrie(&root , "quanto") ,isinmatrix(M , "quanto")) ; 
  
  
  if(isinmatrix(M , "quanto")){
    printf("it fucking works3\n");
  }
  printf("%d" , isintrie(&root , "quanto") );

  if(isintrie(&root , "quanto")){
    printf("it fucking works4\n");
  }

  return 0;
} 