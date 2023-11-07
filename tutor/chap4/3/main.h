//--------------------------------------------------------------------------
// File: main.h
// Author: George Bain
// Date: July 10, 1998
// Description: Prototypes
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//-------------------------------------------------------------------------- 

#ifndef _MAIN_H
#define _MAIN_H

//--------------------------------------------------------------------------
// D E F I N E S 
//-------------------------------------------------------------------------- 

// screen resolution
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 256

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

extern int output_buffer_index;            // buffer index
extern GsOT world_ordering_table[2];       // ordering table headers
extern GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
extern PACKET gpu_work_area[2][24000];     // GPU packet work area
extern u_char prev_mode;				   // previous code
extern int fnt_id[9]; 					   // font id

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

GsIMAGE *ReadTIM( u_long *addr );
void InitGame( void );
void DeInitGame( void );
void UpdateScreen( void );
void InitSprite( GsSPRITE *sprite, u_long attribute, short x, short y, u_short w, u_short h,
                 u_short tpage, u_char u, u_char v, short cx, short cy, u_char r, u_char g,
                 u_char b, short mx, short my, short scalex, short scaley, long rotate );
int SphereCollision( GsSPRITE *sprite, GsSPRITE *sprite_two);
void InitSquareSprite( void );

#endif

//----------------------------------EOF-------------------------------------
