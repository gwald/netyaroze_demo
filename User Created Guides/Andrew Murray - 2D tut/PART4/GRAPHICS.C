//
// graphics.h
// 
// Encaptulation of graphics functionality for 
// the playstation.
// 
// Andrew Murray - 15th November 2002
// 

#include <libps.h>

#include "graphics.h"

// --------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------

// declare a graphics structure to handle 
// the double buffers fonts and workspace
Graphics gfx;

// --------------------------------------------------------------------------------
// Function Definitions
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Name:	InitGfx()
// Desc:	Initialise the screen res and mode, draw environment, double buffers 
// 			and	debug font.
// --------------------------------------------------------------------------------
void InitGfx(Graphics *gfx)
{
	// enable pal resolutions
	SetVideoMode(MODE_PAL);                     

	// initilise the graphics engine
	GsInitGraph(SCRNW, SCRNH, GsNONINTER|GsOFSGPU, 0, 0);
	
	// set up the display buffers coords
	GsDefDispBuff(0,0,0,SCRNH);
	
	// set up the ordering table lengths and the headers for them
	gfx->ot[0].length = OTLEN;                      
	gfx->ot[0].org = gfx->ottags[0];
	gfx->ot[1].length = OTLEN;                      
	gfx->ot[1].org = gfx->ottags[1];
	
	// initialise the OTs
	GsClearOt(0, 0, &gfx->ot[0]);
	GsClearOt(0, 0, &gfx->ot[1]);

	// fix the background clearing problem
	GsDRAWENV.isbg = 1;
	
	// display environment initial position stored in 
	// variables to allow moving of display environment
	gfx->screenPosX = 5;
	gfx->screenPosY = 18;
	
	// set the initial display environment parameters
	// this is required as PAL resolutions require
	// extra initilisation code to display to the
	// full extent of the requested resolution
	GsDISPENV.screen.x = gfx->screenPosX;
	GsDISPENV.screen.y = gfx->screenPosY;
	
	// change the screen height & width from the libps 
	// header to compensate for the PAL resolution
	GsDISPENV.screen.w = SCRNW;
	GsDISPENV.screen.h = SCRNH;

	// removed from LoadFont as multiple font ids
	// will not work due to an extra call to FntLoad()
	FntLoad(960,256);                           

	// create a font display area
	gfx->fntID[0] = OpenDebugFont(0, 0, SCRNW, SCRNH);
}

// --------------------------------------------------------------------------------
// Name:	OpenDebugFont
// Desc:	Open a font to the screen and return an ID that allows access to it.
// --------------------------------------------------------------------------------
int OpenDebugFont(int x1, int y1, int width, int height)
{
	return(FntOpen(x1, y1, width, height, 0, 512));
}

// --------------------------------------------------------------------------------
// Name:	StartFrame()
// Desc:	Function initialises the ordering tables and working memory for the 
// 			start of a new frame.
// --------------------------------------------------------------------------------
void StartFrame(Graphics *gfx)
{
	// get the index of the current display buffer
	gfx->buf = GsGetActiveBuff();
	
	// set the buffer pointer to the current display buffer
	gfx->pOT = &gfx->ot[gfx->buf];
	
	// set the packet work area to the packet area for the current buffer
	GsSetWorkBase((PACKET *)gfx->gpuPktArea[gfx->buf]);
	
	// initialise the current ordering table for drawing
	GsClearOt(0, 0, gfx->pOT);
}

// --------------------------------------------------------------------------------
// Name:	EndFrame()
// Desc:	Function concludes rendering of the current frame by sending the 
// 			current ordering table to the GPU (synchronised with the VSync 
// 			interrupt)
// --------------------------------------------------------------------------------
void EndFrame(Graphics *gfx)
{
	// draw all font calls to the screen
	FntFlush(gfx->fntID[0]);
	
	// execute all non-blocking GPU drawing commands
	DrawSync(0);
	
	// block until vsync occurs - makes sure everything
	// for current frame is drawn before going onto next
	VSync(0);
	
	// swap the double buffers
	GsSwapDispBuff();
	
	// clear the buffer to black
	GsSortClear(0, 0, 0, gfx->pOT);
	
	// execute ordering table drawing commands for
	// appropriate graphics buffer
	GsDrawOt(gfx->pOT);
}
