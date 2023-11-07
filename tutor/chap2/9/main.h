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
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 512

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

extern int output_buffer_index;            // buffer index
extern GsOT world_ordering_table[2];       // ordering table headers
extern GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
extern PACKET gpu_work_area[2][24000];     // GPU packet work area
extern u_char prev_mode;				   // previous code
extern int fnt_id[9]; 

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void ReadTIM( u_long *addr );
void InitGame( void );
void DeInitGame( void );
void UpdateScreen( void );
void InitSprite( void );

#endif

//----------------------------------EOF-------------------------------------
