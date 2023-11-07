CFLAGS	= -Wall -O2
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000

OBJECTS	= pad.o	rand.o 2d1.o text_str.o	sound.o \
			tmd.o object.o sincos.o	

HEADERS = pad.h 2d1.h rand.h text_str.h dump.h asssert.h general.h \
			address.h sound.h tmd.h object.h geom.h sincos.h


all: main


	
main: $(OBJECTS) main.o 
	gcc $(LINKER) $(OBJECTS) main.o -o main
	strip main

main.o: main.c $(HEADERS)
	gcc -c $(CFLAGS) main.c



.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<




### NOTE: not all dependencies listed: eg asssert.h, dump.h, etc

pad.o: pad.c pad.h
	gcc -c $(CFLAGS) pad.c

2d1.o: 2d1.c 2d1.h
	gcc -c $(CFLAGS) 2d1.c
	
tmd.o: tmd.c tmd.h tmdutil.h
	gcc -c $(CFLAGS) tmd.c
	
rand.o: rand.c rand.h
	gcc -c $(CFLAGS) rand.c
	
text_str.o: text_str.c text_str.h 2d1.h	address.h
	gcc -c $(CFLAGS) text_str.c
	
sound.o: sound.c sound.h address.h
	gcc -c $(CFLAGS) sound.c
	
object.o: object.c object.h
	gcc -c $(CFLAGS) object.c

sincos.o: sincos.c sincos.h
	gcc -c $(CFLAGS) sincos.c
	
 