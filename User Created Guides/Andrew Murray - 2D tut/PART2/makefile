CC=mips-unknown-ecoff-gcc
CFLAGS	= -Wall
LINKER  = -Xlinker -Ttext -Xlinker 80140000

PROG	= main.exe
OBJS    = main.o pad.o

all: $(PROG)

main.exe :  $(OBJS)
	$(CC) $(LINKER) $(OBJS) -o $@
	strip $@

.c.o:
	$(CC) $(CFLAGS) -funsigned-char -c $<

run: $(PROG)
	yarexe auto.sio
	nopsx psx.exe

clean:
	del $(PROG)
	del *.o
	del psx.exe

