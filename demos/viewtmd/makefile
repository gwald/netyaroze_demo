CFLAGS  = -O3 -g
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000

PROG    = viewtmd
OBJS    = viewtmd.o

all: $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
		
viewtmd.o: viewtmd.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


