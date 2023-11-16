// 
// main.c
// 
// Sprite animation example
// 
// Andrew Murray - 26th July 2003
// 

#include <libps.h>

#include "pad.h"
#include "sprite.h"
#include "graphics.h"
#include "player.h"

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

// the location of the sprite in memory
#define MANANIM_ADDR		0x80090000

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
		{
			// move the player up
			g_player.spr.y -= 1;

			// set the direction to north (travelling upward)
			g_player.dir = N;			
		}
			
		// down pressed - move sprite down
		if(padStatus[0].state & PadDown)
		{
			// move the player down
			g_player.spr.y += 1;

			// set the direction to south (travelling down)
			g_player.dir = S;
		}
		
		// left pressed - move sprite left
		if(padStatus[0].state & PadLeft)
		{
			// move the player left
			g_player.spr.x -= 1;

			// test for a diagonal direction
			if(g_player.dir == N)
				g_player.dir = NW;
			else
				if(g_player.dir == S)
					g_player.dir = SW;
				else
					g_player.dir = W;
		}

		// right pressed - move sprite right
		if(padStatus[0].state & PadRight)
		{
			// move the player right
			g_player.spr.x += 1;
			
			// test for a diagonal direction
			if(g_player.dir == N)
				g_player.dir = NE;
			else
				if(g_player.dir == S)
					g_player.dir = SE;
				else
					g_player.dir = E;
		}
		
		// if the d-pad has been pressed at all
		if(padStatus[0].state & PadDPad)
		{
			// then we want to animate
			g_player.anim = 1;
		}			

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
	LoadSprite(&g_image, (unsigned long*)MANANIM_ADDR);
	
	// set up the player using the GsIMAGE just acquired
	InitPlayer(&g_player, &g_image, 0, (SCRNW >> 1), (SCRNH >> 1));
	
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
		
		// print the sprite x, y position to the screen
		FntPrint(gfx.fntID[0], "sprite x: %d, y: %d\n", g_player.spr.x, g_player.spr.y);
		
		// draw the animated player sprite
		DrawPlayer(&g_player, gfx.pOT);
		
		// end the current frame drawing
		EndFrame(&gfx);
	}
	
	// reset the graphics system
	// and blank the screen
	ResetGraph(0);
}
