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
#define CAR_MEM_ADDR  (0x80090000)

// we need a variable to store the status of the joypad
u_long PADstatus=0;
// We need two Ordering Table Headers, one for each buffer
GsOT othWorld[2];
// And we need Two Ordering Tables, one for each buffer
GsOT_TAG        otWorld[2][1<<ORDERING_TABLE_LENGTH];
//We also allocate memory used for depth sorting, a block for each buffer
PACKET          out_packet[2][MAX_NO_PACKETS];
// We need a viewing system for 3D graphics
GsRVIEW2        view;
// This is an array of structures for the lights
GsF_LIGHT       flLights[2];

// Define a structure to hold a 3D object that will be rendered on screen
typedef struct
{
	 GsDOBJ2         gsObjectHandler;
	 GsCOORDINATE2   gsObjectCoord;
}PlayerStructType0;
// Create an instance of the structure to hold our car object 
PlayerStructType0 theCar;

/************* FUNCTION PROTOTYPES *******************/
void AddModelToPlayer(PlayerStructType0 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress);
void DrawPlayer(PlayerStructType0 *thePlayer, GsOT *othWorld);
void InitialiseLight(GsF_LIGHT *flLight, int nLight, int nX, int nY, int nZ,
		int nRed, int nGreen, int nBlue);
void InitialiseAllLights(void);
void InitialiseView(GsRVIEW2 *view, int nProjDist, int nRZ,
						  int nVPX, int nVPY, int nVPZ,
						  int nVRX, int nVRY, int nVRZ);
void InitialiseGraphics(void);
void RenderWorld(void);
/*****************************************************/

// This function associates a model with our player datastructure
void AddModelToPlayer(PlayerStructType0 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress)
{
	//increment the pointer to past the model id. (weird huh?)
	lModelAddress++;
	// map tmd data to its actual address
	GsMapModelingData(lModelAddress);
	// initialise the players coordinate system - set to be that of the
	//world
	GsInitCoordinate2(WORLD, &thePlayer->gsObjectCoord);
	// increment pointer twice more - to point to top of model data
	// (beats me!)
	lModelAddress++;  lModelAddress++;
	// link the model (tmd) with the players object handler
	GsLinkObject4((long unsigned int)lModelAddress,
				&thePlayer->gsObjectHandler,0);
	// Assign the coordinates of the object model to the Object Handler
	thePlayer->gsObjectHandler.coord2 = &thePlayer->gsObjectCoord;
	// Set the initial position of the object
	thePlayer->gsObjectCoord.coord.t[0]=nX;   // X
	thePlayer->gsObjectCoord.coord.t[1]=nY;   // Y
	thePlayer->gsObjectCoord.coord.t[2]=nZ;   // Z
	// setting the players gsObjectCoord.flg to 0 indicates it is to be
	// drawn
	thePlayer->gsObjectCoord.flg = 0;
}
// This function deals with setting up matrices needed for rendering
// and sends the object to the ordering table so it is drawn
void DrawPlayer(PlayerStructType0 *thePlayer, GsOT *othWorld)
{
	 MATRIX  tmpls, tmplw;
	 GsDOBJ2 *tmpPtr;
	 // Set The Local World/Screen MATRIX
	 GsGetLws(thePlayer->gsObjectHandler.coord2, &tmplw, &tmpls);
	 GsSetLightMatrix(&tmplw);
	 GsSetLsMatrix(&tmpls);
	 tmpPtr = &thePlayer->gsObjectHandler;
	 // Send Object To Ordering Table
	 GsSortObject4( tmpPtr,othWorld,4,//14 - ORDERING_TABLE_LENGTH,
				(u_long *)getScratchAddr(0));
}

void InitialiseLight(GsF_LIGHT *flLight, int nLight, int nX, int nY, int nZ,
		int nRed, int nGreen, int nBlue)
{
	 // Set the direction in which light travels
	 flLight->vx = nX;flLight->vy = nY;    flLight->vz = nZ;
	 // Set the colour
	 flLight->r = nRed; flLight->g = nGreen; flLight->b = nBlue;
	 // Activate light
	 GsSetFlatLight(nLight, flLight);
}

void InitialiseAllLights()
{
	 InitialiseLight(&flLights[0], 0, -1, -1, -1, 255,255,255);
	 InitialiseLight(&flLights[1], 1, 1, 1, 1, 255,255,255);
	 GsSetAmbient(0,0,0);
	 GsSetLightMode(0);
}

void InitialiseView(GsRVIEW2 *view, int nProjDist, int nRZ,
						  int nVPX, int nVPY, int nVPZ,
						  int nVRX, int nVRY, int nVRZ)
{
	 // This is the distance between the eye
	 // and the imaginary projection screen
	 GsSetProjection(nProjDist);
	 // Set the eye position or center of projection
	 view->vpx = nVPX; view->vpy = nVPY;  view->vpz = nVPZ;
	 // Set the look at position
	 view->vrx = nVRX; view->vry = nVRY; view->vrz = nVRZ;
	 // Set which way is up
	 view->rz=-nRZ;
	 // Set the origin of the coord system in this case the car
	 view->super = WORLD; //&theCar.gsObjectCoord ;
	 // Activate view
	GsSetRefView2(view);
}

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

// This function deals with double buffering and drawing of 3D objects
void RenderWorld( )
{
// This variable keeps track of the current buffer for double buffering
int currentBuffer;
		//get the current buffer
		currentBuffer=GsGetActiveBuff();
		// set address for GPU scratchpad area
		GsSetWorkBase((PACKET*)out_packet[currentBuffer]);
		// clear the ordering table
		GsClearOt(0, 0, &othWorld[currentBuffer]);
		// draw the player
		DrawPlayer(&theCar, &othWorld[currentBuffer]);
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
		// register clear-command: clear ordering table to black
		GsSortClear(0, 0, 0,&othWorld[currentBuffer]);
		// register request to draw ordering table
		GsDrawOt(&othWorld[currentBuffer]);
}

int main()
{
	// set up print-to-screen font, the parameters are where the font is
	// loaded into the frame buffer
	FntLoad(960, 256);
	//specify where to write on the PSX screen
	FntOpen(-96, -96, 192, 192, 0, 512);
	// initialise the joypad
	PadInit();
	// initialise graphics
	InitialiseGraphics();
	// Setup view to view the car from the side
	InitialiseView(&view, 250, 0, 1000, -500, 0, 0, 0, 0 );
	InitialiseAllLights();
	// The car's initial xyz is set to 0,-200,0
	AddModelToPlayer(&theCar,  0, -200,0, (long unsigned int *)CAR_MEM_ADDR);
	while(1){
		PADstatus=PadRead();
		if (PADstatus & PADselect) break;
      // render the car and hello world message
		RenderWorld();
		}
	// clean up
	ResetGraph(3);
	return 0;
}

