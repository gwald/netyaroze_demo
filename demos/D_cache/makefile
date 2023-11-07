#Makefile for Plasma (blur2) demo


all: main.o plasma.o
	gcc main.o plasma.o cdfeed.o -oblur2

main.o: main.c
	gcc -O2 -c main.c

plasma.o: plasma.s
	gcc -c -x assembler-with-cpp plasma.s
