#Compiler
CC = gcc

# # Compiler flags
CFLAGS = -Wall -g -pedantic -pthread

final:
	$(CC) $(CFLAGS) server.c -o paroliere_srv
	$(CC) $(CFLAGS) client.c -o paroliere_cl

Clean:
	rm -f *.o final

# #Header files
# HDRS = matrix.h paroliere.h pthread_barrier.h macro.h


# #source files
# SRCS = matrix.c paroliere.c server.c client.c pthread_barrier.c

# #OBJS
# OBJS = $(SRCS:.c=.o)

# #Executables
# SRVR = paroliere_srv
# CLNT = paroliere_cl

# .PHONY: clean all

# all: $(SRVR) $(CLNT)
# 	$(CC) server.c -o $(SRVR)
# 	$(CC) client.c -o $(CLNT)

# clean:
# 	rm -f $(OBJS) $(SRVR) $(CLNT)