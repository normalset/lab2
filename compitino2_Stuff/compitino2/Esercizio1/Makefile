.PHONY: clean test
CC=gcc
objects_compitino2 = graph.o compitino2.o

compitino2: $(objects_compitino2)
	$(CC) $(objects_compitino2) -o compitino2

compitino2.o: compitino2.c graph.h graph.o

graph.o: graph.c graph.h

clean:
	rm -f *.o
	rm -f compitino2


test: compitino2
	./compitino2 g2.txt

release: compitino2
	rm -f *.o



