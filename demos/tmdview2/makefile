C_FLAGS  = -c
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000
RM = del


OBJECT_FILES = pad.o tmd.o object.o

all: main shortest


main: $(OBJECT_FILES) main.o
	gcc $(LINKER) $(OBJECT_FILES) main.o -o main

shortest: tmd.o object.o shortest.o
	gcc $(LINKER) tmd.o object.o shortest.o -o shortest




main.o: main.c pad.h tmd.h asssert.h object.h
	gcc $(C_FLAGS) main.c

shortest.o: shortest.c tmd.h object.h
	gcc $(C_FLAGS) shortest.c



	  