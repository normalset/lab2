#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


int main(int argc , char *argv[]){
	//argv[1] nome del file
	//mancano i controlli
	
	const char *name = argv[1];
	int fd = open(name , O_RDWR) ; 
	fstat(fd , &statbuf) ;
	size_t sz = statbuf.st_size ;
	char *p = mmap(NULL , sz , PROT_READ | PROT_WRITE , MAP_SHARED , fs , 0);
	close(fd) ;
	// leggo e scrivo p 
}
// devo avere le opzioni di lettura e scrittura della open e della mmap consistenti
// se MAP_PRIVATE al posto che MAP_SHARED le modifiche non sono riportate nel file
