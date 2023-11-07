CC    = gcc
CFLAGS  = -O2
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000
RM	= del

PROG    = main
OBJS    = main.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LINKER) $(OBJS) -o $@

main.o: main.c
