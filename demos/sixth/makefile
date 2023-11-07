CFLAGS	= -W

## CFLAGS options: -W, -Wall, -O3, -g, ....

LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000



OBJECTS	= pad.o	rand.o 2d1.o text_str.o	sound.o \
			tmd.o object.o trig.o matrix.o vector.o coord.o	tunnel.o

HEADERS = pad.h 2d1.h rand.h text_str.h dump.h asssert.h general.h \
			address.h sound.h tmd.h object.h trig.h	matrix.h vector.h \
			coord.h	tunnel.h
 


all: tuto1


tuto1: $(OBJECTS) tuto1.o 
	gcc $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1

tuto1.o: tuto1.c $(HEADERS)
	gcc -c $(CFLAGS) tuto1.c



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
	
sincos.o: sincos.c sincos.h
	gcc -c $(CFLAGS) sincos.c
	
asincos.o: asincos.c asincos.h
	gcc -c $(CFLAGS) asincos.c
	
tangent.o: tangent.c tangent.h
	gcc -c $(CFLAGS) tangent.c
	
atan.o: atan.c atan.h
	gcc -c $(CFLAGS) atan.c

trig.o: trig.c trig.h
	gcc -c $(CFLAGS) trig.c

matrix.o: matrix.c matrix.h
	gcc -c $(CFLAGS) matrix.c

vector.o: vector.c vector.h
	gcc -c $(CFLAGS) vector.c

coord.o: coord.c coord.h matrix.h
	gcc -c $(CFLAGS) coord.c

tunnel.o: tunnel.c tunnel.h
	gcc -c $(CFLAGS) tunnel.c
	
	 