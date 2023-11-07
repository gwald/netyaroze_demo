CFLAGS	= -O2 -G0 -W -Wno-comment
LINKER  = -Xlinker -Ttext -Xlinker 80140000
RM	= del

OBJECTS	= pad.o tmd.o object.o sincos.o atan.o #####asincos.o

####all: test

all: tuto2 tuto1 tuto0 tuto3 tuto4


  
tuto0: $(OBJECTS) tuto0.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto0.o -o tuto0
	strip tuto0

tuto1: $(OBJECTS) tuto1.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto1.o -o tuto1
	strip tuto1

tuto2: $(OBJECTS) tuto2.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto2.o -o tuto2
	strip tuto2

tuto3: $(OBJECTS) tuto3.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto3.o -o tuto3
	strip tuto3

tuto4: $(OBJECTS) tuto4.o pad.h tmd.h asssert.h object.h dump.h sincos.h atan.h
	$(CC) $(LINKER) $(OBJECTS) tuto4.o -o tuto4
	strip tuto4
  


object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c


.c.o:
	$(CC) $(CFLAGS) -funsigned-char -c $<




cleanup:
	del *.o
	del *.exe




	  