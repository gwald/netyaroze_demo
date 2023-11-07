CFLAGS	= -O3 -g
LINKER  = -Xlinker -Ttext -Xlinker 801e3000
RM	= del

PROG	= main
OBJS    = main.o control.o mime.o snd.o

all: $(PROG)

#$(PROG): $(OBJS)
#	$(CC) $(OBJS) $(LINKER) -o $@ $?
#	strip $@

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
	strip $@

#main.o: main.c
#	$(CC) gcc -c $(CFLAGS) main.c

clean:
	$(RM) $(PROG)
	$(RM) main.o
	$(RM) control.o
	$(RM) mime.o
	$(RM) snd.o

.PHONY: all load clean
