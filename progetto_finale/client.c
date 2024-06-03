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
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

//librerie custom
#include "matrix.c"
#include "paroliere.c"

//defines
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 32

//varibili globali 
int logged = 0 ;
int score  = 0 ;
int alarm_final_score = 0 ;
sem_t prompt_sem ; 

//messaggi di aiuto
char *prompt = "[ PROMPT PAROLIERE ]-->";

char *msg_aiuto = "\n\nComandi disponibili : \nregistra_utente [nome] : registrazione alla partita con un nome alfanumerico lungo max 10 caratteri,\nmatrice :\n  -se la partita e' in corso mostra la board corrente e il tempo rimanente\n  -se la partita e' finita mostra il tempo di attesa\np [parola_indicata] : permette di inviare una guess al server, punteggi ottenuti solo una volta per parola!\nfine : disconette il client dal serfer e termina la sessione con il server\nclassifica: dopo la fine di una partita per vedere il risultato\n\n";

void * message_reader(void * args){
  int client_fd = *((int*)args) ;

  messaggio msg ; 
  while(1){
    //legge il messaggio 
    msg = silent_read_message(client_fd) ; 

    if(msg.type == MSG_ERR){
      if(msg.length != 0){
        printf("[ERR] %s\n" , msg.data) ; 
      }
      sem_post(&prompt_sem);
    }

    if(msg.type == MSG_OK && logged == 0){
      logged = 1 ;
      printf("[ ] Now logged in\n") ; 
      silent_write_message(client_fd , MSG_MATRICE , NULL);
    }

    if(msg.type == MSG_PUNTI_PAROLA){
      score += atoi(msg.data) ; 
    }

    if(msg.type == MSG_MATRICE){
      printf("[ ] Matrice di gioco :\n");
        for (int r = 0; r < 4; r++) {
          for (int c = 0; c < 4; c++) {
            if(msg.data[r * 4 + c] == 'q'){
              printf("|qu");
            }else{
              printf("|%c ", msg.data[r * 4 + c]);
            }
          }
          printf("|\n");
        }
        silent_write_message(client_fd , MSG_TEMPO_PARTITA , NULL);
    }

    if(msg.type == MSG_TEMPO_PARTITA){
      printf("[TIME] Tempo rimanente nella partita : %s\n", msg.data) ;
      sem_post(&prompt_sem);
    }

    if(msg.type == MSG_TEMPO_ATTESA){
      printf("[TIME] Tempo rimanente per la prossima partita : %s\n", msg.data) ;
      sem_post(&prompt_sem);
    }

    if(msg.type == MSG_PUNTI_PAROLA){
      printf("[POINTS] Got %s points for that word!\n" , msg.data);
      score += atoi(msg.data);
      sem_post(&prompt_sem);
    }

    if(msg.type == MSG_PUNTI_FINALI){
      //printa il csv dei punti finali
      printf("\n[FINAL] Classifica:\n");
      char * token ; 
      token = strtok(msg.data , ";");
      printf("%s\n" , token) ; 
      while ((token = strtok(NULL , ";")) != NULL){
        printf("%s\n" , token) ; 
      }

      //Se e' il messaggio di final score automatico riscrivo il prompt e non posto il semaforo, se e' una richiesta del giocatore posto il semaforo 
      if(alarm_final_score == 1){
        int rv ;
        SYSC(rv, write(STDOUT_FILENO, "[ PROMPT PAROLIERE ]--> ", sizeof("[ PROMPT PAROLIERE ]--> ")), "nella final score write");
        alarm_final_score = 0;
      }else{
        sem_post(&prompt_sem);
      }
    }

    if(msg.type == MSG_ALARM){
      printf("\n------ TIME'S UP ! ------\n") ; 
      alarm_final_score = 1 ; 
      silent_write_message(client_fd , MSG_ALARM , NULL) ;
    }
    free(msg.data);
  }
}

int main(int argc , char * argv[]){
  if(argc != 3){
      perror("Args sbagliati , ./paroliere_cl , nome_server porta_server");
      exit(EXIT_FAILURE);
  }
  

  //server name
  char *server_name = malloc(sizeof(char) * 25) ;
  strcpy(server_name , argv[1]);
  //server port
  int server_port = atoi(argv[2]);
  // variabili
  int rv , client_fd;
  struct sockaddr_in server_addr;
    
  // Creazione del socket
  SYSC(client_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");

  // Inizializzazione della struttura server_addr
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Connect
  SYSC(rv, connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "nella connect");
  // posso ora scrivere su client_fd

  //creo un thread dedicato a leggere i messaggi 
  pthread_t client_tid ; 
  if(pthread_create(&client_tid , NULL , &message_reader , (void*)&client_fd) != 0){
      perror("Errore nella pthread create");
      close(client_fd) ;
  }

  char usr_input[100] ;

  char cmd_arg[20] ;

  //inizializzo il semaforo
  // SYSCN(&prompt_sem, sem_open("/&prompt_sem", O_CREAT | O_EXCL, 0666, 1), "nella &prompt_sem open");
  sem_init(&prompt_sem , 1 , 1) ; 

  //main game loop
  while(1){
    sem_wait(&prompt_sem) ; 
    printf("%s ", prompt);
    scanf("%s" , usr_input) ; 
    // printf("[DEBUG] usr_input : %s\n" , usr_input) ; 

    // messaggio di aiuto
    if(strcmp(usr_input , "aiuto") == 0){
      printf("%s\n" , msg_aiuto) ;
      sem_post(&prompt_sem) ; 
      continue ; 
    }

    //comando matrice
    else if(strcmp(usr_input , "matrice") == 0 ){
      if(logged){//mando un messaggio matrice al server
        silent_write_message(client_fd , MSG_MATRICE , NULL) ;
        continue ;
      }else{
        printf("[ERR] Log in to see matrix\n") ;
        sem_post(&prompt_sem);
        continue; 
      }
    }

    //comando fine
    else if(strcmp(usr_input , "fine") == 0){
      printf("[ ] client exiting\n");
      silent_write_message(client_fd , MSG_CLIENT_QUIT , NULL) ;
      sem_destroy(&prompt_sem);
      exit(EXIT_SUCCESS);
    }

    //comando registra utente
    else if(strcmp(usr_input , "registra_utente") == 0){
      if(logged){ // se sono gia' loggato non posso usare il comando
        printf("Already logged in");
        sem_post(&prompt_sem);
        continue; 
      }
      
      // printf("[LOGIN] type username : ");
      scanf("%s" , usr_input) ; 

      //primo check sulla lunghezza
      if(strlen(cmd_arg) > 10){
        printf("Lunghezza MAX nome superata") ;
        sem_post(&prompt_sem);
        continue ;
      }
      //scrivo il messaggio con il nome
      silent_write_message(client_fd , MSG_REGISTRA_UTENTE , usr_input) ;
    }

    //comando parola
    else if(strcmp(usr_input , "p") == 0){
      if(logged){ // se sono loggato posso proporre una parola
        // printf("[ ] type guess : ");
        scanf("%s" , usr_input) ; 
        silent_write_message(client_fd , MSG_PAROLA , usr_input);
      } else { //se non sono loggato devo loggare
        printf("You have to be logged in to guess , log with registra_utente\n");
        sem_post(&prompt_sem);
        continue ; 
      }
    }
    
    else if(strcmp(usr_input , "classifica") == 0){
      silent_write_message(client_fd , MSG_PUNTI_FINALI , NULL) ; 
    }

    //input non riconosciuto
    else{
      printf("[ERR] Comando '%s' non riconosciuto\n", usr_input) ; 
      sem_post(&prompt_sem) ; 
    }
  }

  free(server_name) ; 
  exit(EXIT_SUCCESS);
}
