C_FLAGS  = -O3
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000


########		OLD				##################
#RM = del										 #
#												 #
#PROJECT = tuto7								 #
#OBJECT_FILES = tuto6.o pad.o tim.o tmd.o		 #
#												 #
#$(PROJECT): $(OBJECT_FILES)					 #
#	gcc $(LINKER) $(OBJECT_FILES) -o $(PROJECT)	 #
#												 #
#tuto6.o: tuto6.c tim.h pad.h tmd.h asssert.h	 #
#	gcc $(C_FLAGS) tuto6.c						 #
#												 #
################################################## 




#####	all: tuto15 tuto14 tuto13 tuto12 tuto11 tuto10 tuto9 \
#####	tuto8 tuto7 tuto6 tuto5 tuto4 tuto3 tuto2 tuto1 tuto0

all: pad.o tim.o tmd.o vector.o	tuto16


tuto16: tuto16.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto16.o pad.o tim.o tmd.o vector.o -o tuto16

tuto16.o: tuto16.c tim.h pad.h tmd.h sincos.h asssert.h	dump.h
	gcc -c $(C_FLAGS) tuto16.c



tuto15: tuto15.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto15.o pad.o tim.o tmd.o vector.o -o tuto15

tuto15.o: tuto15.c tim.h pad.h tmd.h sincos.h asssert.h	dump.h
	gcc -c $(C_FLAGS) tuto15.c


tuto14: tuto14.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto14.o pad.o tim.o tmd.o vector.o -o tuto14

tuto14.o: tuto14.c tim.h pad.h tmd.h sincos.h asssert.h	dump.h
	gcc -c $(C_FLAGS) tuto14.c


tuto13: tuto13.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto13.o pad.o tim.o tmd.o vector.o -o tuto13

tuto13.o: tuto13.c tim.h pad.h tmd.h sincos.h asssert.h	dump.h
	gcc -c $(C_FLAGS) tuto13.c


tuto12: tuto12.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto12.o pad.o tim.o tmd.o vector.o  -o tuto12

tuto12.o: tuto12.c tim.h pad.h tmd.h sincos.h asssert.h
	gcc -c $(C_FLAGS) tuto12.c


tuto11: tuto11.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto11.o pad.o tim.o tmd.o vector.o -o tuto11

tuto11.o: tuto11.c tim.h pad.h tmd.h sincos.h vector.h asssert.h
	gcc -c $(C_FLAGS) tuto11.c



tuto10: tuto10.o pad.o tim.o tmd.o vector.o
	gcc $(LINKER) tuto10.o pad.o tim.o tmd.o vector.o -o tuto10

tuto10.o: tuto10.c tim.h pad.h tmd.h sincos.h vector.h asssert.h
	gcc -c $(C_FLAGS) tuto10.c



  
tuto9: tuto9.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto9.o pad.o tim.o tmd.o -o tuto9

tuto9.o: tuto9.c tim.h pad.h tmd.h sincos.h asssert.h
	gcc -c $(C_FLAGS) tuto9.c


   
tuto8: tuto8.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto8.o pad.o tim.o tmd.o -o tuto8

tuto8.o: tuto8.c tim.h pad.h tmd.h sincos.h asssert.h
	gcc -c $(C_FLAGS) tuto8.c




tuto7: tuto7.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto7.o pad.o tim.o tmd.o -o tuto7

tuto7.o: tuto7.c tim.h pad.h tmd.h sincos.h asssert.h
	gcc -c $(C_FLAGS) tuto7.c



tuto6: tuto6.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto6.o pad.o tim.o tmd.o -o tuto6

tuto6.o: tuto6.c tim.h pad.h tmd.h sincos.h asssert.h
	gcc -c $(C_FLAGS) tuto6.c




tuto5: tuto5.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto5.o pad.o tim.o tmd.o -o tuto5

tuto5.o: tuto5.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto5.c



tuto4: tuto4.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto4.o pad.o tim.o tmd.o -o tuto4

tuto4.o: tuto4.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto4.c


tuto3: tuto3.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto3.o pad.o tim.o tmd.o -o tuto3

tuto3.o: tuto3.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto3.c



tuto2: tuto2.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto2.o pad.o tim.o tmd.o -o tuto2

tuto2.o: tuto2.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto2.c



tuto1: tuto1.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto1.o pad.o tim.o tmd.o -o tuto1

tuto1.o: tuto1.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto1.c



tuto0: tuto0.o pad.o tim.o tmd.o
	gcc $(LINKER) tuto0.o pad.o tim.o tmd.o -o tuto0

tuto0.o: tuto0.c tim.h pad.h tmd.h asssert.h
	gcc -c $(C_FLAGS) tuto0.c



pad.o: pad.c pad.h
	gcc -c $(C_FLAGS) pad.c

tim.o: tim.c tim.h
	gcc -c $(C_FLAGS) tim.c

tmd.o: tmd.c tmd.h
	gcc -c $(C_FLAGS) tmd.c

vector.o: vector.c vector.h
	gcc -c $(C_FLAGS) vector.c





#########			OLD
#clean:
#	$(RM) $(PROJECT)
#	$(RM) $(OBJECT_FILES)
##########