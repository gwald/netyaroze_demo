//
//	sincos.h
//
//
// Copyright (c) 1996 by Sony Computer Entertainment Inc.
// All Rights Reserved.
//


	   
	// prototypes for sincos.c: the access functions
	
int rsin_sub(int angle);
int rsin (int angle);
int rcos (int angle);
double quickSin (int angle);
double quickCos (int angle);


	// the LUT itself
extern short SinCosTable[];