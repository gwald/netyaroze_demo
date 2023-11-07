CFLAGS	= -O2 -G0 -W -Wno-comment
LINKER  = -Xlinker -Ttext -Xlinker 80140000
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o atan.o asincos.o

####all: test

all: tuto16



test	: $(OBJECTS) pad.h tmd.h sin.h
	$(CC) $(LINKER) $(OBJECTS) -o $@
	strip $@

tuto0: $(OBJECTS) pad.h tmd.h sin.h asssert.h object.h dump.h
	$(CC) $(LINKER) $(OBJECTS) -o tuto0
	strip tuto0

tuto1: $(OBJECTS) pad.h tmd.h sin.h asssert.h object.h dump.h
	$(CC) $(LINKER) $(OBJECTS) -o tuto1
	strip tuto1

tuto2: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h
	$(CC) $(LINKER) $(OBJECTS) -o tuto2
	strip tuto2

tuto3: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) -o tuto3
	strip tuto3

tuto4.exe: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) -o tuto4.exe
	strip tuto4.exe

tuto5.exe: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto5.o -o tuto5.exe
	strip tuto5.exe

tuto6: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto6.o -o tuto6
	strip tuto6

tuto7: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto7.o -o tuto7
	strip tuto7

tuto8: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto8.o -o tuto8
	strip tuto8

tuto9: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto9.o -o tuto9
	strip tuto9

tuto10: $(OBJECTS) pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto10.o -o tuto10
	strip tuto10

tuto11: $(OBJECTS) tuto11.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto11.o -o tuto11
	strip tuto11

tuto12: $(OBJECTS) tuto12.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto12.o -o tuto12
	strip tuto12

tuto13: $(OBJECTS) tuto13.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto13.o -o tuto13
	strip tuto13

tuto14: $(OBJECTS) tuto14.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto14.o -o tuto14
	strip tuto14

tuto15: $(OBJECTS) tuto15.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto15.o -o tuto15
	strip tuto15

tuto16: $(OBJECTS) tuto16.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto16.o -o tuto16
	strip tuto16





object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c


.c.o:
	$(CC) $(CFLAGS) -funsigned-char -c $<




cleanup:
	del *.o
	del *.exe




	  