CFLAGS	= -W -O3
LINKER	= -Xlinker -Ttext -Xlinker 80140000

OBJECTS	= pad.o


all: tuto0 tuto1 tuto2


	
tuto0: $(OBJECTS) tuto0.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1

tuto2: $(OBJECTS) tuto2.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto2.o -o tuto2
	strip tuto2



tuto0.o: tuto0.c
	gcc -c $(CFLAGS) tuto0.c

tuto1.o: tuto1.c
	gcc -c $(CFLAGS) tuto1.c

tuto2.o: tuto2.c
	gcc -c $(CFLAGS) tuto2.c
   