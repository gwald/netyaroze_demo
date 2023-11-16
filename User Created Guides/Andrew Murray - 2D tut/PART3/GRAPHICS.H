//
// graphics.h
// 
// Encaptulation of graphics functionality for 
// the playstation.
// 
// Andrew Murray - 15th November 2002
// 

#ifndef GRAPHICS_H
#define GRAPHICS_H

// ----------------------------------------------
// Defines
// ----------------------------------------------

#define SCRNW	(320)
#define SCRNH	(256)
#define KBYTE	(1024)
#define OTLEN	(9)

// ----------------------------------------------
// Structures
// ----------------------------------------------

// This stucture holds all the data used for double 
// buffering, the font id(s) and the work area
typedef struct
{
	GsOT ot[2];
	GsOT *pOT;
	GsOT_TAG ottags[2][1<<OTLEN];

	PACKET gpuPktArea[2][64*KBYTE];

	int buf;
	int fntID[1];
	
	// these hold the GsDISPENV position on the screen
	// this is required to properly initialise PAL resoloutions
	int screenPosX;
	int screenPosY;
}Graphics;

// ----------------------------------------------
// Globals
// ----------------------------------------------

extern Graphics gfx;

// ----------------------------------------------
// Prototypes
// ----------------------------------------------

void	InitGfx			(Graphics *gfx);
int		OpenDebugFont	(int x1, int y1, int width, int height);
void	StartFrame		(Graphics *gfx);
void	EndFrame		(Graphics *gfx);

#endif
