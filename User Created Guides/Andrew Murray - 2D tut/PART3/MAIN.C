// 
// main.c
// 
// Sprite display example
// 
// Andrew Murray - 26th July 2003
// 

#include <libps.h>

#include "pad.h"
#include "sprite.h"
#include "graphics.h"

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

// check to see if the true and false
// keywords are defined (this depends on 
// the compiler) if not then define them
#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

// ordering table length
#define OT_LENGTH	9

// player sprite's drawing priority
#define PLAYER_OT_PRI	2

// the location of the sprite in memory
#define REDMAN_ADDR		0x80090000

// --------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------

// current frame number
int frameNo;

// holds the run state of the demo
int g_finished = false;

// holds the pause state of the demo
int g_paused = false;

// image instance for storing image data
GsIMAGE g_image;

// sprite instance for handling image data in the game
GsSPRITE g_sprite;

// a buffer to load the tim file into
u_char g_imgBuffer[KBYTE];

// --------------------------------------------------------------------------------
// Function Definitions
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Name:	ProcessInput()
// Desc:	
// --------------------------------------------------------------------------------
void ProcessInput()
{
	// instance of the pad data structure
	// although 2 are defined (for both pads)
	// only one is used in this function
	static PadInfo padStatus[2];

	// request the input from the pad in port 1
	GetPadData(0, &padStatus[0]);

	if(PadConnected(0) == PadIn)
	{
		// determine what type of pad is connected
		switch(PadType(0))
		{
			// standard playstation digital pad
			case PadStandard:	
				FntPrint(gfx.fntID[0], "STANDARD PAD DETECTED\n");
				break;
			// analogue playstation pad
			case PadAnalogue:
				FntPrint(gfx.fntID[0], "ANALOGUE PAD DETECTED\n");
				break;
			// dual shock playstation pad
			case PadDualShock:
				FntPrint(gfx.fntID[0], "DUALSHOCK PAD DETECTED\n");
				break;
		}
		
		// start pressed
		if(padStatus[0].press & PadStart)
		{
			g_paused = !g_paused;
		}
			
		// up pressed - move sprite up
		if(padStatus[0].state & PadUp)
			g_sprite.y -= 1;
			
		// down pressed - move sprite down
		if(padStatus[0].state & PadDown)
			g_sprite.y += 1;
		
		// left pressed - move sprite left
		if(padStatus[0].state & PadLeft)
			g_sprite.x -= 1;

		// right pressed - move sprite right
		if(padStatus[0].state & PadRight)
			g_sprite.x += 1;

		// test for program terminate condition
		if((padStatus[0].state & PadStart) && 
			(padStatus[0].state & PadSelect))
			g_finished = true;
	}
	else
	{
		FntPrint(gfx.fntID[0], "PORT 1: PAD NOT CONNECTED\n");
	}
}

// --------------------------------------------------------------------------------
// Name:	main()
// Desc:	Point of execution
// --------------------------------------------------------------------------------
int main()
{
	// set up the graphics
	InitGfx(&gfx);
	
	// set up the control pad buffers
	InitPads();
	
	// load the tim file and set up the GsIMAGE info
	LoadSprite(&g_image, (unsigned long*)REDMAN_ADDR);
	
	// set up the GsSPRITE using the GsIMAGE
	InitSprite(&g_sprite, &g_image);
	
	// set the sprite initial position of 
	// the sprite to the centre of the screen
	// bitshift 1 ( >> 1) is the same as dividing by two
	g_sprite.x = (SCRNW >> 1) - (g_sprite.w >> 1);
	g_sprite.y = (SCRNH >> 1) - (g_sprite.h >> 1);
	
	// terminate only when told to
	while(!g_finished)
	{
		// set up the hardware for a new frame
		StartFrame(&gfx);

		if(!g_paused)
		{
			// increment frame count
			frameNo++;
		}

		// process the user input
		ProcessInput();

		// print the frame number to the screen
		FntPrint(gfx.fntID[0], "frame: %d\n", frameNo);
		
		// draw the sprite
		GsSortFastSprite(&g_sprite, gfx.pOT, PLAYER_OT_PRI);
		
		// end the current frame drawing
		EndFrame(&gfx);
	}
	
	// reset the graphics system
	// and blank the screen
	ResetGraph(0);
}
