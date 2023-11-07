//--------------------------------------------------------------------------
// File: sound.h
// Author: George Bain
// Date: July 24, 1998
// Description: Prototypes
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//-------------------------------------------------------------------------- 

#ifndef _SOUND_H
#define _SOUND_H

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//-------------------------------------------------------------------------- 

void InitSound( void );
void PlayMusic( void );
void StopSound( void );
void InitSFX( voice_ptr sound_effect, short vab_id, short program, 
              short tone, short note, short fine, short vol_l, short vol_r );
void PlaySFX( voice_ptr sound_effect );
void SetupReverb( void ); 

#endif

//----------------------------------EOF-------------------------------------
