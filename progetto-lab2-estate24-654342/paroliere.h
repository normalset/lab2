#define MSG_OK 'K'
#define MSG_ERR 'E'
#define MSG_REGISTRA_UTENTE 'R'
#define MSG_MATRICE 'M'
#define MSG_TEMPO_PARTITA 'T'
#define MSG_TEMPO_ATTESA 'A'
#define MSG_PAROLA 'W'
#define MSG_PUNTI_FINALI 'F'
#define MSG_PUNTI_PAROLA 'P'
#define MSG_CLIENT_QUIT 'Q'
#define MSG_ALARM 'Z'
#define MSG_CLASSIFICA 'C'
#define MSG_PRINT_PROMPT 'L'

//Lunghezza massima del nome
#define MAX_NAME 11

//typedef messaggio , inviare solo i byte significativi del messaggio e non un biffer di lunghezza fissa
typedef struct messaggio {
    char type ; 
    unsigned int length ; //0 nel caso in cui il campo data non e' significativo
    char * data ; //dati effettivi
}messaggio ; 

/*
* Struttura dai per la coda dei dati di ogni giocatore
*/
typedef struct player{
    int client_fd ;
    char name[11] ;  
    int score; 
    int words_index ; 
    char words_used[100][17] ; 

    struct player *next ; 
}Player ; 


/* 
 * Return 1 se c'e' un utente con questo nome nella lista
 * Return 0 se non c'e' nessun utente con quel nome
 */
int search_player(const char *, Player *) ; 

/*
 * Crea un nuovo utente con quel nome e aggiungilo alla fine della lista.
 * Return:
 *   0 se viene aggiunto
 *   1 se il nome e' gia' usato
 *   2 se il nome e' troppo lungo
 */
int add_player(const char *, Player **);


/* 
 * Return un puntatore la giocatore con quel nome nella lista
 * Return NULL se non trova nulla.
 */
Player *find_player(const char *, const Player *) ; 


/*
 * Printa la lista dei giocatori con alcune statistiche.
 */
void list_players(const Player *) ;


/*
 * Cerca e elimina un giocatore dalla lista dei giocatori
 *
 * Return:
 *   - 0 se lo cancella
 *   - 1 se non trova il giocatore nella lista
 */
int delete_player(int , Player * );


/*
 * Trova un giocatore e aggiorna il suo score
 *
 * Return:
 *   - 0 se ha funzionato
 *   - 1 se non trova il giocatore
 */
int add_score(char *, int , Player *);


/*
 * Calcola e return la lunghezza della lista dei giocatori
 */
int plist_length(Player *);


/*
* Resetta a 0 l'indice del numero di parole utilizzate dai diversi giocatori
*/
void clear_players_data(Player *);

/*
* Funzione per scrivere un messaggio su una socket tramite il protocollo tipo-length-data
* ARGS : 
* - int socket file descriptor
* - char tipo messaggio
* - char * data
*/
void write_message(int socket_fd , char type , char * );

/*
* Funzione per leggere un messaggio da una socket e restituire una struct messaggio con tipo-length-data
*/
messaggio read_message(int );

/*
 * Versione senza print della write_message per il client
 */
void silent_write_message(int socket_fd, char type, char *);

/*
 * Versione senza print della read_message per il client 
 */
messaggio silent_read_message(int);

