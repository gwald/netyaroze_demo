/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 9th Jan 97                    *
*                                                              *
***************************************************************/

#ifndef _sincos_h

#define _sincos_h

#include <libps.h>	   
	
long rinvtan(long x, long y);
int rsin (int angle);
int rcos (int angle);

extern short SinTable[];
extern short Tinvtan[];

#endif