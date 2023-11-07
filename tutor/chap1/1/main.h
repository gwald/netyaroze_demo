//--------------------------------------------------------------------------
// File: main.h
// Author: George Bain
// Date: August 3, 1998
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
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitGame( void );
void DeInitGame( void );
void UpdateScreen( void ); 
void CheckGunCon( cntrl_data *port );
void CheckNegcon( cntrl_data *port );
void CheckController( cntrl_data *port );
void CheckMouse( cntrl_data *port );
void CheckTerminalType( void );

#endif


//----------------------------------EOF-------------------------------------
