CC=arm-linux-gnueabihf-gcc -static -ggdb3
.PHONY=clean cleanall

bitmapmain: bitmapmain.c printbitad1.s
bitmapSC: bitmapSC.s printbitad1.s

test1: bitmapmain
	echo "1" | ./bitmapmain
	echo "3" | ./bitmapmain
	echo "-2004318072" | ./bitmapmain	# 0x88888888
	echo "657921" | ./bitmapmain		# 0x000a0a01

test2: bitmapSC
	echo "1" | ./bitmapSC
	echo "3" | ./bitmapSC
	echo "-2004318072" | ./bitmapSC		# 0x88888888
	echo "657921" | ./bitmapSC		# 0x000a0a01

clean:
	rm -fr bitmapSC bitmapmain
cleanall: clean
	rm -f *~

