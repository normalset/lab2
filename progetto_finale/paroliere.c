#include "paroliere.h"
#include "macro.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* 
 * Return 1 se c'e' un utente con questo nome nella lista
 * Return 0 se non c'e' nessun utente con quel nome
 */
int search_player(const char *name, Player *head){
    printf("[ ] Searching for player : %s\n" , name);
    Player *current = head;
    while (current != NULL){
        if(strcmp(current->name, name) == 0){
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/*
 * Crea un nuovo utente con quel nome e aggiungilo alla fine della lista.
 * Return:
 *   0 se viene aggiunto
 *   1 se il nome e' gia' usato
 *   2 se il nome e' troppo lungo
 */
int add_player(const char *name, Player **head_player_ptr){
    printf("Trying to add player : %s \n" , name) ; 
    if (strlen(name) >= MAX_NAME) {
        return 2;
    }
    if(search_player(name, *head_player_ptr) == 1){
        return 1;
    }
    else{
        Player *new_player = malloc(sizeof(Player));
        if (new_player == NULL) {
            perror("nella malloc");
            exit(1);
        }
        strcpy(new_player->name, name);
        new_player->score = 0;
        new_player->next = NULL;  
        if(*head_player_ptr == NULL){
            *head_player_ptr = new_player;
            new_player->next = NULL;
        }else{
            new_player->next = *head_player_ptr;
            *head_player_ptr = new_player;
        }
        return 0;
    }
}

/* 
 * Return un puntatore la giocatore con quel nome nella lista
 * Return NULL se non trova nulla.
 */
Player *find_player(const char *name, const Player *head) {

    while (head != NULL && strcmp(name, head->name) != 0) {
        head = head->next;
    }

    return (Player *)head;
}

/*
 * Printa la lista dei giocatori con alcune statistiche.
 */
void list_players(const Player *curr) {
    printf("Player List\n");
    while (curr != NULL) {
        printf("\t%s score : %d fd : %d\n",curr->name, curr->score, curr->client_fd);
        curr = curr->next;
    }
}

/*
 * Trova un giocatore e aggiorna il suo score
 *
 * Return:
 *   - 0 se ha funzionato
 *   - 1 se non trova il giocatore
 */
int add_score(char *name, int score, Player *player_list){
    // Player *player = find_player(name, player_list);
    Player * curr = player_list ; 
    while(curr != NULL && strcmp(curr->name , name) != 0){; 
        curr = curr->next ; 
    }
    if(curr == NULL){
        return 1;
    } 
    curr->score += score;
    return 0;
}

/*
 * Calcola e return la lunghezza della lista dei giocatori
 */
int plist_length(Player *head){
    Player *cur = head;
    int counter = 0;
    while (cur) {
        cur = cur->next;
        counter++;
    }
    return counter;
}

/*
* Resetta a 0 l'indice del numero di parole utilizzate dai diversi giocatori
*/
void clear_players_data(Player * curr){
    while (curr != NULL) {
        curr->words_index = 0 ;
        curr->score = 0 ; 
        curr = curr->next;
    }
}

void write_message(int socket_fd , char type , char * data){
  //calcolo la length
  int length ; 
  if(data == NULL){
    length = 0 ; 
  }else {
    length = strlen(data) ;
  }
  //formatto il testo in csv
  char buffer[100] ; 
  sprintf(buffer , "%c,%d,%s" , type , length , data);
  //scrivo sulla socket
  int rv ; 
  SYSC(rv , write(socket_fd , buffer , sizeof(buffer)) , "nella write_message" ) ;
  printf("[ ] Wrote Message : %c %d %s\n" , type , length , data) ; 
}

messaggio read_message(int socket_fd){
  messaggio msg ; 
  char buffer[100];
  int rv ; 
  SYSC(rv , read(socket_fd , buffer , sizeof(buffer)), "nella read_message");
  //se il messaggio ha length == 0 prendo solamente il tipo del messaggio senza tokenizzarlo 
  if(buffer[2] == '0'){
    msg.type = buffer[0] ;
    msg.length = 0 ;
    msg.data = NULL ;
  } else { // se ha length tokenizzo prendendo i 3 campi separati dalla ","
    char * token ;
    token = strtok(buffer , ",");
    msg.type = *token;
    token = strtok(NULL , ",");
    msg.length = atoi(token);
    token = strtok(NULL , ",");
    msg.data = malloc(sizeof(char) * (msg.length + 1));
    strcpy(msg.data , token) ;
  }
  printf("[ ] Read Message : %c %d %s\n" , msg.type , msg.length , msg.data) ; 
  return msg ;
}
