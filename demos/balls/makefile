CFLAGS	= -O2 -W -Wno-comment
LINKER  = -Xlinker -Ttext -Xlinker 80140000
RM	= del



all: tuto0 tuto1


##main: main.o vid_mode.h
##	$(CC) $(LINKER) main.o -o main
##	strip main

tuto0: tuto0.o
	$(CC) $(LINKER) tuto0.o -o tuto0
	strip tuto0

tuto1: tuto1.o vid_mode.h
	$(CC) $(LINKER) tuto1.o -o tuto1
	strip tuto1
 


tuto1.o: tuto1.c vid_mode.h
	gcc $(CFLAGS) -c tuto1.c

tuto0.o: tuto0.c
	gcc $(CFLAGS) -c tuto0.c

		
.c.o:
	$(CC) $(CFLAGS) -c $<




cleanup:
	del *.o
	del *.exe




	  