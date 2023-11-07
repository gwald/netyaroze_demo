CFLAGS  = -O3 -g
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000

PROG    = example
OBJS    = example.o pad.o tmd.o

all: $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
		
example.o: example.c

pad.o: pad.h pad.c

tmd.o: tmd.h tmd.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


