CFLAGS	= -Wall
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 801a0000

OBJECTS	= pad.o


all: tuto0 tuto1 tuto2 tuto3 tuto4 tuto5 tuto6 tuto7 tuto8


	
tuto0: $(OBJECTS) tuto0.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1

tuto2: $(OBJECTS) tuto2.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto2.o -o tuto2
	strip tuto2

tuto3: $(OBJECTS) tuto3.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto3.o -o tuto3
	strip tuto3

tuto4: $(OBJECTS) tuto4.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto4.o -o tuto4
	strip tuto4

tuto5: $(OBJECTS) tuto5.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto5.o -o tuto5
	strip tuto5

tuto6: $(OBJECTS) tuto6.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto6.o -o tuto6
	strip tuto6

tuto7: $(OBJECTS) tuto7.o profiler.o pad.h asssert.h dump.h profiler.h
	gcc $(LINKER) $(OBJECTS) profiler.o tuto7.o -o tuto7
	strip tuto7

tuto8: $(OBJECTS) tuto8.o pad.h asssert.h dump.h
	gcc $(LINKER) $(OBJECTS) tuto8.o -o tuto8
	strip tuto8



tuto0.o: tuto0.c
	gcc -c $(CFLAGS) tuto0.c

tuto1.o: tuto1.c
	gcc -c $(CFLAGS) tuto1.c

tuto2.o: tuto2.c
	gcc -c $(CFLAGS) tuto2.c

tuto3.o: tuto3.c
	gcc -c $(CFLAGS) tuto3.c

tuto4.o: tuto4.c
	gcc -c $(CFLAGS) tuto4.c

tuto5.o: tuto5.c
	gcc -c $(CFLAGS) tuto5.c
   
tuto6.o: tuto6.c
	gcc -c $(CFLAGS) tuto6.c

tuto7.o: tuto7.c
	gcc -c $(CFLAGS) tuto7.c

tuto8.o: tuto8.c
	gcc -c $(CFLAGS) tuto8.c




profiler.o: profiler.c
	gcc -c $(CFLAGS) profiler.c
