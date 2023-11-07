CFLAGS	= -O3 -W -Wno-comment
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000





OBJECTS	= pad.o tmd.o object.o sincos.o tangent.o atan.o
HEADERS = pad.h tmd.h asssert.h object.h dump.h sincos.h tangent.h atan.h asincos.h




all: main demo demo2




main: $(OBJECTS) main.o $(HEADERS)
	$(CC) $(LINKER) $(OBJECTS) main.o -o main
	strip main

demo: $(OBJECTS) demo.o $(HEADERS) storescr.o
	$(CC) $(LINKER) $(OBJECTS) storescr.o demo.o -o demo
	strip demo

demo2: $(OBJECTS) demo2.o $(HEADERS) storescr.o
	$(CC) $(LINKER) $(OBJECTS) storescr.o demo2.o -o demo2
	strip demo2





main.o: main.c $(HEADERS)
	gcc -c $(CFLAGS) main.c

demo.o: demo.c $(HEADERS) storescr.h
	gcc -c $(CFLAGS) demo.c

demo2.o: demo2.c $(HEADERS) storescr.h
	gcc -c $(CFLAGS) demo2.c





object.o: object.c object.h
	gcc	-c $(CFLAGS) object.c


.c.o:
	gcc -c $(CFLAGS) -funsigned-char -c $<






cleanup:
	del *.o
	del *.exe




	  