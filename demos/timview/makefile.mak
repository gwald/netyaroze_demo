CC    = gcc
CFLAGS  = -O2 -g
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000
RM	= del

PROG    = main
OBJS    = main.o pad.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LINKER) $(OBJS) -o $@

main.o: main.c tim.h pad.h asssert.h
