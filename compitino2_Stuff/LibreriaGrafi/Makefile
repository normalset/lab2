.PHONY: clean test
objects=graph.o main.o libstack.a libqueue.a
CC=gcc

graphApp: $(objects)
	$(CC) $(objects) -o graphApp -lstack -lqueue -L. -Wl,-rpath,./

graph.o: graph.c Stack.h graph.h Queue.h

main.o: main.c graph.h

libqueue.so: sharedqueue.o
	gcc -shared $< -o $@

sharedqueue.o: Queue.c Queue.h
	gcc -c -fPIC $< -o $@

libstack.so: sharedstack.o
	gcc -shared $< -o $@

sharedstack.o: Stack.c Stack.h
	gcc -c -fPIC $< -o $@

libstack.a: Stack.c Stack.h
	gcc -c Stack.c Stack.h
	ar -qv libstack.a Stack.o
libqueue.a: Queue.c Queue.h
	gcc -c Queue.c Queue.h
	ar -qv libqueue.a Queue.o
clean:
	rm -f *.o
	rm -f graphApp
	rm -f libstack.so
	rm -f libqueue.so
	rm -f libstack.a
	rm -f libqueue.a
	rm -f Queue.h.gch
	rm -f Stack.h.gch
test: graphApp
	./graphApp g1.txt 9 0


