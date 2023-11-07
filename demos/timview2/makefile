CFLAGS	= -O3 -W
LINKER	= -Xlinker -Ttext -Xlinker 80140000

OBJECTS	= pad.o


all: tuto0


	
tuto0: $(OBJECTS) tuto0.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0



tuto0.o: tuto0.c
	gcc -c $(CFLAGS) tuto0.c
   