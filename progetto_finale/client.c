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
#include <signal.h>

//librerie custom
#include "libraries/matrix.c"
#include "libraries/paroliere.c"

//defines
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 32

// todo
// use mutex per aspettare che la lettura dei messaggi sia finita prima di stampare il promt

//varibili globali
int client_fd ;
int logged = 0;
int score  = 0 ;
int alarm_final_score = 0 ;
int gamestate = 0; // 0 playing | 1 pause;

pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER; 
sem_t prompt_sem ; 

//SigInt handler
void my_signal_handler(int signum)
{
  printf("[ ] client exiting\n");
  silent_write_message(client_fd, MSG_CLIENT_QUIT, NULL);
  sem_destroy(&prompt_sem);
  pthread_mutex_destroy(&mux);
  close(client_fd) ; 
  exit(EXIT_SUCCESS);

}

//messaggi di aiuto
char *prompt = "[ PROMPT PAROLIERE ]-->";

char *msg_aiuto = "\n\nComandi disponibili : \nregistra_utente [nome] : registrazione alla partita con un nome alfanumerico lungo max 10 caratteri,\nmatrice :\n  -se la partita e' in corso mostra la board corrente e il tempo rimanente\n  -se la partita e' finita mostra il tempo di attesa\np [parola_indicata] : permette di inviare una guess al server, punteggi ottenuti solo una volta per parola!\nfine : disconette il client dal server e termina la sua sessione\nclassifica: dopo la fine di una partita per vedere il risultato\n\n";

void * message_reader(void * args){
  int client_fd = *((int*)args) ;

  messaggio msg ; 
  while(1){
    //legge il messaggio
    msg = silent_read_message(client_fd);

    if(msg.type == MSG_ERR){
      if(msg.length != 0){
        printf("[ERR] %s\n" , msg.data) ; 
      }
      sem_post(&prompt_sem);
    }

    if(msg.type == MSG_OK && logged == 0){
      logged = 1 ;
      printf("[ ] Now logged in\n") ;
      mux_f(silent_write_message(client_fd , MSG_MATRICE , NULL), mux);
    }

    if(msg.type == MSG_MATRICE){
      // controllo se la richiesta dal server e' stata dell'utente o automatica
      int autom = 0 ;
      if(msg.data[0] == '!'){
        autom = 1 ;
      }
      printf("\n[ ] Matrice di gioco :\n");
        for (int r = 0; r < 4; r++) {
          for (int c = 0; c < 4; c++) {
            if(msg.data[r * 4 + c + autom] == 'q'){
              printf("|qu");
            }else{
              printf("|%c ", msg.data[r * 4 + c+ autom]);
            }
          }
          printf("|\n");
        }
        if(autom){
          mux_f(silent_write_message(client_fd, MSG_TEMPO_PARTITA, "!"), mux);
        }else{
          mux_f(silent_write_message(client_fd , MSG_TEMPO_PARTITA , NULL), mux);
        }
    }

    if(msg.type == MSG_TEMPO_PARTITA){
      // controllo se la richiesta dal server e' stata dell'utente o automatica
      if((msg.data[0] == '!')){
        printf("[TIME] Tempo rimanente nella partita : %s\n", msg.data+1);
        int rv ; 
        SYSC(rv, write(STDOUT_FILENO, "[ PROMPT PAROLIERE ]--> ", sizeof("[ PROMPT PAROLIERE ]--> ")), "nella final score write");
      }else{
        printf("[TIME] Tempo rimanente nella partita : %s\n", msg.data) ;
        sem_post(&prompt_sem);
      }
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
      // Se e' il messaggio di final score automatico riscrivo il prompt e non posto il semaforo, se e' una richiesta del giocatore posto il semaforo 
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
      mux_f(silent_write_message(client_fd , MSG_ALARM , NULL), mux) ;
    }
  }
}

int main(int argc , char * argv[]){
  if(argc != 3){
      perror("Args sbagliati , ./paroliere_cl , nome_server porta_server");
      exit(EXIT_FAILURE);
  }

  // Aggiungo il signal handler a SIGINT
  if (signal(SIGINT, my_signal_handler) == SIG_ERR)
  {
    perror("nella signal del client");
    return 1;
  }

  //server name
  char *server_name = malloc(sizeof(char) * 25) ;
  strcpy(server_name , argv[1]);
  //server port
  int server_port = atoi(argv[2]);
  // variabili
  int rv ;
  struct sockaddr_in server_addr;

  // Creazione del socket
  SYSC(client_fd, socket(AF_INET, SOCK_STREAM, 0), "nella socket");  

  // Inizializzazione della struttura server_addr
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = inet_addr(server_name);

  // Connect
  SYSC(rv, connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)), "nella connect");
  // posso ora scrivere su client_fd

  // inizializzo il semaforo
  // SYSCN(&prompt_sem, sem_open("/&prompt_sem", O_CREAT | O_EXCL, 0666, 1), "nella &prompt_sem open");
  sem_init(&prompt_sem, 1, 1);

  //creo un thread dedicato a leggere i messaggi 
  pthread_t client_tid ; 
  if(pthread_create(&client_tid , NULL , &message_reader , (void*)&client_fd) != 0){
      perror("Errore nella pthread create");
      close(client_fd) ;
  }

  char usr_input[128] ;

  char usr_arg[128] ;


  //main game loop
  while(1){
    sem_wait(&prompt_sem) ;
    printf("%s", prompt);
    //read user input 
    scanf("%s" , usr_input) ; 
    if(strcmp(usr_input , "p") == 0 || strcmp(usr_input , "registra_utente") == 0){
      scanf("%s" , usr_arg);
    }

    // messaggio di aiuto
    if(strcmp(usr_input , "aiuto") == 0){
      printf("%s\n" , msg_aiuto) ;
      sem_post(&prompt_sem) ; 
      continue ; 
    }

    //comando matrice
    else if(strcmp(usr_input , "matrice") == 0 ){
      if(logged){//mando un messaggio matrice al server
        mux_f(silent_write_message(client_fd , MSG_MATRICE , NULL), mux) ;
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
      mux_f(silent_write_message(client_fd , MSG_CLIENT_QUIT , NULL), mux) ;
      sem_destroy(&prompt_sem);
      close(client_fd); 
      pthread_mutex_destroy(&mux);

      //fermo anche il reader thread
      if ((rv = pthread_detach(client_tid)) != 0 ) {
        perror("nella pthread_detach");
        exit(EXIT_FAILURE);
      };
      if ((rv = pthread_cancel(client_tid)) != 0)
      {
        perror("nella pthread_cancel");
        exit(EXIT_FAILURE);
      };
      exit(EXIT_SUCCESS);
    }

    //comando registra utente
    else if(strcmp(usr_input , "registra_utente") == 0){
      if(logged){ // se sono gia' loggato non posso usare il comando
        printf("[ERR] Already logged in\n");
        sem_post(&prompt_sem);
        continue; 
      }

      //primo check sulla lunghezza
      if(strlen(usr_arg) > 10){
        printf("[ERR] Lunghezza MAX nome superata\n") ;
        sem_post(&prompt_sem);
        continue ;
      }
      //scrivo il messaggio con il nome
      mux_f(silent_write_message(client_fd, MSG_REGISTRA_UTENTE, usr_arg), mux);
    }

    //comando parola
    else if(strcmp(usr_input , "p") == 0){
      if(logged){ // se sono loggato posso proporre una parola
        // printf("[ ] type guess : ");
        if (strlen(usr_arg) < 4){
          printf("[ERR] La parola deve essere lunga almeno 4 lettere\n") ; 
          sem_post(&prompt_sem);
          continue ;
        }
        mux_f(silent_write_message(client_fd, MSG_PAROLA, usr_arg), mux);
      } else { //se non sono loggato devo loggare
        printf("[ERR] You have to be logged in to guess , log with registra_utente\n");
        sem_post(&prompt_sem);
        continue ; 
      }
    }
    
    else if(strcmp(usr_input , "classifica") == 0){
      mux_f(silent_write_message(client_fd , MSG_PUNTI_FINALI , NULL), mux) ; 
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
