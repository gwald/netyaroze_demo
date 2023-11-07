CFLAGS  = -W -Wno-comment
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o tangent.o atan.o storescr.o ###asincos.o




all: tuto0 tuto1

tuto0: $(OBJECTS) tuto0.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	$(CC) $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1
   



tuto0.o: tuto0.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto0.c

tuto1.o: tuto1.c pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h
	gcc -c $(CFLAGS) tuto1.c
  




object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c



storescr.o: storescr.c storescr.h
	gcc	-c $(CFLAGS) storescr.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<








cleanup:
	del *.o
	del *.exe




	  
