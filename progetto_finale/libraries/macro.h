#define SYSC(r,c,m)\
  if((r=c) == -1) {perror(m) ; exit(EXIT_FAILURE); }

#define SYSCN(r,c,m)\
  if((r=c) == NULL) {perror(m) ; exit(EXIT_FAILURE); }

#define NOTZERO(r,c,m)\
  if((r=c) != 0) {perror(m) ; exit(EXIT_FAILURE); }

#define BUF_SIZE 256

#define mux_f(f, mux)         \
  pthread_mutex_lock(&(mux)); f; pthread_mutex_unlock(&(mux))
  
