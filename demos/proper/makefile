LINKER	= -Xlinker -Ttext -Xlinker 80140000


all: pal ntsc tellme


	
pal: pal.o
	gcc $(LINKER) pal.o -o pal

ntsc: ntsc.o
	gcc $(LINKER) ntsc.o -o ntsc

tellme: tellme.o
	gcc $(LINKER) tellme.o -o tellme



 
pal.o: pal.c
	gcc -c pal.c

ntsc.o: ntsc.c
	gcc -c ntsc.c

tellme.o: tellme.c
	gcc -c tellme.c

   