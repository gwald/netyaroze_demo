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
// D E F I N E S 
//-------------------------------------------------------------------------- 

// screen resolution
#define SCREEN_WIDTH  512
#define SCREEN_HEIGHT 256

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
// S T R U C T U R E S 
//--------------------------------------------------------------------------

typedef struct voice_tag
 { 	
 	
	short vabid;
	short program;
	short tone;
	short note;
	short fine;
	short vol_l;
	short vol_r;

 }voice_data, *voice_ptr;

voice_data siren,helli;  // sound effects

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitSound( void );
void StopSound( void );
void InitSFX( voice_ptr sound_effect, short vab_id, short program, 
              short tone, short note, short fine, short vol_l, short vol_r );
void PlaySFX( voice_ptr sound_effect );	

#endif

//----------------------------------EOF-------------------------------------
