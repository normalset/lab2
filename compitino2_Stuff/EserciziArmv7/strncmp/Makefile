CC=arm-linux-gnueabihf-gcc -static -ggdb3
.PHONY=clean cleanall

strncmp: strncmp.c strncmp.s

strncmp-v2: strncmp.c strncmp-v2.s

clean:
	rm -fr strncmp strncmp-v2
cleanall: clean
	rm -f *~

