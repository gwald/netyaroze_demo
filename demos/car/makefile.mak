#all:
#	ccpsx -O2 -Xo$800a0000 main.c -omain.cpe,main.sym
#
#for_menu:
#	ccpsx -O2 -Xo$800a0000 main.c \psx\lib\none3.obj -o60frame.cpe
#	cpe2x 60frame.cpe
#	del   60frame.cpe
#
#load:
#	pqbload giuli_bd.tim 80010000
#	pqbload giuli_fr.tim 80020000
#	pqbload giuli_pl.tim 80030000
#	pqbload giuli_tl.tim 80040000
#	pqbload giuli_tr.tim 80050000
#	pqbload giuli_wn.tim 80060000
#	pqbload wheel256.tim 80070000
#	pqbload giulieta.tmd 80080000

main: main.o
	gcc -Xlinker -Ttext -Xlinker 80090000 -o main.exe main.o

#   main: main.o
#       ld -o main.exe main.o

main.o: main.c
	gcc -c main.c


