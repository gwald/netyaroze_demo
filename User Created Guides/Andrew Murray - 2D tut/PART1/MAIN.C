//
// main1.c
// 
// Double buffering
// 
// Andrew Murray - 19th March 2001
// 

#include <libps.h>
#include <stdio.h>

// screen height & width
#define SCRNW	320
#define SCRNH	256

// ordering table length
#define OT_LENGTH	9

// current frame number
int g_frameNo;

// holds the ID of the open font stream
// used to pass to FntPrint so it can
// identify where on the screen to print
int g_fntID;

// ordering table headers
GsOT		g_WorldOT[2];
// ordering table elements
GsOT_TAG	g_WorldTags[2][1 << OT_LENGTH];

// gpu packet work area
static PACKET g_packetArea[2][24 * 100];


void InitGfx(void)
{
	// this allows use of PAL resolutions
	SetVideoMode(MODE_PAL);
	
	// initialise the graphics engine with 320 x 240 resolution
	GsInitGraph(SCRNW, SCRNH, GsNONINTER|GsOFSGPU, 1, 0);
	
	// define the buffer positions on VRAM
	GsDefDispBuff(0, 0, 0, SCRNH);
	
	// set OT length and link the OT 
	//elements (the GsOT_TAG structures)
	g_WorldOT[0].length = OT_LENGTH;
	g_WorldOT[0].org = g_WorldTags[0];
	g_WorldOT[1].length = OT_LENGTH;
	g_WorldOT[1].org = g_WorldTags[1];
	
	// initialise the OTs
	GsClearOt(0, 0, &g_WorldOT[0]);
	GsClearOt(0, 0, &g_WorldOT[1]);

	// set the initial display environment parameters
	// this is required as PAL resolutions require
	// extra initilisation code to display to the
	// full extent of the requested resolution
	GsDISPENV.screen.x = 5;
	GsDISPENV.screen.y = 18;
	
	GsDISPENV.screen.w = SCRNW;
	GsDISPENV.screen.h = SCRNH;
	
	// load the font into VRAM
	FntLoad(960, 256);
	
	// open the font to the screen
	g_fntID = FntOpen(10, 80, 256, 200, 0, 512);
}

int main()
{
	// buffer index
	int buffer = 0;
	
	// set up the graphics
	InitGfx();
	
	// grab the active buffer on which to commence drawing
	buffer = GsGetActiveBuff();
	
	printf("\nStarting!\n");
	// loop through 200 times and then terminate
	while(g_frameNo < 200)
	{
		// increment frame count
		g_frameNo++;
		
		// set the packet work area to the packet area for the current buffer
		GsSetWorkBase((PACKET*)g_packetArea[buffer]);
		
		// initialise the current ordering table for drawing
		GsClearOt(0, 0, &g_WorldOT[buffer]);
		
		// print the frame number to the screen
		FntPrint(g_fntID, "frame: %d\n", g_frameNo);

		// print the font call
		FntFlush(g_fntID);
		
		// execute all non-blocking GPU drawing commands
		DrawSync(0);
		
		// block until vsync occurs - makes sure everything
		// for current frame is drawn before going onto next
		VSync(0);
				
		// swap the double buffers
		GsSwapDispBuff();

		// clear the buffer to black
		GsSortClear(0, 0, 0, &g_WorldOT[buffer]);

		// execute ordering table drawing commands for
		// appropriate graphics buffer
		GsDrawOt(&g_WorldOT[buffer]);
		
		// change buffer to 1 or 0
		buffer = buffer^1;

		printf(".");
	}
	printf("\nfinished!\n");
	ResetGraph(0);
}
