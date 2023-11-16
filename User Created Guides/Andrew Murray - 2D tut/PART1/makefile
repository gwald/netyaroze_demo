CC=mips-unknown-ecoff-gcc
CFLAGS	= -Wall -g
LINKER  = -Xlinker -Ttext -Xlinker 80140000

PROG	= main.exe
OBJS    = main.o

all: $(PROG)

main.exe:  $(OBJS)
	$(CC) $(LINKER) $(OBJS) -o $@
	mips-unknown-ecoff-strip $@

.c.o:
	$(CC) $(CFLAGS) -funsigned-char -c $<

run: $(PROG)
	yarexe auto.sio
	nopsx psx.exe

clean:
	rm $(PROG)
	rm *.o
	rm psx.exe
	rm *.iso
	rm *.cue
	rm *.bin

