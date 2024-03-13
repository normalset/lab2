#define SYSC(variable,funcall,err_msg)\
  if((variable=funcall) == -1) {perror(err_msg) ; exit(EXIT_FAILURE);}

#define SYSCN(variable,funcall,err_msg)\
  if((variable=funcall) == NULL) {perror(err_msg) ; exit(EXIT_FAILURE);}
