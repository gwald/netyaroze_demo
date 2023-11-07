C_FLAGS  = -O3
LINKER  = -Xlinker -Map -Xlinker mapfile -Xlinker -Ttext -Xlinker 80100000

  



	  
all: pad.o tim.o tmd.o vector.o	sincos.o object.o tangent.o tuto7
  



tuto7: tuto7.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto7.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto7

tuto7.o: tuto7.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h tangent.h
	gcc -c $(C_FLAGS) tuto7.c


tuto6: tuto6.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto6.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto6

tuto6.o: tuto6.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h
	gcc -c $(C_FLAGS) tuto6.c



tuto5: tuto5.o pad.o tim.o tmd.o sincos.o vector.o object.o	tangent.o
	gcc $(LINKER) tuto5.o pad.o tim.o tmd.o vector.o object.o sincos.o tangent.o -o tuto5

tuto5.o: tuto5.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h tangent.h
	gcc -c $(C_FLAGS) tuto5.c




tuto4: tuto4.o pad.o tim.o tmd.o sincos.o vector.o object.o	tangent.o
	gcc $(LINKER) tuto4.o pad.o tim.o tmd.o vector.o object.o sincos.o tangent.o -o tuto4

tuto4.o: tuto4.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h tangent.h
	gcc -c $(C_FLAGS) tuto4.c



tuto3: tuto3.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto3.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto3

tuto3.o: tuto3.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h
	gcc -c $(C_FLAGS) tuto3.c


tuto2: tuto2.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto2.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto2

tuto2.o: tuto2.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h
	gcc -c $(C_FLAGS) tuto2.c



tuto1: tuto1.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto1.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto1

tuto1.o: tuto1.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h
	gcc -c $(C_FLAGS) tuto1.c
	

tuto0: tuto0.o pad.o tim.o tmd.o sincos.o vector.o object.o
	gcc $(LINKER) tuto0.o pad.o tim.o tmd.o vector.o object.o sincos.o -o tuto0

tuto0.o: tuto0.c tim.h pad.h tmd.h asssert.h vector.h dump.h object.h sincos.h
	gcc -c $(C_FLAGS) tuto0.c



pad.o: pad.c pad.h
	gcc -c $(C_FLAGS) pad.c

tim.o: tim.c tim.h
	gcc -c $(C_FLAGS) tim.c

tmd.o: tmd.c tmd.h
	gcc -c $(C_FLAGS) tmd.c

vector.o: vector.c vector.h
	gcc -c $(C_FLAGS) vector.c

sincos.o: sincos.c sincos.h
	gcc -c $(C_FLAGS) sincos.c

tangent.o: tangent.c tangent.h
	gcc -c $(C_FLAGS) tangent.c

object.o: object.c object.h
	gcc -c $(C_FLAGS) object.c



	




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