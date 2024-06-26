#define _XOPEN_SOURCE 600
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
#include <signal.h>

//parsing dei parametri
#include <getopt.h>

//librerie custom
#include "libraries/matrix.c"
#include "libraries/paroliere.c"


//defines
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 32


// variabili globali  
char game_matrix[4][4];
int using_matrixfile = 0 ; 
FILE * matrix_fd ; 
trienode * dict_trie = NULL ;
int using_dict = 0 ;
int seed =-1;
int duration = 3 * 60 * 60; //durata di 3 minuti standard in secondi
char leaderboard[2048] ; 

int gamestate = 0 ; // 0 playing | 1 pause;

Player * players_lis_ptr ; 

//mutexes per le var globali
pthread_mutex_t matrix_mux = PTHREAD_MUTEX_INITIALIZER ; 
pthread_mutex_t players_mux = PTHREAD_MUTEX_INITIALIZER ;
pthread_mutex_t alarm_mux = PTHREAD_MUTEX_INITIALIZER ; 
pthread_mutex_t leaderboard_mux = PTHREAD_MUTEX_INITIALIZER ; 


// barriera
pthread_barrier_t barrier;

/*
* Restituisce il tempo rimanente del timer e lo salva come stringa in buff
*/
void get_alarm_time(char * timebuffer){
    int timer_left = alarm(0);
    alarm(timer_left); // prendo il tempo rimanente resettando con alarm(0) e faccio ripartire un nuovo alarm con il tempo rimanente
    sprintf(timebuffer, "%d", timer_left);
}

/*
 * Cerca e elimina un giocatore dalla lista dei giocatori
 *
 * Return:
 *   - 0 se lo cancella
 *   - 1 se non trova il giocatore nella lista
 */
int delete_player(int fd , Player * players_list_head) {
    Player * curr = players_list_head ; 
    Player * prev = NULL ;
     
    // Handle head node
    if (curr != NULL && curr->client_fd == fd) {
        players_lis_ptr = curr->next;
        free(curr);
        return 0;
    }

    //else cerco il giocatore
    while (curr != NULL && curr->client_fd != fd) {
        prev = curr; // Update prev before advancing curr
        curr = curr->next;
    }
    //se non trovo il player
    if(curr == NULL){
        return 1 ;
    }
     // Deletion
    prev->next = curr->next;
    free(curr);
    return 0; 
}

/*
* Funzione per controllare se una parola e' goa' stata usata da un determinatio client durante una partita
* ARGS : lista di parole usate , index della lista di parole , parola da controllare
*/
bool is_word_used(char words_used[100][17] ,int words_index , char * word){
    for(int i = 0 ; i < words_index ; i++){
        if(strcmp(words_used[i] , word) == 0) return true ;
    }
    return false ; 
}

void* client_handler(void* args){
    printf("Client handler here\n");
    int rv ; 
    int client_fd = *((int*)args) ;

    Player * profile ; 
    int logged = 0 ; 
    //main loop
    messaggio msg ; 
    while(1){
        printf("---------------------\n");
        msg = read_message(client_fd) ; 

        //* REGISTRAZIONE
        if(msg.type == MSG_REGISTRA_UTENTE){
            rv = mux_f(add_player(msg.data , &players_lis_ptr) , players_mux) ; 
            if(rv == 0 ){ //se posso aggiungerlo alla lista dei giocatori
                pthread_mutex_lock(&players_mux) ; //blocco la lista mentre faccio le operazioni
                    write_message(client_fd , MSG_OK , NULL) ; 
                    printf("[ ] Added player %s\n" , msg.data) ;
                    list_players(players_lis_ptr) ; 
                    profile = find_player(msg.data , players_lis_ptr);
                    strcpy(profile->name , msg.data) ;  
                    profile->words_index = 0  ; //index dell'ultima parola aggiunta
                    profile->client_fd = client_fd ; 
                    profile->score = 0 ;
                    pthread_mutex_init(&(profile->p_mux), NULL); //inizializzo il suo mux personale
                    logged = 1 ;
                pthread_mutex_unlock(&players_mux);
            } else if(rv == 1) {
                mux_f(write_message(client_fd , MSG_ERR , "Chose another name"), profile->p_mux);
            } else {
                mux_f(write_message(client_fd , MSG_ERR , "Name Too Long"), profile->p_mux);
            }
        }

        //* RICHIESTA MATRICE 
        if(msg.type == MSG_MATRICE){
            if(gamestate == 0 ){ //mando la matrice di gioco e il tempo rimanente
                //matrice
                char * buf = malloc(sizeof(char) * 32) ; 
                int index = 0;
                mux_f(for_all_matrix( buf[index++] = game_matrix[r][c]), matrix_mux) ; 
                // Add the null terminator at the end of the string
                buf[index] = '\0';
                //write message
                mux_f(write_message(client_fd , MSG_MATRICE , buf), profile->p_mux) ;
                free(buf); 
            }else{
                //se sono in pausa devo solo mandare il tempo di attesa
                msg.type = MSG_TEMPO_PARTITA ; 
            }
        }

        //* MESSAGGIO DI RICHIESTA TEMPO
        if(msg.type == MSG_TEMPO_PARTITA){
            int autom = 0 ;
            if(gamestate == 0){
                if(strcmp(msg.data , "!") == 0 ){
                    autom = 1 ; 
                }
                pthread_mutex_lock(&alarm_mux);
                    char timebuffer[32];
                    //controllo se la richiesta dal server e' stata dell'utente o automatica
                    if(autom){ 
                        timebuffer[0] = '!';
                        get_alarm_time(timebuffer+1);
                    }else{
                        get_alarm_time(timebuffer);
                    }
                pthread_mutex_unlock(&alarm_mux);
                
                mux_f(write_message(client_fd, MSG_TEMPO_PARTITA, timebuffer), profile->p_mux);
            }else{ // mando solo il tempo di attesa rimanente
                char timebuffer[32];
                mux_f(get_alarm_time(timebuffer), alarm_mux) ; 
                mux_f(write_message(client_fd, MSG_TEMPO_ATTESA, timebuffer) , profile->p_mux);
            }
        }

        /*
         * PROPOSTA PAROLA | Parola giusta -> controllo se gia' stata usata -> messaggio punti | Parola sbagliata -> messaggio errore
         * Il check sulle parole con len<4 e' gia' stato fatto dal client 
         */
        if(msg.type == MSG_PAROLA){
            //se il gamestate = 1 non posso proporre parole quindi salto
            if(gamestate == 1){
                mux_f(write_message(client_fd , MSG_ERR , "Gioco in pausa, non si possono proporre parole"), profile->p_mux); 
            }else{
                //controllo se la parola e' gia' stata usata

                //lock sul mutex del profilo per non fare aggiungere dati nuovi mentre controlla, non blocco la matrice perche' il cambio viene fatto quando il comando parola non funziona nella pausa
                pthread_mutex_lock((&(profile->p_mux))) ; 

                if( ! is_word_used(profile->words_used , profile->words_index , msg.data)){
                    if(isinmatrix(game_matrix , msg.data) && isintrie(&dict_trie , msg.data)){
                        //copio la parola nella lista delle parole usate
                        strcpy(profile->words_used[profile->words_index] , msg.data);
                        profile->words_index++ ; 
                        //calcolo i punti della parola
                        int points = 0 ; 
                        for(int c = 0 ; c < strlen(msg.data) ; c++){
                            if(msg.data[c] != 'q') points++ ; 
                        }
                        char * points_buffer = malloc(sizeof(char) * 10) ; 
                        sprintf(points_buffer , "%d" , points) ; 
                        write_message(client_fd , MSG_PUNTI_PAROLA , points_buffer );
                        free(points_buffer) ;

                        // aggiungo i punti  allo score dell client
                        mux_f(add_score(profile->name , points , players_lis_ptr), profile->p_mux) ; 
                    }else{
                        write_message(client_fd , MSG_ERR , "Wrong choice :("); 
                    }
                }else{ //parola gia' stata usata, mando 0 punti
                    write_message(client_fd , MSG_PUNTI_PAROLA , "0"); 
                }
                pthread_mutex_unlock((&(profile->p_mux)));
            }
            
        }

        //* MESSAGGIO IN ARRIVO POST ALLARME DAL CLIENT
        if(msg.type == MSG_ALARM){
            //dopo aver mandato lo score resetto i dati del profilo
            //aspetto sulla barrier che tutti i messaggi siano stati mandati sulla queue (len players + 1 per lo scorer)
            printf("[ %s ] Waiting on barrier\n" , profile->name);
            pthread_barrier_wait(&barrier) ; 
            printf("[ %s ] Exited barrier\n" , profile->name);
            
            pthread_mutex_lock(&(profile->p_mux));
                write_message(client_fd , MSG_PUNTI_FINALI , leaderboard);
                profile->score = 0 ; 
                profile->words_index = 0 ;
            pthread_mutex_unlock(&(profile->p_mux));
        }

        //* MESSAGGIO DI CLASSIFICA
        if(msg.type == MSG_PUNTI_FINALI){
            if(gamestate == 0){
                mux_f(write_message(client_fd , MSG_ERR , "Classifica non disponibile, still playing."), profile->p_mux);
            } else {
                mux_f(write_message(client_fd , MSG_PUNTI_FINALI , leaderboard), profile->p_mux) ; 
            }
        }

        //* MESSAGGIO DI QUIT DAL CLIENT
        if(msg.type == MSG_CLIENT_QUIT){
            printf("[ %d ]Got a quit msg \n", client_fd); // uso il client_fd passato al thread perche' potrebbe non essere ancora stato fatto il login  
            //se non e' ancora neanche loggato chiudo il thread e basta, se e' loggato elimino il profilo dalla lista
            if( logged == 1 ){
                // delete player dalla player list bloccandola mentre lo faccio 
                pthread_mutex_lock(&players_mux);

                    printf("[ ] Player %d left\n", profile->client_fd);
                    pthread_mutex_destroy(&(profile->p_mux)) ; 
                    delete_player(profile->client_fd, players_lis_ptr);
                    list_players(players_lis_ptr);

                pthread_mutex_unlock(&players_mux);
            }
            close(client_fd);
            pthread_detach(pthread_self()) ; 
            return NULL;
        }
    }
    return NULL ; 
}

void * scorer(void * args){
    printf("[ ] scorer started\n");
    //blocco la lista dei giocatori
    pthread_mutex_lock(&players_mux);

    //lista dei client controllati
    int len = plist_length(players_lis_ptr);
    int scores[len] ; 
    char names[len][11] ; 

    int index = 0 ;
    Player * curr = players_lis_ptr;
    printf("List players in scorer thread: \n");
    list_players(players_lis_ptr) ; 

    while(curr != NULL){
        scores[index] = curr->score;
        strcpy(names[index] , curr->name);
        printf("[DEBUG] Writing msg to client %d\n", curr->client_fd) ; 
        mux_f(write_message(curr->client_fd , MSG_ALARM , NULL), curr->p_mux);
        curr = curr->next;
        index++ ; 
    }

    printf("Pre Sort Arrays:\n");
    for(int i = 0 ; i < len ; i++){
        printf("(%d) %s | %d", i , names[i] , scores[i]) ; 
        printf("\n") ;
    }

    //Bubble sort per nomi e scores
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (scores[j] < scores[j + 1]) {
                int temp_num = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp_num;

                char temp_name[11];
                strcpy(temp_name, names[j]);
                strcpy(names[j], names[j + 1]);
                strcpy(names[j + 1], temp_name);
            }
        }
    }

    //Post sort array print
    for(int i = 0 ; i < len ; i++){
        printf("Post Sort Arrays:\n");
        printf("(%d) %s | %d", i+1 , names[i] , scores[i]) ; 
        printf("\n") ;
    }

    //scrivo la leaderboard, lock mentre scrivo
    pthread_mutex_lock(&leaderboard_mux) ; 

    //svuoto la leaderboard prima di scriverla per cancellare i risultati vecchi
    memset(leaderboard , 0 , sizeof(leaderboard)) ; 

    for(int i = 0 ; i < len ; i++){
        char * buf = malloc(sizeof(char) * 32) ;
        sprintf(buf , "(%d) %s : %d;" , i+1 ,names[i], scores[i]); 
        printf("adding %s to the leaderboard\n", buf) ; 
        strcat(leaderboard , buf) ; 
    }

    pthread_mutex_unlock(&leaderboard_mux) ; 
    
    printf("[ ] Leaderboard : %s\n" , leaderboard) ; 

    //clear gli word indexes nei giocatori e resetta gli score
    clear_players_data(players_lis_ptr) ;

    // faccio la wait sulla barrier
    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    // se lo scorer ha finito, sblocco le operazioni sulla lista dei giocatori
    pthread_mutex_unlock(&players_mux);

    printf("[ ] scorer ended\n"); 
    pthread_detach(pthread_self()) ; 
    return NULL ; 
}

void alarm_handler(int sig){
    printf("ALARM RANG!\n");
    if(gamestate == 0){ 
        printf("[DEGUB] Number of players when alarm : %d\n" , plist_length(players_lis_ptr)) ; 
        pthread_barrier_init(&barrier , NULL , plist_length(players_lis_ptr) + 1);
        printf("------ Inizio pausa ------\n");
        //far partire un timer per la fine della pausa
        mux_f(alarm(60), alarm_mux) ; 

        //triggeraro lo scorer thread che calcola la classifica
        pthread_t client_tid ; 
        if(pthread_create(&client_tid , NULL , &scorer , NULL) != 0){
            perror("Errore nella pthread create dello scorer");
        }

        //cambiare il gamestate
        gamestate = 1 ; 
    } else {
        printf("------ Fine pausa! ------\n");
        //se sono in pausa faccio ripartire la partita
        //genero/leggo la prossima matrice
        //* blocco la matrice mentre la aggiorno e comunico ai giocatori
        pthread_mutex_lock(&matrix_mux);
            if(using_matrixfile){
                //nel caso in cui abbia raggiunto la fine del file ricomincio dall'inizio
                if(feof(matrix_fd)){
                    fseek(matrix_fd, 0, 0);
                }
                load_matrix_fromfile(game_matrix , matrix_fd) ; 
            } else {
                generate_letters(game_matrix , seed + rand()) ; //faccio questo per avere matrici diverse ma che dipendono dal seed, per non avere sempre lo stesso risultato
            }
            printf("New game matrix:\n");
        print_matrix(game_matrix);

        //mando a tutti i client il messaggio per la nuova matrice
        char * buf = malloc(sizeof(char) * 32) ; 
        buf[0] = '!' ; //automatic response dal server, non una richiesta del client 
        int index = 1;
        for_all_matrix( buf[index++] = game_matrix[r][c] )
        // Add the null terminator at the end of the string
        buf[index] = '\0';
        
        //write messages to clients
        Player * curr = players_lis_ptr ;
        while(curr != NULL){
            mux_f(write_message(curr->client_fd , MSG_MATRICE , buf), curr->p_mux) ; 
            curr = curr->next ; 
        }
        free(buf); 

        //cambiare il gamestate
        gamestate = 0 ; 
        //faccio ripartire il timer 
        mux_f(alarm(duration), alarm_mux) ;

        //* finito di lavorare sulla matrice, la libero
        pthread_mutex_unlock(&matrix_mux);
    }
}


int main(int argc , char * argv[]){
    if(argc < 3){
        perror("Not enough args");
        exit(EXIT_FAILURE);
    }
    
    int rv;

    //server name
    char server_name[25]  ;
    strcpy(server_name , argv[1]);
    //server port
    int server_port = atoi(argv[2]);

    //args opzionali con valori di default
    char matrix_filename[50] ;
    char dictionary_file[50] = "dict.txt" ;  // da cambiare al nome del dizionario di base  

    struct option long_options[] = {
        {"matrici" , required_argument , NULL , 'm' },
        {"durata" , required_argument , NULL , 't'}, //t : time
        {"seed" , required_argument , NULL , 's'},
        {"diz" , required_argument , NULL , 'd'},
        {NULL , 0 , NULL} 
    };

    while((rv = getopt_long(argc, argv, "m:t:s:d:" ,long_options , NULL)) != -1) {

        switch (rv) {
            case 'm':
                using_matrixfile = 1 ; 
                strcpy(matrix_filename , optarg);

                printf("Loading custom matrix from : %s\n" , matrix_filename) ; 
                matrix_fd = fopen(matrix_filename , "r");
                if(matrix_fd){
                    load_matrix_fromfile(game_matrix , matrix_fd) ; 
                }else{
                    perror("Error while loading custom matrix file\n");
                    exit(EXIT_FAILURE);
                }
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
    printf("Seed: %d (default if not provided)\n", seed);
    printf("Duration: %d (default 3 min if not provided)\n", duration);
    printf("Dictionary file: %s\n", dictionary_file);

    //load dictionary to server usando la trie se sto usando un dizionario
    load_trie_fromdict(&dict_trie , dictionary_file) ;  
        
    //se non ho caricato la matrice dal file la genero adesso
    pthread_mutex_lock(&matrix_mux);
        if(!using_matrixfile){
            generate_letters(game_matrix , seed);
        }

        //print per debug della matrice in uso
        printf("Starting Matrix:\n");
        print_matrix(game_matrix);
    pthread_mutex_unlock(&matrix_mux);

    //seeddo il random
    srand(seed) ; 

    //faccio partire l'alarm per il timer della partita e associo l'handler all'evento 
    struct sigaction sa ; 
    SYSC(rv , sigaction(SIGALRM , NULL , &sa) , "nella sigaction1"); 
    sa.sa_handler = alarm_handler ; 
    sa.sa_flags = SA_RESTART; // Add SA_RESTART flag per far ripartire le blocking syscalls interrotte dalla alarm (per esempio la accept)
    SYSC(rv , sigaction(SIGALRM , &sa , NULL) , "nella sigaction2") ; 
    alarm(duration) ;  

    //apertura del socket
        //varibili
    int server_fd , client_fd; 
    struct sockaddr_in server_addr , client_addr ; 
    socklen_t client_addr_len ; 

    //creazione del socket
    SYSC(server_fd, socket(AF_INET , SOCK_STREAM , 0), "nella socket call");

    //inizializzazione della struttra server_addr
    server_addr.sin_family = AF_INET ;
    server_addr.sin_port = htons(server_port) ;
    server_addr.sin_addr.s_addr = inet_addr(server_name) ;

    //binding
    SYSC(rv , bind(server_fd , (struct sockaddr *)&server_addr , sizeof(server_addr)) , "nella bind");

    //listening
    SYSC(rv , listen(server_fd , MAX_CLIENTS), "nella listen");

    //client accepting loop
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
    if(using_matrixfile) fclose(matrix_fd);
    exit(EXIT_SUCCESS);
}
