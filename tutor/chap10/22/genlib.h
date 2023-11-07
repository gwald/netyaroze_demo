//--------------------------------------------------------------------------
// File: gamelib.h
// Author: George Bain
// Date: March 1999  
// Description: Prototypes
// Copyright (C) 1999 Sony Computer Entertainment Europe.,
//           All Rights Reserved. 
//--------------------------------------------------------------------------

#ifndef _GENLIB_H
#define _GENLIB_H

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitTMD( GsDOBJ2 *obj, GsCOORDINATE2 *coord, u_long *addr );
void SortObject( GsDOBJ2 *obj, GsOT *wot, int shift );
void UpdateCoordinates( SVECTOR *pos, GsCOORDINATE2 *coor);
void InitSprite( GsSPRITE *sprite, u_long attribute, short x, short y, u_short w, u_short h,
                 u_short tpage, u_char u, u_char v, short cx, short cy, u_char r, u_char g,
                 u_char b, short mx, short my, short scalex, short scaley, long rotate );
GsIMAGE *ReadTIM( u_long *addr );
void Sort3DLine( GsLINE *line, GsCOORDINATE2 *coord, VECTOR *point1, VECTOR *point2);


#endif

//----------------------------------EOF-------------------------------------
