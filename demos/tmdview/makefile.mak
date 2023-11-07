C_FLAGS  = -c -O3
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000
RM = del

PROJECT = tuto0
OBJECT_FILES = tuto0.o pad.o tim.o tmd.o

all: main


main: main.o pad.o tim.o tmd.o
	gcc $(LINKER) main.o pad.o tim.o tmd.o -o main

main.o: main.c tim.h pad.h tmd.h asssert.h
	gcc $(C_FLAGS) main.c



	  