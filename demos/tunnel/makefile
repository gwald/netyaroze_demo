CFLAGS	= -O3 -W -Wno-comment
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000
	## USED to put program at 80140000; changed to get sound to work
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o tangent.o atan.o storescr.o ###asincos.o



CURRENT_C = tuto13.c
CURRENT_O = tuto13.o


all: tuto13

   
tuto0: $(OBJECTS) tuto0.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
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

 



$(CURRENT_O): $(CURRENT_C) pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) $(CURRENT_C)




object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<








cleanup:
	del *.o
	del *.exe




	  