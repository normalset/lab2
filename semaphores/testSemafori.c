#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>


int main(){
	sem_t *sem = mmap(NULL , sizeof(sem_t), PROT_READ | PROT_WRITE , MAP_SHARED | MAP_ANONYMOUS , -1 , 0 ) ;
	if(sem == MAP_FAILED){
		perror("nella creazione di sem");
		exit(EXIT_FAILURE);
	}
	sem_init(sem , 1 , 0) ;
	pid_t pid = fork() ;
	if(pid == 0){
		//sono nel figlio
		sem_post(sem) ; //incrementa di uno il semaforo per segnalare al processo padre che e' pronto 
		// altro 
		exit(0);
	}
	sem_wait(sem) // attendo la sem_post() del figlio
	wait(NULL);
	sem_destroy(sem) ; 
	exit(0);
}
