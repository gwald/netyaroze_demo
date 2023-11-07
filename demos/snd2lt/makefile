#CFLAGS  = -O2 -g
CFLAGS  = -O2 -g
#LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000
LINKER  = -Xlinker -Ttext -Xlinker 80100000

PROG    = main
OBJS    = main.o cdfeed.o

all: $(PROG)

$(PROG): $(OBJS) makefile
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
		
main.o: main.c makefile


feed:
	$(CC) -c -O2 cdfeed.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


