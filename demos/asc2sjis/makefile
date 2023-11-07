CFLAGS	= -Wall

LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000

PROGRAM = main

SRC = main.c asc2sjis.c

OBJECTS = main.o asc2sjis.o




all: $(OBJECTS)	$(SRC)
	$(CC) $(LINKER) $(OBJECTS) -o main


main.o: main.c asc2sjis.h
	gcc -c $(CFLAGS) main.c

asc2sjis.o: asc2sjis.c asc2sjis.h
	gcc -c $(CFLAGS) asc2sjis.c



