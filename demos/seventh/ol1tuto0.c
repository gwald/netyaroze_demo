	// taken and modified from the shell program main.c in blackpsx\modules\third
	// using 'smaller' batch file



/****************************************************************************
					includes
****************************************************************************/


#include <stdio.h>

#include <libps.h>

#include "general.h"

#include "asssert.h"

#include "dump.h"

#include "pad.h"

#include "2d1.h"

#include "rand.h"

#include "text_str.h"

#include "address.h"

#include "sound.h"

#include "object.h"







/****************************************************************************
					structures and constants
****************************************************************************/




GsIMAGE WaveTextureInfo;






int ScreenWidth, ScreenHeight;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */
  

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH];



long ProjectionDistance;
GsRVIEW2 TheView;


GsF_LIGHT TheLights[3];

GsFOGPARAM TheFogging;

int OverallLightMode;






int QuitFlag = FALSE;

int frameNumber = 0;














	// object types
#define CUBE 0
#define SQUARE 1

#define MAX_CUBES 12
ObjectHandler TheCubes[MAX_CUBES];

#define MAX_SQUARES 128
ObjectHandler TheSquares[MAX_SQUARES];



	// graphical world clipping

ObjectHandler* DisplayedObjects[MAX_OBJECTS];



	// the walls of large cube
#define SQUARE_SIZE 128

#define FLOOR_X 4
#define FLOOR_Y 4
#define FLOOR_Z 4


#define WORLD_X (FLOOR_X * SQUARE_SIZE)
#define WORLD_Y (FLOOR_Y * SQUARE_SIZE)
#define WORLD_Z (FLOOR_Z * SQUARE_SIZE)





#define MAX_TUNNEL_SECTIONS 64

int NumberTunnelSections;

VECTOR TunnelSpline[MAX_TUNNEL_SECTIONS];
VECTOR TunnelDirectionVectors[MAX_TUNNEL_SECTIONS];

#define NUMBER_SHAPES_PER_SECTION 12

#define TUNNEL_SECTION_LENGTH (SQUARE_SIZE)

#define TUNNEL_SECTION_RADIUS (SQUARE_SIZE)

typedef struct
{
	int id;

	VECTOR startPoint;

	VECTOR directionAtStart;

	GsCOORDINATE2 coordinateData;
} TunnelSection;

TunnelSection TheTunnel[MAX_TUNNEL_SECTIONS];



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);


int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);
void FindViewPoint (VECTOR* output);




void InitialiseAll (void);



void SortVideoMode (void);

void InitialiseLighting (void);
void InitialiseView (void);





void InitialiseObjects (void);



void CreateTheCube (void);



void InitialiseTheTunnel (void);

void CreateTheTunnel (void);

void UpdateTheTunnel (void);



void CleanupAndExit (void);



void DealWithControllerPad (void);




void HandleAllObjects (void);

void HandleACube (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);



void CheckCollisions (void);
int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);




void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

u_short SortSpriteObjectPosition (ObjectHandler* object);




void StoreScreen (void);


void ScaleVectorToUnit (VECTOR* vector);

int VectorIsUnitSize (VECTOR* vector);





/****************************************************************************
					 macros
****************************************************************************/


#define min(a,b) ( ((a) > (b)) ? (b) : (a))

#define max(a,b) ( ((a) > (b)) ? (a) : (b))

#define KeepWithinRange(quantity, min, max)					\
{															\
	if ((quantity) < (min))									\
		(quantity) = (min);									\
	else if ((quantity) > (max))							\
		(quantity) = (max);									\
}



#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	

	


#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)			\
	{														\
	u_long onlyNthBitOn;									\
															\
	onlyNthBitOn = 1 << (N);								\
															\
	argument ^= onlyNthBitOn;								\
	}


		
  
/****************************************************************************
					functions
****************************************************************************/


void main (void)
{
	int	hsync = 0;
	int bufferIndex;			  // buffer index
	int i;
	u_short zValue;
	ObjectHandler* object;
	MATRIX tmpls;
	int numberToDisplay;
	
	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{		
		FntPrint("frame: %d\n", frameNumber);
	
		//RegisterTextStringForDisplay ("display test", 0, 0);
		
		FntPrint("VP: x y z\n%d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);
		
		FntPrint("VR: x y z\n%d %d %d\n", TheView.vrx, TheView.vry, TheView.vrz);				

		frameNumber++;

		DealWithControllerPad();

		HandleSound();

		HandleAllObjects();

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		numberToDisplay 
					= PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);

		DisplayTextStrings (&Wot[bufferIndex]);

		for (i = 0; i < numberToDisplay; i++)
			{
			object = DisplayedObjects[i];

			if (object->displayFlag == TMD)
				{
				GsGetLs(&(object->coord), &tmpls);
						   
				GsSetLightMatrix(&tmpls);
							
				GsSetLsMatrix(&tmpls);
							
				GsSortObject4( &(object->handler), 
						&Wot[bufferIndex], 
							3, getScratchAddr(0));
				}
			else
				{
				zValue = SortSpriteObjectPosition(object);
				GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
				}
			}

		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 32, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		FntPrint("hsync: %d\n", hsync);
		FntFlush(-1);
		}

	CleanupAndExit();
}






	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

	// CURRENTLY DISABLED (clipping with no criterion -> all get included)

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	VECTOR viewPoint;
	int SimpleClipDistance;

	FindViewPoint( &viewPoint);

	SimpleClipDistance = SQUARE_SIZE * 8;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				#if 0
				if (abs(firstArray[i]->coord.coord.t[0] - viewPoint.vx) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[1] - viewPoint.vy) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[2] - viewPoint.vz) < SimpleClipDistance)
					{
					secondArray[numberDisplayedObjects] = firstArray[i];
					numberDisplayedObjects++;
					}
				#endif
					// no clipping: direct copy
				secondArray[numberDisplayedObjects] = firstArray[i];
				numberDisplayedObjects++;
				}
		}
		
	return numberDisplayedObjects;	
}		






void FindViewPoint (VECTOR* output)
{
	setVECTOR( output, TheView.vpx, TheView.vpy, TheView.vpz);
}





void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	ScreenWidth = 320;
	ScreenHeight = 240;

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,ScreenHeight);

	GsInit3D();		  
	
	SortVideoMode(); 

		// sort our only ordering table
	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0, &Wot[0]);
	GsClearOt(0, 0, &Wot[1]);

	InitialiseLighting();

	InitialiseView();

		/// texture for square model
	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
		 
		// sort basic text printing
	FntLoad(960, 256);
	FntOpen(0, 0, 256, 200, 0, 512);

	InitialiseObjects();

	//CreateTheCube();		 // NO MORE

	InitialiseTheTunnel();

	CreateTheTunnel();
}





void SortVideoMode (void)
{
	int currentMode;

	currentMode = GetVideoMode();

	printf("\n\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}

	printf("If everything is black and white\n");
	printf("or there are lines at bottom or top of screen\n");
	printf("Then you need to change video mode\n\n\n");
}





void InitialiseLighting (void)
{
		// three flat light sources
	TheLights[0].vx = ONE; TheLights[0].vy = 0; TheLights[0].vz = 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;
	GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vx = 0; TheLights[1].vy = ONE; TheLights[1].vz = 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[1].b = 0;
	GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vx = 0; TheLights[2].vy = 0; TheLights[2].vz = ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;
	GsSetFlatLight(2, &TheLights[2]);

		
		// background lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);

  
		// distance colour blending
  	TheFogging.dqa = -960;
	TheFogging.dqb = 5120*5120;
	TheFogging.rfc = 0; 
	TheFogging.gfc = 0; 
	TheFogging.bfc = 4;
	GsSetFogParam( &TheFogging);


		// overall lighting conditions
	OverallLightMode = 0;			// NO fogging yet 
	GsSetLightMode(OverallLightMode);
}






void InitialiseView (void)
{
	ProjectionDistance = 192;			

	GsSetProjection(ProjectionDistance);  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -500;
	
	TheView.rz = 0;

	TheView.super = WORLD;

	GsSetRefView2(&TheView);
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();

	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}

	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}


	 
	LinkAllObjectsToModelsOrSprites();
	
	LinkAllObjectsToTheirCoordinateSystems();



	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;		// start off dead
		}

	TheCubes[0].alive = TRUE;
	setVECTOR( &TheCubes[0].position, (FLOOR_X * SQUARE_SIZE/2),
					(FLOOR_Y * SQUARE_SIZE/2), (FLOOR_Z * SQUARE_SIZE/2) );
}






	// make six large walls, each from square tiles

void CreateTheCube (void)
{
	int x, y, z;
	int xPos, yPos, zPos;
	int squareID = 0;
	ObjectHandler* square;

	yPos = FLOOR_Y * SQUARE_SIZE;
	for (x = 0; x < FLOOR_X; x++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, ONE/2, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0); 

			squareID++;
			}
		}

	for (x = 0; x < FLOOR_X; x++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, 0, zPos);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			squareID++;
			}
		}




	xPos = FLOOR_X * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, ONE/4);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, 0, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, 3*ONE/4);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}



	zPos = FLOOR_Z * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 3*ONE/4, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, 0);

			setVECTOR( &square->rotate, ONE/4, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}
}





void InitialiseTheTunnel (void)
{
	int i;
	VECTOR *previousPoint, *thisPoint, *nextPoint;
	VECTOR previousToThis, thisToNext, previousToNext;

		// very first
	NumberTunnelSections = 64;
	assert(NumberTunnelSections <= MAX_TUNNEL_SECTIONS);

		// initialise all the data
	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		setVECTOR( &TunnelSpline[i], 0, 0, 0);
		setVECTOR( &TunnelDirectionVectors[i], 0, 0, 0);

		TheTunnel[i].id = i;
		setVECTOR( &TheTunnel[i].startPoint, 0, 0, 0);
		setVECTOR( &TheTunnel[i].directionAtStart, 0, 0, 0);
		GsInitCoordinate2( WORLD, &TheTunnel[i].coordinateData);
		}



		// HERE: create tunnel data itself

		// firstly: the spline: INITIALLY a simple straight going into screen
	for (i = 0; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TunnelSpline[i], 0, 0, (i * SQUARE_SIZE) );
		}

		// now calculate the directions at each tunnel section start
	for (i = 0; i < NumberTunnelSections; i++)
		{
		if (i == 0)
			{			// first one: Always straight ahead
			setVECTOR( &TunnelDirectionVectors[i], 0, 0, ONE);
			}
		else if (i == NumberTunnelSections-1)
			{
						// last one: Always same as one before
			setVECTOR( &TunnelDirectionVectors[i], 
							TunnelDirectionVectors[i-1].vx,
							TunnelDirectionVectors[i-1].vy,
							TunnelDirectionVectors[i-1].vz);
			}
		else		   // use vectors to sections behind and ahead, average the two
			{
			assert( (i-1 >= 0) && (i+1 < NumberTunnelSections) );

			previousPoint = &TunnelSpline[i-1];
			thisPoint = &TunnelSpline[i];
			nextPoint = &TunnelSpline[i+1];

			setVECTOR( &previousToThis, (thisPoint->vx - previousPoint->vx),
										(thisPoint->vy - previousPoint->vy),
										(thisPoint->vz - previousPoint->vz) );

			setVECTOR( &thisToNext, (nextPoint->vx - thisPoint->vx),
										(nextPoint->vy - thisPoint->vy),
										(nextPoint->vz - thisPoint->vz) );

			setVECTOR( &previousToNext, (previousToThis.vx + thisToNext.vx),
										(previousToThis.vy + thisToNext.vy),
										(previousToThis.vz + thisToNext.vz) );

				// we only want the direction itself
			ScaleVectorToUnit(&previousToNext);
			assert( VectorIsUnitSize(&previousToNext) );

			setVECTOR( &TunnelDirectionVectors[i], previousToNext.vx, 
					previousToNext.vy, previousToNext.vz);
			}
		}


		// now store this data in TheTunnel (redundant I know ...)
	for (i = 0; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TheTunnel[i].startPoint, TunnelSpline[i].vx,
				TunnelSpline[i].vy, TunnelSpline[i].vz);

		setVECTOR( &TheTunnel[i].directionAtStart, TunnelDirectionVectors[i].vx,
				TunnelDirectionVectors[i].vy, TunnelDirectionVectors[i].vz);
		}

	// in InitialiseAll: call CreateTheTunnel ....
	// some initial creation, set up the dynamic creation
}






void CreateTheTunnel (void)
{
	int i;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		}
}




void UpdateTheTunnel (void)
{
	int i;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		}
}



void CleanupAndExit (void)
{
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}





void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;

	pad = PadRead();

	if (pad & PADselect && pad & PADstart)
		QuitFlag = TRUE;

	if (pad & PADstart)
		{
		for (;;)
			{
			pad = PadRead();
			if ( (pad & PADstart) == 0)
				break;
			}
		}

		// how fast controls act
	if (pad & PADselect)
		controlSpeed = 5;
	else
		controlSpeed = 1;


			// right pad: moving viewing point
		if (pad & PADRleft)
                {
                TheView.vpx -= 20 * controlSpeed;
                return;
                }
        if (pad & PADRright)
                {
                TheView.vpx += 20 * controlSpeed;
                return;
                }
        if (pad & PADR1)
                {
                TheView.vpz += 20 * controlSpeed;
                return;
                }
        if (pad & PADR2)
                {
                TheView.vpz -= 20 * controlSpeed;
                return;
                }
		if (pad & PADRup)
                {
                TheView.vpy += 20 * controlSpeed;
                return;
                }
        if (pad & PADRdown)
                {
                TheView.vpy -= 20 * controlSpeed;
                return;
                }


			// left pad: move reference point
		if (pad & PADLleft)
                {
                TheView.vrx -= 20 * controlSpeed;
                return;
                }
        if (pad & PADLright)
                {
                TheView.vrx += 20 * controlSpeed;
                return;
                }
        if (pad & PADL1)
                {
                TheView.vrz += 20 * controlSpeed;
                return;
                }
        if (pad & PADL2)
                {
                TheView.vrz -= 20 * controlSpeed;
                return;
                }
		if (pad & PADLup)
                {
                TheView.vry += 20 * controlSpeed;
                return;
                }
        if (pad & PADLdown)
                {
                TheView.vry -= 20 * controlSpeed;
                return;
                }

}





void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

	CheckCollisions();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];

					// updating common to all object types
				object->lifeTime++;
				if (object->canFire == TRUE)
					object->framesSinceLastFire++;
				// HandleCommonUpdating(object);

				switch(object->type)
					{
					case CUBE:
						HandleACube(object);
						break;
					case SQUARE:
						HandleASquare(object);
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
}





void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);

	UpdateObjectCoordinates2 ( &object->rotate,
							&object->position, &object->coord);
}




void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);

#if 0
	UpdateObjectCoordinates2 ( &object->rotate,
							&object->position, &object->coord);
#endif
}




void CheckCollisions (void)
{
	int i, j;

	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == TRUE)
			{
			for (j = 0; j < MAX_SQUARES; j++)
				{
				if (TheSquares[j].alive == TRUE)
					{
					if (ObjectsVeryClose(&TheCubes[i], &TheSquares[j]))
						{
						//printf("cube %d and square %d collision\n", i, j);
						TheCubes[i].currentHealth--;
						TheSquares[j].currentHealth--;
						}
					}
				}
			}
		}
}






	// extremely simple and crude collision detection

#define APPROXIMATE_RADIUS 32

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->coord.coord.t[0] - second->coord.coord.t[0]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[1] - second->coord.coord.t[1]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[2] - second->coord.coord.t[2]) > APPROXIMATE_RADIUS)
		return FALSE;

	return TRUE; 
}





	// this function does object-relative movement and rotation
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
	VECTOR realMovement;
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;
	
		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, velocity, &realMovement);

		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;

	if (xVector.vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(matrix, &xMatrix, matrix);
		}
	else if (yVector.vy != 0)
		{
		yVector.vx = twist->vy;
		yVector.vy = 0;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(matrix, &yMatrix, matrix);
		}
	else if (yVector.vz != 0)
		{
		zVector.vx = twist->vz;
		zVector.vy = 0;
		zVector.vz = 0;
			
		RotMatrix(&zVector, &zMatrix);

		MulMatrix0(matrix, &zMatrix, matrix);
		}
	 
	coordSystem->coord = *matrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}








	// this does world-relative movement and rotation
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;

		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// set position by absolute coordinates	
	coordSystem->coord.t[0] = translationVector->vx;
	coordSystem->coord.t[1] = translationVector->vy;
	coordSystem->coord.t[2] = translationVector->vz;

	   	// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}





	// sort out the conversion of coordinates from local->world->screen			 
	// for objects that are displayed by GsSPRITE rather than GsDOBJ2
u_short SortSpriteObjectPosition (ObjectHandler* object)
{
	VECTOR screen;
	int visualX, visualY;
	u_short zValue;

	PushMatrix();

	ApplyMatrixLV( &GsWSMATRIX, &object->position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

	if (screen.vz == 0)			   // prevent division by zero
		return 0;	   // right at the front

	visualX = object->sprite.w * ProjectionDistance / screen.vz;	
	visualY = object->sprite.h * ProjectionDistance / screen.vz;	

	object->sprite.x = ((screen.vx * ProjectionDistance / screen.vz) - visualX/2);
	object->sprite.y = ((screen.vy * ProjectionDistance / screen.vz) - visualY/2);

	object->sprite.scalex = object->scaleX * ProjectionDistance / screen.vz; 
	object->sprite.scaley = object->scaleY * ProjectionDistance / screen.vz; 

	PopMatrix();

		// 3D clipping
	if (screen.vz < ProjectionDistance/2)
		object->sprite.attribute |= GsDOFF;			 // display off
	else
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31)		// ensure 31st bit is OFF

	zValue = (u_short) (screen.vz >> 6);	 // NOTE: need to adjust this
											// just a blagg to make it look right

	return zValue;			   // NOT YET CORRECT
}






#define STORING_SCREEN 0

void StoreScreen (void)
{
#if STORING_SCREEN
	u_long* destination;
	int x, y, w, h;
	RECT rect;

	destination = (u_long *) SCREEN_SAVE_ADDRESS;

	x = y = 0;		// top left of frame buffer

	w = ScreenWidth;
	h = ScreenHeight;

	*(destination+0) = 0x00000010;		/* ID */
	*(destination+1) = 0x00000002;		/* FLAG(15bit Direct,No Clut) */
	*(destination+2) = (w*h/2+3)*4;		/* pixel bnum */
	*(destination+3) = ((0 & 0xffff) << 16) | (640 & 0xffff);
						/* pixel DX,DY: at 640, 0 */
	*(destination+4) = ((h & 0xffff) << 16) | (w & 0xffff);
						/* pixel W,H */

	// NO CLUT since 16-bit mode used

	rect.x = x; 
	rect.y = y;
	rect.w = w; 
	rect.h = h;
	DrawSync(0);
	StoreImage(&rect, destination+5);		

	printf("\n\nPress [F10][F4] for dsave, to get screen picture\n"); 
	printf("Dsave[0]: filename %08x %x\n\n\n", destination, (w*h/2+5)*4);

	DrawSync(0);
	VSync(0);
#endif
}






void ScaleVectorToUnit (VECTOR* vector)
{
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) + pow(vector->vz, 2); 

	length = pow(square, 0.5);
	assert(length != 0);	   // never allow division by zero

	vector->vx *= ONE;
	vector->vy *= ONE;
	vector->vz *= ONE;

	vector->vx /= length;
	vector->vy /= length;
	vector->vz /= length;
}





#define TOLERANCE_LIMIT 100

int VectorIsUnitSize (VECTOR* vector)
{
	int booleanResult = FALSE;
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) 
					+ pow(vector->vz, 2); 

	length = pow(square, 0.5);
	
	if (abs(length - ONE) < TOLERANCE_LIMIT)
		booleanResult = TRUE;

	return booleanResult;
}
	