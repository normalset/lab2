#define SYSC(r,c,m)\
  if((r=c) == -1) {perror(m) ; exit(EXIT_FAILURE); }

#define SYSCN(r,c,m)\
  if((r=c) == NULL) {perror(m) ; exit(EXIT_FAILURE); }

#define SYSCZ(r,c,m)\
	if((r=c)== 0){perror(m); exit(EXIT_FAILURE);}
