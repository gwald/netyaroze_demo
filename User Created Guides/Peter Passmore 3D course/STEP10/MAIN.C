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
#define MAX_X (15)
#define MAX_Z (15)
#define MAX_WORLD_OBJECTS   (225)
#define SEPERATION (1200)
#define CAR_MEM_ADDR  (0x80090000)
#define CAR_TEX_MEM_ADDR    (0x800A0000)
#define SQUARE1_MEM_ADDR     (0x80093000)
#define SQUARE1_TEX_MEM_ADDR   (0x800B0000)

// a variable to track the vsync interval
u_long vsyncInterval=0;

unsigned long PLAYING=1;

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
	 long speed;
	 SVECTOR rotation;
	 GsDOBJ2         gsObjectHandler;
	 GsCOORDINATE2   gsObjectCoord;
}PlayerStructType2;
// Create and instance of the structure to hold our car object (tmd)
PlayerStructType2 theCar;

typedef struct
{
	 // A variable to track the total number of models in the world
	 int nTotalModels;
	 // We need one of each of the following two data structures for each
	 // object in the world thus we have arrays of them
	 GsDOBJ2         gsObjectHandler[MAX_WORLD_OBJECTS];
	 GsCOORDINATE2   gsObjectCoord[MAX_WORLD_OBJECTS];
	 // We also need an array of pointers to tmds in memory
	 unsigned long *lObjectPointer[MAX_WORLD_OBJECTS];

} WorldStructType0;
WorldStructType0     theWorld;

char worldGroundData[MAX_Z][MAX_X] ={
{'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','0','0','0','0','0','0','0','0','0','0','0','0','0','1'},
{'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1'},
};

/************* FUNCTION PROTOTYPES *******************/
void AddModelToWorld(WorldStructType0 *theWorld, int nX, int nY, int nZ,
	unsigned long *lModelAddress);
int LoadTexture(long addr);
void InitialiseWorld ();
void DrawWorld(WorldStructType0 *theWorld, GsOT *othWorld);
void AddModelToPlayer(PlayerStructType2 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress);
void InitialisePlayer(PlayerStructType2 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress);
void DrawPlayer(PlayerStructType2 *thePlayer, GsOT *othWorld);
void RenderWorld(PlayerStructType2 *thePlayer,WorldStructType0 *theWorld);
void InitialiseLight(GsF_LIGHT *flLight, int nLight, int nX, int nY, int nZ,
		int nRed, int nGreen, int nBlue);
void InitialiseTrackerView(GsRVIEW2 *view, int nProjDist, int nRZ,
						  int nVPX, int nVPY, int nVPZ,
						  int nVRX, int nVRY, int nVRZ);
void InitialiseStaticView(GsRVIEW2 *view, int nProjDist, int nRZ,
						  int nVPX, int nVPY, int nVPZ,
						  int nVRX, int nVRY, int nVRZ);
void ResetMatrix(short m[3][3]);
void RotateModel (GsCOORDINATE2 *gsObjectCoord,SVECTOR *rotateVector , int nRX,
	int nRY, int nRZ );
void AdvanceModel (GsCOORDINATE2 *gsObjectCoord, SVECTOR *rotateVector,
						long *speed, int nD);
void ProcessUserInput();


/*****************************************************/
void AddModelToWorld(WorldStructType0 *theWorld, int nX, int nY, int nZ,
		unsigned long *lModelAddress)
{
    theWorld->lObjectPointer[theWorld->nTotalModels] = (unsigned long*)lModelAddress;
	 //increment the pointer to move past the model id. (weird huh?)
	 theWorld->lObjectPointer[theWorld->nTotalModels]++;
	 // map tmd data to its actual address
	 GsMapModelingData(theWorld->lObjectPointer[theWorld->nTotalModels]);
	 // initialise the objects coordinate system - set to be that of the WORLD
	 GsInitCoordinate2(WORLD, &theWorld->gsObjectCoord[theWorld->nTotalModels]);
	 // increment pointer twice more - to point to top of model data (beats me!)
    theWorld->lObjectPointer[theWorld->nTotalModels]++;
	 theWorld->lObjectPointer[theWorld->nTotalModels]++;
	 // link the model (tmd) with the players object handler
	 GsLinkObject4((unsigned long *)theWorld->lObjectPointer[theWorld->nTotalModels],
								&theWorld->gsObjectHandler[theWorld->nTotalModels], 0);
	 // set the amount of polygon subdivision that will be done at runtime (none!)
	 theWorld->gsObjectHandler[theWorld->nTotalModels].attribute = GsDIV1;
	 // Assign the coordinates of the object model to the Object Handler
	 theWorld->gsObjectHandler[theWorld->nTotalModels].coord2 =
							&theWorld->gsObjectCoord[theWorld->nTotalModels];
    // Set The Position of the Object
    theWorld->gsObjectCoord[theWorld->nTotalModels].coord.t[0]=nX;   // X
    theWorld->gsObjectCoord[theWorld->nTotalModels].coord.t[1]=nY;   // Y
	 theWorld->gsObjectCoord[theWorld->nTotalModels].coord.t[2]=nZ;   // Z
	 // Increment the object counter
	 theWorld->nTotalModels++;
    // flag the object as needing to be drawn
	 theWorld->gsObjectCoord[theWorld->nTotalModels].flg = 0;
}

// load up from conventional memeory into video memory
int LoadTexture(long addr)
{
	RECT rect;
	GsIMAGE tim1;

	// get tim info/header, again a little bit of majic is needd the pointer
	// is incremented past the first 4 positions to get to this!
	GsGetTimInfo((u_long *)(addr+4),&tim1);

	// set the rect struct to contain the images x and y offset, width and height
	rect.x=tim1.px;
	rect.y=tim1.py;	
	rect.w=tim1.pw;	
	rect.h=tim1.ph;	

	//load image from main memory to video memory
	LoadImage(&rect,tim1.pixel);

	// if image has clut we need to load it too,
	//pmode =8 for 4 bit and 9 for 8 bit colour

	if((tim1.pmode>>3)&0x01) 
    {
	// set the rect struct to contain the clut's x and y offset, width and height
		rect.x=tim1.cx;
		rect.y=tim1.cy;	
		rect.w=tim1.cw;	
		rect.h=tim1.ch;	
	// load the clut into video memeory
		LoadImage(&rect,tim1.clut);
	}
	DrawSync(0);
	return(0);
}

void InitialiseWorld ()
{
int tmpx,tmpz;
	char c;
	//initialise total number of models to zero
	theWorld.nTotalModels=0;
	// load up the square1 texture to video memeory
	LoadTexture(SQUARE1_TEX_MEM_ADDR);
	// then for each element of the worldGroundData array if we find a 1
	// then place an instance of square1.tmd at the appropriate position
   // in the world.
	for (tmpz = 0; tmpz < MAX_Z; tmpz++ ){
		for (tmpx = 0; tmpx < MAX_X; tmpx++ ){
			c= worldGroundData[tmpz][tmpx];
			if(c == '1')  {
				AddModelToWorld(&theWorld, (tmpz * SEPERATION),(0),
								(tmpx * SEPERATION), (long *)SQUARE1_MEM_ADDR);
      	} //end if
		}  //end for tmpx
	 } //end for tmpz
}

void DrawWorld(WorldStructType0 *theWorld, GsOT *othWorld)
{
	 MATRIX  tmpls, tmplw;
	 int nCurrentModel;

	 for (nCurrentModel = 0; nCurrentModel < theWorld->nTotalModels; nCurrentModel++)
	 {
	 //Get the local world and screen coordinates, needed for light calculations
		  GsGetLws(theWorld->gsObjectHandler[nCurrentModel].coord2, &tmplw, &tmpls);
	 // Set the resulting light source matrix
		  GsSetLightMatrix(&tmplw);
	 // Set the local screen matrix for the GTE (so it works out perspective etc)
		  GsSetLsMatrix(&tmpls);
	 // Send Object To Ordering Table
		  GsSortObject4( &theWorld->gsObjectHandler[nCurrentModel], othWorld,
								2,(u_long *)getScratchAddr(0));
	 }
}

// This function associates a model with our player datastructure
// later we will want to add more than one model to the player
// to implement animation
void AddModelToPlayer(PlayerStructType2 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress)
{
	//increment the pointer to move past the model id. (weird huh?)
	lModelAddress++;
	// map tmd data to its actual address
	GsMapModelingData((unsigned long *)lModelAddress);
	// initialise the players coordinate system - set to be that of the world
	GsInitCoordinate2(WORLD, &thePlayer->gsObjectCoord);
	// increment pointer twice more - to point to top of model data (beats me!)
	lModelAddress++;  lModelAddress++;
	// link the model (tmd) with the players object handler
	GsLinkObject4((unsigned long *)lModelAddress, &thePlayer->gsObjectHandler,0);
	// Assign the coordinates of the object model to the Object Handler
	thePlayer->gsObjectHandler.coord2 = &thePlayer->gsObjectCoord;
	// setting the players gsObjectCoord.flg to 0 indicates it is to be drawn
	// Set the initial position of the object
	thePlayer->gsObjectCoord.coord.t[0]=nX;   // X
	thePlayer->gsObjectCoord.coord.t[1]=nY;   // Y
	thePlayer->gsObjectCoord.coord.t[2]=nZ;   // Z

	// setting the players gsObjectCoord.flg to 0 indicates it is to be drawn
	thePlayer->gsObjectCoord.flg = 0;
}

void InitialisePlayer(PlayerStructType2 *thePlayer, int nX, int nY, int nZ,
	unsigned long *lModelAddress)
{
	// initialise the players rotation vector to 0
	thePlayer->rotation.vx=0;thePlayer->rotation.vy=0;thePlayer->rotation.vz=0;
	//initialise speed to 0
	thePlayer->speed=0;
	// initialise other player variables and link in tmd
	AddModelToPlayer(thePlayer,  nX, nY,nZ,(unsigned long *) CAR_MEM_ADDR); //-200
}

// This function deals with setting up matrices needed for rendering
// and sends the object to the ordering table so it will be drawn
void DrawPlayer(PlayerStructType2 *thePlayer, GsOT *othWorld)
{
	 MATRIX  tmpls, tmplw;
	 //Get the local world and screen coordinates, needed for light calculations
	 GsGetLws(thePlayer->gsObjectHandler.coord2, &tmplw, &tmpls);
	 // Set the resulting light source matrix
	 GsSetLightMatrix(&tmplw);
	 // Set the local screen matrix for the GTE (so it works out perspective etc)
	 GsSetLsMatrix(&tmpls);
	 // Send Object To Ordering Table
	 GsSortObject4( &thePlayer->gsObjectHandler,othWorld,3,(u_long *)getScratchAddr(0));
}

// This function deals with double buffering and drawing of 3D objects
void RenderWorld(PlayerStructType2 *thePlayer,WorldStructType0 *theWorld)
{
// This variable keeps track of the current buffer for double buffering
int currentBuffer;
	//get the current buffer
	currentBuffer=GsGetActiveBuff();
	// Set the viewing system
	GsSetRefView2(&view);
	// Set the address of the packet area that will contain drawing commands
	GsSetWorkBase((PACKET*)out_packet[currentBuffer]);
	// clear the ordering table
	GsClearOt(0, 0, &othWorld[currentBuffer]);
	// Draw the world
	DrawWorld(theWorld,  &othWorld[currentBuffer]);
	// Draw the player
	DrawPlayer(thePlayer, &othWorld[currentBuffer]);
	// wait for end of drawing
	DrawSync(0);
	// wait for V_BLANK interrupt
	vsyncInterval=VSync(0);
	// print the vSync interval
	FntPrint("VSync Interval: %d.\n",vsyncInterval);
	// force text output
	FntFlush(-1);
	// Swap The Buffers
	GsSwapDispBuff();
	// register clear-command: clear to black
	GsSortClear(0,0,0,&othWorld[currentBuffer]);
	// register request to draw ordering table
	 GsDrawOt(&othWorld[currentBuffer]);
}

void InitialiseLight(GsF_LIGHT *flLight, int nLight, int nX, int nY, int nZ,
		int nRed, int nGreen, int nBlue)
{
	 // Set the direction in which the light travels
	 flLight->vx = nX;flLight->vy = nY;    flLight->vz = nZ;
	 // Set the colour
	 flLight->r = nRed; flLight->g = nGreen; flLight->b = nBlue;
	 // Activate light
	 GsSetFlatLight(nLight, flLight);
}

void InitialiseTrackerView(GsRVIEW2 *view, int nProjDist, int nRZ,
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
	 view->super = &theCar.gsObjectCoord ;
	 // Activate view
	GsSetRefView2(view);
}

void InitialiseStaticView(GsRVIEW2 *view, int nProjDist, int nRZ,
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
	 // Set the origin of the coord system in this case the WORLD
	 view->super = WORLD ;
	 // Activate view
	GsSetRefView2(view);
}


void InitialiseAllLights()
{
	 InitialiseLight(&flLights[0], 0, -100, -100, -100, 0xff, 0xff, 0xff);
	 InitialiseLight(&flLights[1], 1, 1000, 1000, 1000, 0xcc, 0xcc, 0xcc);
	 GsSetAmbient(0,0,0);
	 GsSetLightMode(0);
}
void InitialiseGraphics()
{
	 // Initialise The Graphics System to PAL as opposed to NTSC
	 SetVideoMode(MODE_PAL);
	 // The Actual Size of the Video memory
	 GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
    // The Top Left Coordinates Of The Two Buffers
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
void ResetMatrix(short m[3][3])
{
m[0][0]=m[1][1]=m[2][2]=ONE;
m[0][1]=m[0][2]=m[1][0]=m[1][2]=m[2][0]=m[2][1]=0;
}

// This rotation function avoids scaling problems that eventually occur from
// successive errors accumulating in the players matrix by cumulatively
// adding the angles in the players rotation vector and reseting the players
// matrix each time with the new cululative rotation angle
void RotateModel (GsCOORDINATE2 *gsObjectCoord,SVECTOR *rotateVector , int nRX, int nRY, int nRZ )
{
	MATRIX matTmp;
	// the reset the players coord system to the identity matrix
	ResetMatrix(gsObjectCoord->coord.m );
	// Add the new rotation factors into the players rotation vector
   // and then set them to the remainder of division by ONE (4096)
	rotateVector->vx = (rotateVector->vx+nRX)%ONE;
	rotateVector->vy = (rotateVector->vy+nRY)%ONE;
	rotateVector->vz = (rotateVector->vz+nRZ)%ONE;
	// RotMatrix sets up the matrix coefficients for rotation
	RotMatrix(rotateVector, &matTmp);
	// Concatenate the existing objects matrix with the rotation matrix
	MulMatrix0(&gsObjectCoord->coord, &matTmp, &gsObjectCoord->coord);
	// set the flag to redraw the object
	gsObjectCoord->flg = 0;
}

// move the model nD steps in the direction it is pointing
void AdvanceModel (GsCOORDINATE2 *gsObjectCoord, SVECTOR *rotateVector,
						long *speed, int nD)
{
	 // Moves the model nD units in the direction of its rotation vector
	MATRIX matTmp;
	SVECTOR startVector;
	SVECTOR currentDirection;
	// if nD = 0 there is no movement and we need to avois the
	// main body of the function which will cause a divide by zero error
	if(nD!=0){
		// set up original vector, pointing down the positive z axis
		startVector.vx = 0; startVector.vy = 0;	startVector.vz = ONE;
		// RotMatrix sets up the matrix coefficients for rotation
		RotMatrix(rotateVector, &matTmp);
		// multiply startVector by mattmp and put the result in currentDirection
		// which is the vector defining the direction the player is pointing
		ApplyMatrixSV(&matTmp, &startVector, &currentDirection);
		// currentDirection components have a maximum value of 4096 so we
		// scale nD to 4096 /nD then when we add the amount of
		// translation we divide by nD. This ensures that we will translate
		// the number of units originally specified by nD
		// nD = 4096 /nD ;
		gsObjectCoord->coord.t[0] +=(currentDirection.vx * nD)/4096;
		gsObjectCoord->coord.t[1] +=(currentDirection.vy * nD)/4096;
		gsObjectCoord->coord.t[2] +=(currentDirection.vz * nD)/4096;
		// Because It Has Changed, 0 Means that we will redraw it
		gsObjectCoord->flg = 0;
	} //end if
}

void ProcessUserInput()
{
	PADstatus=PadRead();
	if(PADstatus & PADselect)PLAYING=0;
	if(PADstatus & PADLleft){

		  FntPrint( "Left Arrow: Rotate Left\n");
					RotateModel (&theCar.gsObjectCoord,&theCar.rotation,0,-64,-0);
	}
	if(PADstatus & PADLright){

		  FntPrint ( "Right Arrow: Rotate Right\n");
		  RotateModel (&theCar.gsObjectCoord,&theCar.rotation,  0, 64, 0 );
	}
	if(PADstatus & PADstart){
		  FntPrint ( "PAD start\n" );
		  theCar.gsObjectCoord.coord=GsIDMATRIX;
         theCar.gsObjectCoord.coord.t[1]=-200;
		  theCar.speed=0;
		  theCar.rotation.vx=0;  theCar.rotation.vy=0;  theCar.rotation.vy=0;
        theCar.gsObjectCoord.flg=0;
	}
	if(PADstatus & PADcross){
		  FntPrint ( "PAD cross: Move Forward\n");
		  AdvanceModel(&theCar.gsObjectCoord,&theCar.rotation,&theCar.speed,64);
	}
	if (PADstatus & PADsquare){
		  FntPrint ( "PAD square Move Backward\n");
			AdvanceModel(&theCar.gsObjectCoord,&theCar.rotation,&theCar.speed,-64);
			// MoveModel(&theCar.gsObjectCoord,0,0,-32);

	 }
	if (PADstatus & PADL1){
		FntPrint ( "PAD padL1:STATIC_VIEW\n");
		InitialiseStaticView(&view, 250, 0, 0, -500,-1000, 0, 0, 0 );
	}
	if (PADstatus & PADR1)
	{
		FntPrint ( "PAD padR1: TRACKER_VIEW!\n");
		InitialiseTrackerView(&view, 250, 0, 0, -300, -1500, 0, 200, 0 );
	 }
	 if (PADstatus & PADR2)
	{
		FntPrint ( "PAD padR2: called =0!\n");
	
	 }
}


int main()
{
	// set up print-to-screen font
	FntLoad(960, 256);
	FntOpen(-96, -96, 192, 192, 0, 512);
	// set up the controller pad
	PadInit();
	InitialiseGraphics();
	InitialiseStaticView(&view, 250, 0, 0, -500,-1000, 0, 0, 0 );
	InitialiseAllLights();
	InitialisePlayer(&theCar,  0, -200,0, (long*)CAR_MEM_ADDR);
	LoadTexture( CAR_TEX_MEM_ADDR);
	InitialiseWorld();
	while(PLAYING){
		ProcessUserInput();
		RenderWorld(&theCar,&theWorld);
		}
	// clean up
	ResetGraph(0);
	return 0;
}


