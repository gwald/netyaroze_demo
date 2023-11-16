/***************************************************************
	 main.c
	 ======
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <libps.h>
#include <string.h>
#include "pad.h"

#define ORDERING_TABLE_LENGTH (12)
#define MAX_NO_PACKETS  (248000)

#define SCREEN_WIDTH  (320)
#define SCREEN_HEIGHT (240)


// We need two Ordering Table Headers, one for each buffer
GsOT othWorld[2];
// And we need Two Ordering Tables, one for each buffer
GsOT_TAG        otWorld[2][1<<ORDERING_TABLE_LENGTH];
//We also allocate memory used for depth sorting, a block for each buffer
PACKET          out_packet[2][MAX_NO_PACKETS];
// we need a variable to store the status of the joypad
u_long  PADstatus=0;


// this function initialises the graphics system
void InitialiseGraphics()
{
	 // Initialise The Graphics System to PAL as opposed to NTSC
	 SetVideoMode(MODE_PAL);
	 // Set the Actual Size of the Video memory
	 GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	 // Set the Top Left Coordinates Of The Two Buffers in video memory
	 GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);
    // Initialise the 3D Graphics...
	 GsInit3D();
    // Before we can use the ordering table headers, 
	 // we need to...
    // 1. Set them to the right length
	 othWorld[0].length = ORDERING_TABLE_LENGTH;
	 othWorld[1].length = ORDERING_TABLE_LENGTH;
    // 2. Associate them with an actual ordering table 
	 othWorld[0].org = otWorld[0];
	 othWorld[1].org = otWorld[1];
	 // 3. initialise the World Ordering Table Headers and Arrays
	 GsClearOt(0,0,&othWorld[0]);
	 GsClearOt(0,0,&othWorld[1]);
}
// This function deals with double buffering (ordering tables etc)
// and writes text to the PSX screen.
void RenderWorld()
{
// This variable keeps track of the current buffer for double buffering
int currentBuffer;
		//get the current buffer
		currentBuffer=GsGetActiveBuff();
		// set address for GPU scratchpad area
		GsSetWorkBase((PACKET*)out_packet[currentBuffer]);
		// clear the ordering table
		GsClearOt(0, 0, &othWorld[currentBuffer]);
		//print your elegant message
		FntPrint("Hello World!\n");
    	// force text output to the PSX screen
		FntFlush(-1);
		// wait for end of drawing
		DrawSync(0);
		// wait for V_BLANK interrupt
		VSync(0);
		// swap double buffers
		GsSwapDispBuff();
		// register clear-command: clear to black
		GsSortClear(0, 0, 0,&othWorld[currentBuffer]);
		// register request to draw ordering table
		GsDrawOt(&othWorld[currentBuffer]);
}

int main()
{
	// set up print-to-screen font, the parameters are where the font is loaded
	// into the frame buffer
	FntLoad(960, 256);
	//specify where to write on the PSX screen
	FntOpen(-96, -96, 192, 192, 0, 512);
	// initialise the joypad
	PadInit();
	// initialise graphics
	InitialiseGraphics();
	while(1){
		// read the joypad
		PADstatus=PadRead();
		// if the select button has been pressed then break out of while loop
		if (PADstatus & PADselect) break;
		//other wise do double buffering
		RenderWorld();
		}
	// clean up
	ResetGraph(3);
	return 0;
}


