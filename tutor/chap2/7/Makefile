
#////////////////////////////////////////////////////////////////////
# Makefile				
#////////////////////////////////////////////////////////////////////

#////////////////////////////////////////////////////////////////////
#				Compiler and Linker flags
#////////////////////////////////////////////////////////////////////
CFLAGS  = -O -g
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80180000
RM	= rm


PROG = main
OBJS = main.o

all: $(PROG)


$(PROG): $(OBJS) 
	$(CC) $(OBJS) $(LINKER) -o $(PROG)

main.o:    main.c main.h cntrl.h


