//--------------------------------------------------------------------------
// File: main.h
// Author: George Bain
// Date: June 17, 1998
// Description: Prototypes
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

#ifndef _MAIN_H
#define _MAIN_H
//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitGame( void );
void DeInitGame( void );
void UpdateScreen( void );
void InitSprite( GsSPRITE *sprite, u_long attribute, short x, short y, u_short w, u_short h,
                 u_short tpage, u_char u, u_char v, short cx, short cy, u_char r, u_char g,
                 u_char b, short mx, short my, short scalex, short scaley, long rotate );
GsIMAGE *ReadTIM( u_long *addr );
void InitYarozeSprite( void ); 

#endif

//----------------------------------EOF-------------------------------------
