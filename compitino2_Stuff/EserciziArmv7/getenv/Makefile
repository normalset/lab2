CC=arm-linux-gnueabihf-gcc -static -ggdb3
.PHONY=clean cleanall

getenv: getenv.c getenv.s ../strncmp/strncmp.s ../strtok/strtok.s

clean:
	rm -fr getenv
cleanall: clean
	rm -f *~

