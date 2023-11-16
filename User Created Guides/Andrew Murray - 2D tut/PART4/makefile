CC=gcc
CFLAGS	= -Wall -g
LINKER  = -Xlinker -Ttext -Xlinker 80140000

PROG	= main.exe
OBJS    = main.o pad.o graphics.o sprite.o player.o

all: $(PROG)

main.exe :  $(OBJS)
	$(info NOTE: the VRAM images on the left hand side of  ) 
	$(info the screen wouldn't have been seen on a CRT TV! ) 
	$(info)
	$(CC) $(LINKER) $(OBJS) -o $@
#	strip $@

.c.o:
	$(CC) $(CFLAGS) -funsigned-char -c $<

run: $(PROG)
	yarexe auto.sio
	nopsx psx.exe

clean:
	del $(PROG)
	del *.o
	del psx.exe


