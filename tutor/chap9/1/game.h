//--------------------------------------------------------------------------
// File: game.h
// Author: George Bain
// Date: July 24, 1998
// Description: Prototypes
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//-------------------------------------------------------------------------- 

#ifndef _GAME_H
#define _GAME_H

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitGame( void );
void DeInitGame( void );
void UpdateScreen( void ); 
void PlayGame( void );
void ControllerStatus( void );
void SetupGame( void );
int  CountCircles( void );
void ResetGame( void );

#endif

//----------------------------------EOF-------------------------------------
