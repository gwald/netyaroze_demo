CFLAGS	= -O3 -Wall
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000

OBJECTS	= pad.o	rand.o 2d1.o text_str.o	sound.o \
			tmd.o object.o

EXTRA_OBJECTS = sincos.o tangent.o asincos.o atan.o	

HEADERS = pad.h 2d1.h rand.h text_str.h dump.h asssert.h general.h \
			address.h sound.h tmd.h object.h

EXTRA_HEADERS = sincos.h tangent.h asincos.h atan.h 


all: main tuto0


	
main: $(OBJECTS) main.o 
	gcc $(LINKER) $(OBJECTS) main.o -o main
	strip main

main.o: main.c $(HEADERS)
	gcc -c $(CFLAGS) main.c

tuto0: $(OBJECTS) tuto0.o 
	gcc $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto0.o: tuto0.c $(HEADERS)
	gcc -c $(CFLAGS) tuto0.c



.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<




### NOTE: not all dependencies listed: eg asssert.h, dump.h, etc

pad.o: pad.c pad.h
	gcc -c $(CFLAGS) pad.c

2d1.o: 2d1.c 2d1.h
	gcc -c $(CFLAGS) 2d1.c
	
tmd.o: tmd.c tmd.h
	gcc -c $(CFLAGS) tmd.c
	
rand.o: rand.c rand.h
	gcc -c $(CFLAGS) rand.c
	
text_str.o: text_str.c text_str.h 2d1.h	address.h
	gcc -c $(CFLAGS) text_str.c
	
sound.o: sound.c sound.h address.h
	gcc -c $(CFLAGS) sound.c
	
object.o: object.c object.h	tmd.h 2d1.h
	gcc -c $(CFLAGS) object.c 