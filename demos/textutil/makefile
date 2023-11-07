CFLAGS	= -O3
LINKER	= -Xlinker -Ttext -Xlinker 80140000

PROG	= main
OBJS    = main.o textfont.o

all: $(PROG)

$(PROG): $(OBJS) 
	gcc $(OBJS) $(LINKER) -o $(PROG)
	
	
	
#####$@ $?
	
	
	
main.o: main.c
	gcc -c $(CFLAGS) main.c

textfont.o: textfont.c textfont.h
	gcc -c $(CFLAGS) textfont.c

clean:
	$(RM) $(PROG) 
	$(RM) $(OBJS)


