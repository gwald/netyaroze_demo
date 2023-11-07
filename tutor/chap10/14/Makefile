
#////////////////////////////////////////////////////////////////////
# Makefile				
#////////////////////////////////////////////////////////////////////

#////////////////////////////////////////////////////////////////////
#				Compiler and Linker flags
#////////////////////////////////////////////////////////////////////
CFLAGS  = -O -g
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000
RM	= rm


PROG = main
OBJS = main.o genlib.o init.o

all: $(PROG)


$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)

main.o:    main.c cntrl.h main.h
genlib.o:  genlib.c genlib.h cntrl.h main.h
init.o:  init.c init.h cntrl.h main.h
