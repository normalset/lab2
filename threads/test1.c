#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static int x ;

static void * f_thread( void* arg){
  while(x < 5){
    printf("secondo thread : %d\n" , ++x);
    sleep(1);
    printf("secondo thread : %d\n", ++x);
  }
} 

int main(){
  pthread_t tid;
  int rv ;

  if(rv = pthread_create(&tid , NULL , &f_thread , NULL) != 0){ 
    exit(EXIT_FAILURE);
  } else {
    pthread_join(tid , NULL);
  }
  printf("primo thread : %d\n", ++x);
  return 0 ; 
}