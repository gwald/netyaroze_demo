#all:
#	ccpsx -O -Xo$80080000 oden.c -ooden.cpe,oden.sym
#for_menu:
#	ccpsx -O -Xo$80080000 \psx\lib\none2.obj oden.c -ooden.cpe
#	cpe2x oden.cpe
#	del   oden.cpe
#load:
#   pqbload oden.tim 801c0000
#		.. starts at 1M+832K
#	pqbload oden.tmd 80040000
#		.. starts at 256K




## changes on blackpsx:
## programs start at 80140000 not 80010000
## ie at (1M + 256K)   not 64K
## hence must add 00130000 to starting address for tim/tmd bloads




oden: oden.o
	gcc -Xlinker -Ttext -Xlinker 801b0000 -o oden.exe oden.o

#   oden: oden.o
#       ld -o oden.exe oden.o

oden.o: oden.c
	gcc -c oden.c

