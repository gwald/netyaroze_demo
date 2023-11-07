CFLAGS	= -O3 -g
LINKER	= -Xlinker -Ttext -Xlinker 80140000

PROG    = main
OBJS    = main.o pad.o blowup.o tmdutil.o

all: $(PROG)

$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)
		
main.o: main.c 

pad.o: pad.h pad.c

blowup.o: blowup.c blowup.h tmd.h tmdutil.c tmdutil.h

tmdutil.o: tmdutil.h tmd.h

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


