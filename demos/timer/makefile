C_FLAGS  = -c -Wall
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80140000
RM = del


OBJECT_FILES = 2d1.o timer.o

all: main


main: $(OBJECT_FILES) main.o
	gcc $(LINKER) $(OBJECT_FILES) main.o -o main


main.o: main.c asssert.h 2d1.h timer.h
	gcc $(C_FLAGS) main.c

2d1.o: 2d1.c
	gcc $(C_FLAGS) 2d1.c

timer.o: timer.c
	gcc $(C_FLAGS) timer.c



	  