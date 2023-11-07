CFLAGS  = -W -Wno-comment
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o tangent.o atan.o storescr.o ###asincos.o




all: tuto0 tuto1 tuto2 tuto3 tuto4 tuto5

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



tuto0.o: tuto0.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto0.c

tuto1.o: tuto1.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto1.c

tuto2.o: tuto2.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto2.c
 
tuto3.o: tuto3.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto3.c

tuto4.o: tuto4.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto4.c

tuto5.o: tuto5.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto5.c



object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c



storescr.o: storescr.c storescr.h
	gcc	-c $(CFLAGS) storescr.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<








cleanup:
	del *.o
	del *.exe




	  
