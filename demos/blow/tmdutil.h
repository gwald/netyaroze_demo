/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                         *
*                                                              *
***************************************************************/

#define _tmdutil_h

#include <libps.h>

#ifndef _tmd_h
	#include"tmd.h"
#endif

void TMDLimits(GsDOBJ2 obj,VECTOR *min,VECTOR *max);
u_char getTMDVertices(PRIM_HDR *prim, u_short *vertex, PRIM_HDR *(*next));
u_char setTMDVertices(PRIM_HDR *prim, u_short *vertex, PRIM_HDR *(*next));
u_char getTMDNormals(PRIM_HDR *prim, u_short *normal, PRIM_HDR *(*next));
u_char setTMDNormals(PRIM_HDR *prim, u_short *normal, PRIM_HDR *(*next));
u_char getTMDColors(PRIM_HDR *prim, CVECTOR *col, PRIM_HDR *(*next));
u_char setTMDColors(PRIM_HDR *prim, CVECTOR *col, PRIM_HDR *(*next));
u_char getTMDTextureInfo(PRIM_HDR *prim, u_short *cba, u_short *tsb, PRIM_HDR *(*next));
u_char setTMDTextureInfo(PRIM_HDR *prim, u_short cba, u_short tsb, PRIM_HDR *(*next));
u_char getTMDPrimLength(PRIM_HDR *prim, PRIM_HDR *(*next));
u_char copyTMDPrim(PRIM_HDR *src, PRIM_HDR *dst, PRIM_HDR *(*srcNext), PRIM_HDR *(*dstNext));

