CFLAGS	= -O1 -g
LINKER	= -Xlinker -Ttext -Xlinker 80100000
RM	= del

PROG	= main
OBJS	= main.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LINKER) -o $@ $?

main.o: main.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)

