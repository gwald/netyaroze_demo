CFLAGS	= -O3 -g
LINKER	= -Xlinker -Ttext -Xlinker 80140000

PROG	= main
OBJS	= main.o

all: $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(LINKER) -o $@ $?
	
main.o: main.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


