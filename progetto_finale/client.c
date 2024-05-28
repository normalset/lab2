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

//todo finire print punti finali

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
#define MSG_CLIENT_QUIT 'Q'
#define MSG_ALARM 'Z'
#define MSG_CLASSIFICA 'C'

//varibili globali 
int logged = 0 ;
int score  = 0 ;

typedef struct messaggio {
    char type ; 
    unsigned int length ; //0 nel caso in cui il campo data non e' significativo
    char * data ; //dati effettivi
}messaggio ; 

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

void * message_reader(void * args){
  int client_fd = *((int*)args) ;

  messaggio msg ; 
  while(1){
    //legge il messaggio 
    msg = read_message(client_fd) ; 

    if(msg.type == MSG_ERR){
      if(msg.length != 0){
        printf("[ERR] %s\n" , msg.data) ; 
      }
    }

    if(msg.type == MSG_OK && logged == 0){
      logged = 1 ;
      printf("[ ] Now logged in\n") ; 
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
    }

    if(msg.type == MSG_TEMPO_PARTITA){
      printf("[TIME] Tempo rimanente nella partita : %s\n", msg.data) ; 
    }

    if(msg.type == MSG_TEMPO_ATTESA){
      printf("[TIME] Tempo rimanente per la prossima partita : %s\n", msg.data) ; 
    }

    if(msg.type == MSG_PUNTI_PAROLA){
      printf("[POINTS] Got %s points for that word!\n" , msg.data);
      score += atoi(msg.data); 
    }

    if(msg.type == MSG_PUNTI_FINALI){
      //printa il csv dei punti finali
      printf("[FINAL] Classifica:\n");
      char * token ; 
      token = strtok(msg.data , ";");
      printf("%s\n" , token) ; 
      while ((token = strtok(NULL , ";")) != NULL){
        printf("%s\n" , token) ; 
      } 
    }

    if(msg.type == MSG_ALARM){
      printf("------ TIME'S UP ! ------\n") ; 
      write_message(client_fd , MSG_ALARM , NULL) ; 
    }
  }
}

char * prompt = "[ PROMPT PAROLIERE ]-->" ; 

char * msg_aiuto = "\n\nComandi disponibili : \nregistra_utente [nome] : registrazione alla partita con un nome alfanumerico lungo max 10 caratteri,\nmatrice :\n  -se la partita e' in corso mostra la board corrente e il tempo rimanente\n  -se la partita e' finita mostra il tempo di attesa\np [parola_indicata] : permette di inviare una guess al server, punteggi ottenuti solo una volta per parola!\nfine : disconette il client dal serfer e termina la sessione con il server ";

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

  //main game loop
  while(1){
    printf("%s ", prompt) ; 
    scanf("%s" , usr_input) ; 
    printf("[DEBUG] usr_input : %s\n" , usr_input) ; 

    // messaggio di aiuto
    if(strcmp(usr_input , "aiuto") == 0){
      printf("%s\n" , msg_aiuto) ;
      continue ; 
    }

    //comando matrice
    if(strcmp(usr_input , "matrice") == 0 ){
      if(logged){//mando un messaggio matrice al server
        write_message(client_fd , MSG_MATRICE , NULL) ;
        continue ;
      }else{
        printf("[ERR] Log in to see matrix\n") ; 
      }
    }

    //comando fine
    if(strcmp(usr_input , "fine") == 0){
      printf("[ ] client exiting");
      write_message(client_fd , MSG_CLIENT_QUIT , NULL) ; 
      sleep(1) ; 
      exit(EXIT_SUCCESS) ; 
    }

    //comando registra utente
    if(strcmp(usr_input , "registra_utente") == 0){
      if(logged){ // se sono gia' loggato non posso usare il comando
        printf("Already logged in");
        continue; 
      }
      
      printf("[LOGIN] type username : ");
      scanf("%s" , usr_input) ; 

      //primo check sulla lunghezza
      if(strlen(cmd_arg) > 10){
        printf("Lunghezza MAX nome superata") ;
        continue ;
      }
      //scrivo il messaggio con il nome
      write_message(client_fd , MSG_REGISTRA_UTENTE , usr_input) ;
    }

    //comando parola
    if(strcmp(usr_input , "p") == 0){
      if(logged){ // se sono loggato posso proporre una parola
        printf("[ ] type guess : ");
        scanf("%s" , usr_input) ; 
        write_message(client_fd , MSG_PAROLA , usr_input);
      } else { //se non sono loggato devo loggare
        printf("You have to be logged in to guess , log with registra_utente\n");
      }
    }
    
    if(strcmp(usr_input , "classifica") == 0){
      write_message(client_fd , MSG_PUNTI_FINALI , NULL) ; 
    }
  }

  free(server_name) ; 
  exit(EXIT_SUCCESS);
}
