# punto 1
matrice quadrata 4x4 di 16 lettere
ogni parola e; akmeno 4 caratteri, composta usando le lettere una sola volta
la parola puo' muoversi in orizzontale e verticale, ma non orizzontale

# Link documentazione
trie data structure
https://www.baeldung.com/cs/finding-words-in-letter-matrix
boogle solver
https://github.com/thomasweng15/boggle-solver/blob/master/boggle.c


## SERVER
- Richiesta di connessione da un client -> thread dedicato alla gestione del client
  - Thread attivo fino alla disconnessione del client

- Client registra il proprio nome , che `deve essere nuovo` , msg tipo MSG_REGISTRA_UTENTE
  - Server -> MSG_OK se nome libero
  - Server -> MSG_ERR se occupato , rimane in attesa del nuovo nome

- Aggiungere client ai giocatori

- Comincare al client la matrice di caratteri con `MSG_MATRICE` e la durata residua nella partita con `MSG_TEMPO_PARTITA` o per la prossima con `MSG_TEMPO_RESTANTE`

- Ricevere i messaggi `MSG_PAROLA`
  -> controllare la correttezza 
    -> punteggio al client (1 per lettera, Qu vale come 1) con `MSG_PUNTI_PAROLA`
    -> se parola sbagliata `MSG_ERR``
    -> dare punti solo la prima volta che la parola viene mandata per client, rispondere con `MSG_PUNTI_PAROLA` com 0 punti

- Pausa di `1 Minuto` tra partite, durante la pausa rispondere solo a
  - `MSG_REGISTRA_UTENTE` 
  - `MSG_MATRICE` (che in questo caso determina soltanto la comunicazione del `MSG_TEMPO_ATTESA`, 
  - `MSG_PUNTI_FINALI`.

- Al termine di una partitta ogni thread dei client spedisce un messaggio contenente il punteggio su una coda condivisa tra i diversi thread

- Scorer thread recupera i messaggi dalla coda e sgtile la classifica del gioco, nominando il vincitore.
  - comincare agli altri thread la classifica tramite `msg_punti_finali` in formato csv username,punto,username ... \0

- Ogni giocatore conessio va considerato partecipante alla partita successiva

- Se il client si disconnette, cancellare il giocatore dal gioco corrente e chiudere il thread -> fare polling?

- chiudere server con CTRL-C


# todo
semaforo per lettura e scrittura del messaggio


#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>

//parsing dei parametri
#include <getopt.h>

//librerie custom
#include "matrix.c"

//defines
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 32

//defines tipi di messaggi
#define MSG_OK 'K'
#define MSG_ERR 'E'
#define MSG_REGISTRA_UTENTE 'R'
#define MSG_MATRICE 'M'
#define MSG_TEMPO_PARTITA 'T'
#define MSG_TEMPO_ATTESA 'A'
#define MSG_PAROLA 'W'
#define MSG_PUNTI_FINALI 'F'
#define MSG_PUNTI_PAROLA 'P'
#define MSG_SEMAFORI 'S'
#define MSG_SKIP 'Z'
// variabili globali
char game_matrix[4][4];
int using_matrixfile = 0 ; 
trienode * dict_trie = NULL ;
int using_dict = 0 ; 
sem_t message_sem_out ;
sem_t message_sem_in ;

//typedef messaggio , unviare solo i byte significativi del messaggio e non un biffer di lunghezza fissa
typedef struct messaggio {
    char type ; 
    unsigned int length ; //0 nel caso in cui il campo data non e' significativo
    char * data ; //dati effettivi
}messaggio ; 

typedef struct player{
    int client_fd ;
    char name[11] ;  
    int score ; 

    struct player *next ; 
}Player ; 

Player * players_list ; 

#define MAX_NAME 11 

/* 
 * Return 1 if the player with this name in
 * the list. Return 0 if no such player exists.
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
 * Create a new player with the given name.  Insert it at the tail of the list
 * of players whose head is pointed to by head_player_ptr_ptr.
 *
 * Return:
 *   0 if successful
 *   1 if a player by this name already exists in this list
 *   2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int add_player(const char *name, Player **head_player_ptr_ptr){
    printf("Trying to add player : %s \n" , name) ; 
    if (strlen(name) >= MAX_NAME) {
        return 2;
    }
    // if(search_player(name, *head_player_ptr_ptr) == 1){
    //     return 1;
    // }
    else{
        Player *new_player = malloc(sizeof(Player));
        if (new_player == NULL) {
            perror("nella malloc");
            exit(1);
        }
        strcpy(new_player->name, name);
        printf("trying to copy name : %s" , new_player->name);
        new_player->score = 0;
        new_player->next = NULL;  
        if(*head_player_ptr_ptr == NULL){
            *head_player_ptr_ptr = new_player;
            new_player->next = NULL;
        }else{
            new_player->next = *head_player_ptr_ptr;
            *head_player_ptr_ptr = new_player;
        }
        return 0;
    }
}

/* 
 * Return a pointer to the player with this name in
 * the list starting with head. Return NULL if no such player exists.
 *
 * NOTE: You'll likely need to cast a (const Player *) to a (Player *)
 * to satisfy the prototype without warnings.
 */
Player *find_player(const char *name, const Player *head) {

    while (head != NULL && strcmp(name, head->name) != 0) {
        head = head->next;
    }

    return (Player *)head;
}

/*
 * Print the playernames of all players in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
void list_players(const Player *curr) {
    printf("Player List\n");
    while (curr != NULL) {
        printf("\t%s\n",curr->name);
        curr = curr->next;
    }
}

/*
 * Finds and deletes a player from the list
 *
 * Return:
 *   - 0 on success
 *   - 1 if player is not in the list
 */
int delete_player(char *name);

/*
 * Finds the player and updates player score
 *
 * Return:
 *   - 0 on success
 *   - 1 if player is not in the list
 */
int add_score(char *name, int score, const Player *player_list){
    Player *player = find_player (name, player_list );
    if (player == NULL){
        return 1;
    } 
    player->score += score;
    return 0;
}

/*
 * Computes the length of the player list
 * 
 * Return: integer length of the list
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

void write_message(int client_fd , sem_t sem_in ,sem_t sem_out , char type , char* data){
    int rv ; 
    int length = strlen(data) * sizeof(char) ; 

    sem_wait(&message_sem_out);
    SYSC(rv, write(client_fd, &length, sizeof(int)), "nella write");
    sem_post(&message_sem_in); 

    sem_wait(&message_sem_out);
    SYSC(rv, write(client_fd, &type, sizeof(char)), "nella write");
    sem_post(&message_sem_in);
    
    if(length != 0){
        sem_wait(&message_sem_out);
        SYSC(rv, write(client_fd, data, length), "nella write");
        sem_post(&message_sem_in);
    }
}

messaggio read_message(sem_t sem_in , sem_t sem_out , int client_fd ){
    int rv ; 
    char databuffer[50] ;
    messaggio msg ; 
    sem_wait(&message_sem_in); 

    SYSC(rv ,read(client_fd , &(msg.length) , sizeof(int)) , "nella read length");
    // printf("Recieved size %d\n" , msg.length) ; 
    sem_post(&message_sem_out);

    sem_wait(&message_sem_in); 
    SYSC(rv ,read(client_fd , &(msg.type) , sizeof(char)) , "nella read char");
    // printf("Recieved type %c\n" , msg.type) ; 
    sem_post(&message_sem_out);

    if(msg.length != 0){
        sem_wait(&message_sem_in); 
        SYSC(rv ,read(client_fd , &databuffer , sizeof(databuffer)) , "nella read string");
        msg.data = malloc(sizeof(char) * msg.length) ; 
        strncpy(msg.data , databuffer, msg.length);
        // printf("Recieved data %s\n" , msg.data) ; 
        sem_post(&message_sem_out);
    }
    // printf("Read : %d , %c , %s\n", msg.length , msg.type , msg.data);
    return msg ; 
}

void* client_handler(void* args){
    printf("Client handler here\n");
    ssize_t n_read ; 
    int rv ; 
    int client_fd = *((int*)args) ;
    printf("Client fd : %d\n" , client_fd); 

    Player * profile = malloc(sizeof(Player));
    profile->client_fd = client_fd ; 

    //main loop
    messaggio msg ; 
    while(1){
        msg = read_message(message_sem_in , message_sem_out , client_fd) ; 
        // printf("Got : %d , %c , %s\n", msg.length , msg.type , msg.data);
        switch(msg.type){
            case MSG_SKIP:
                continue;
            case MSG_REGISTRA_UTENTE:
                if(!(add_player(msg.data , &players_list) == 0)){
                    //mando meggaggio di errore al client
                }
                //mando la matrice e la durata residua della partita in corso
                char * matrix_buff  = matrix_to_char(game_matrix) ; 
                write_message(client_fd , message_sem_in , message_sem_out , MSG_MATRICE , matrix_buff) ; 
                free(matrix_buff);
            case MSG_PAROLA :
                if(isinmatrix(game_matrix , msg.data) && isintrie(&dict_trie , msg.data)){
                    //todo punti vengono attribuiti solamente la prima volta
                    //todo nel punteggio Qu vale solo come q
                    add_score(profile->name , strlen(msg.data), players_list);
                    //converto la lunghezza a stringa per mandarla con il messaggio
                    char score_buffer[10];
                    sprintf(score_buffer , "%lu" , strlen(msg.data));
                    write_message(client_fd , message_sem_in , message_sem_out , MSG_PUNTI_PAROLA, score_buffer);
                } else {
                    write_message(client_fd , message_sem_in , message_sem_out , MSG_ERR , NULL) ;
                }

        }
        msg.type = 'Z' ;  
    }

    // messaggio msg  = read_message(message_sem_in , message_sem_out , client_fd );

    return NULL ; 
}


void client(){
    printf("Client here\n");
    srand(4);
    int client_fd , rv ; 
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // creazione del socket
    SYSC(client_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");

    // inizializzazione struttura server_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(200);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 127.0.0.1 usato per comunicare sulla stessa macchina come indirizzo di loopback

    // connect
    SYSC(rv, connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)), "nella connect");

    write_message(client_fd , message_sem_in , message_sem_out , MSG_REGISTRA_UTENTE , "ciao") ; 

    // close socket
    SYSC(rv, close(client_fd), "nella close");
}



int main(int argc , char * argv[]){
    if(argc < 3){
        perror("Not enough args");
        exit(EXIT_FAILURE);
    }
    
    int rv;

    //server name
    char *server_name = malloc(sizeof(char) * 25) ;
    strcpy(server_name , argv[1]);
    //server port
    int server_port = atoi(argv[2]);

    //args opzionali con valori di default
    char matrix_filename[50] ;  
    int duration = 3 * 60 * 60; //durata in secondi
    int seed =-1;
    char dictionary_file[50] = "dict.txt" ;  // da cambiare al nome del dizionario di base  

    struct option long_options[] = {
        {"matrici" , required_argument , NULL , 'm' },
        {"durata" , required_argument , NULL , 't'}, //t : time
        {"seed" , required_argument , NULL , 's'},
        {"diz" , required_argument , NULL , 'd'},
        {NULL , 0 , NULL} 
    };

    while((rv = getopt_long(argc, argv, "m:t:s:d:" ,long_options , NULL)) != -1) {
        printf("test %c\n", rv); 

        switch (rv) {
            case 'm':
                using_matrixfile = 1 ; 
                strcpy(matrix_filename , optarg);

                printf("Loading custom matrix from : %s\n" , matrix_filename) ; 
                FILE * matrix_fd = fopen(matrix_filename , "r");
                if(matrix_fd){
                    load_matrix_fromfile(game_matrix , matrix_fd) ; 
                }else{
                    perror("Error while loading custom matrix file\n");
                    exit(EXIT_FAILURE);
                }
                fclose(matrix_fd);
            break;
            case 't':
                duration = atoi(optarg); // imposto la durata delle partite
            break;
            case 's':
                seed = atoi(optarg) ; 
            break;
            case 'd':
                using_dict = 1 ;
                strcpy(dictionary_file , optarg); 
            break;
            //caso in cui ho un tag che non e' corretto
            case '?':
                printf("Unknown option: %c\n", optopt);
            return 1;
            default:
                printf("Invalid option\n");
            return 1;
        }
    }

    // Controllo che almeno u due parametri indispensabili siano stati definiti
    if (server_name == NULL || server_port == -1) {
        printf("Error: Missing mandatory options (-f server_name, -p port_number)\n");
        return 1;
    }

    // Debug
    printf("*** Data for server ***\n"); 
    printf("Server name: %s\n", server_name);
    printf("Server port: %d\n", server_port);
    printf("Using custom matrix filename: %d\n", using_matrixfile);
    printf("Duration: %d seconds (default if not provided)\n", seed);
    printf("Seed: %d (default if not provided)\n", (seed == -1) ? 0 : seed);
    printf("Dictionary file: %s\n", dictionary_file);

    //load dictionary to server usando la trie se sto usando un dizionario
    load_trie_fromdict(&dict_trie , dictionary_file) ;  
        
    //se non ho caricato la matrice dal file la genero adesso
    if(!using_matrixfile){
        generate_letters(game_matrix , seed);
    }

    //inizializzo i semafori
    sem_init(&message_sem_in , 0 , 1) ; 
    sem_init(&message_sem_out , 1 , 1) ; 

    //print per debug della matrice in uso
    printf("Starting Matrix:\n");
    print_matrix(game_matrix);

    //apertura del socket
        //varibili
    int server_fd , client_fd; 
    struct sockaddr_in server_addr , client_addr ; 
    socklen_t client_addr_len ; 
    char buffer[BUFFER_SIZE] ; 

    //creazione del socket
    SYSC(server_fd, socket(AF_INET , SOCK_STREAM , 0), "nella socket call");

    //inizializzazione della struttra server_addr
    server_addr.sin_family = AF_INET ;
    server_addr.sin_port = htons(server_port) ;
    server_addr.sin_addr.s_addr = INADDR_ANY ;

    //binding
    SYSC(rv , bind(server_fd , (struct sockaddr *)&server_addr , sizeof(server_addr)) , "nella bind");

    //listening
    SYSC(rv , listen(server_fd , MAX_CLIENTS), "nella listen");

    //testing client
    pid_t pid ; 
    SYSC(pid , fork() , "nella fork");
    if(pid == 0){
        client();
    }else{
        while(1){

        SYSC(client_fd, accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len), "nella accept");

        printf("New Client connected %d!\n" , client_fd); 

        pthread_t client_tid ; 
        if(pthread_create(&client_tid , NULL , &client_handler , (void*)&client_fd) != 0){
            perror("Errore nella pthread create");
            close(client_fd) ;
            continue ; 
        }
        }
    }

    exit(EXIT_SUCCESS);
}
