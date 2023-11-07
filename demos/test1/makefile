CFLAGS	= -Wall
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000

OBJECTS	= pad.o	rand.o 2d1.o text_str.o	sound.o \
			tmd.o object.o

EXTRA_OBJECTS = sincos.o tangent.o asincos.o atan.o	

HEADERS = pad.h 2d1.h rand.h text_str.h dump.h asssert.h general.h \
			address.h sound.h tmd.h object.h

EXTRA_HEADERS = sincos.h tangent.h asincos.h atan.h 


all: test1 test2


	
test1: $(OBJECTS) test1.o 
	gcc $(LINKER) $(OBJECTS) test1.o -o test1
	strip test1

test1.o: test1.c $(HEADERS)
	gcc -c $(CFLAGS) test1.c




test2: $(OBJECTS) test2.o 
	gcc $(LINKER) $(OBJECTS) test2.o -o test2
	strip test2

test2.o: test2.c $(HEADERS)
	gcc -c $(CFLAGS) test2.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<
 