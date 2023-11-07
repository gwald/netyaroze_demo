CFLAGS	= -O3 -W -Wno-comment		  ## -O3
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80110000
	## USED to put program at 80140000; changed to get sound to work
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o tangent.o atan.o ###asincos.o



#######all: tuto0 tuto1 tuto2 tuto3	tuto4 tuto5	tuto6

all: tuto19



tuto0: $(OBJECTS) tuto0.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1
 
tuto2: $(OBJECTS) tuto2.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto2.o -o tuto2
	strip tuto2

tuto3: $(OBJECTS) tuto3.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto3.o -o tuto3
	strip tuto3

tuto4: $(OBJECTS) tuto4.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto4.o -o tuto4
	strip tuto4

tuto5: $(OBJECTS) tuto5.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto5.o -o tuto5
	strip tuto5

tuto6: $(OBJECTS) tuto6.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto6.o -o tuto6
	strip tuto6

tuto7: $(OBJECTS) tuto7.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto7.o -o tuto7
	strip tuto7

tuto8: $(OBJECTS) tuto8.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto8.o -o tuto8
	strip tuto8

tuto9: $(OBJECTS) tuto9.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto9.o -o tuto9
	strip tuto9

tuto10: $(OBJECTS) tuto10.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto10.o -o tuto10
	strip tuto10

tuto11: $(OBJECTS) tuto11.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto11.o -o tuto11
	strip tuto11

tuto12: $(OBJECTS) tuto12.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto12.o -o tuto12
	strip tuto12

tuto13: $(OBJECTS) tuto13.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto13.o -o tuto13
	strip tuto13

tuto14: $(OBJECTS) tuto14.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto14.o -o tuto14
	strip tuto14

tuto15: $(OBJECTS) tuto15.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto15.o -o tuto15
	strip tuto15

tuto16: $(OBJECTS) tuto16.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto16.o -o tuto16
	strip tuto16

tuto17: $(OBJECTS) tuto17.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto17.o -o tuto17
	strip tuto17

tuto18: $(OBJECTS) tuto18.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto18.o -o tuto18
	strip tuto18

tuto19: $(OBJECTS) tuto19.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto19.o -o tuto19
	strip tuto19



## these two for sound testing
tuto55: $(OBJECTS) tuto55.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto55.o -o tuto55
	strip tuto55

tuto56: $(OBJECTS) tuto56.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto56.o -o tuto56
	strip tuto56






object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<




cleanup:
	del *.o
	del *.exe




	  