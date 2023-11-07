all: tuto3 tuto4 tuto5



tuto3: tuto3.o
	gcc -Xlinker -Ttext -Xlinker 80090000 -o tuto3.exe tuto3.o


tuto3.o: tuto3.c
	gcc -c tuto3.c





tuto4: tuto4.o
	gcc -Xlinker -Ttext -Xlinker 80090000 -o tuto4.exe tuto4.o


tuto4.o: tuto4.c
	gcc -c tuto4.c





tuto5: tuto5.o
	gcc -Xlinker -Ttext -Xlinker 80090000 -o tuto5.exe tuto5.o


tuto5.o: tuto5.c
	gcc -c tuto5.c

