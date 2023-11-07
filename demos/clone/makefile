CFLAGS  = -O3 -g
LINKER  = -Xlinker -Ttext -Xlinker 80100000

PROG    = main
OBJS    = main.o pad.o sincos.o

all: $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
		
main.o: main.c tmd.h

pad.o: pad.h pad.c

sincos.o: sincos.h sincos.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


