/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                         *
*                                                              *
***************************************************************/

#define _blowup_h

#include <libps.h>

#ifndef _tmd_h
	#include "tmd.h"
#endif

#ifndef _tmdutil_h
	#include "tmdutil.h"
#endif

void blowVertices(GsDOBJ2 obj, u_char speed, int count);
void explosionColor(int c, CVECTOR *col);
void explosionClut(int c, u_short *clut);
void separateVertices(u_long *tmdAddrIn, u_long *tmdAddrOut);
void resetVertices(u_long *tmdAddrIn, u_long *tmdAddrOut, u_long objectNumber);
void resetColors(u_long *tmdAddrIn, u_long *tmdAddrOut, u_long objectNumber);
