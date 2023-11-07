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

#include "trig.h"

#include "vector.h"

#include "matrix.h"

#include "coord.h"







/****************************************************************************
					structures and constants
****************************************************************************/



	// Here: declare a GsIMAGE for every texture to be used in program
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
#define SHIP 0
#define CUBE 1
#define SQUARE 2
#define POLYGON 3


ObjectHandler TheShip;

#define MAX_CUBES 256
ObjectHandler TheCubes[MAX_CUBES];

#define MAX_SQUARES 128
ObjectHandler TheSquares[MAX_SQUARES];

#define MAX_POLYGONS 256
ObjectHandler ThePolygons[MAX_POLYGONS];




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




void CleanupAndExit (void);



void DealWithControllerPad (void);




void HandleAllObjects (void);

void HandleTheShip (ObjectHandler* object);
void HandleACube (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);
void HandleAPolygon (ObjectHandler* object);




void CheckCollisions (void);
int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);




void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output);

u_short SortSpriteObjectPosition (ObjectHandler* object);




void StoreScreen (void);






   

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

#define setNORMAL(normal, x, y, z)				\
				(normal)->nx = (x), (normal)->ny = (y), (normal)->nz = (z)
				
#define setVERTEX(vertex, x, y, z)				\
				(vertex)->vx = (x), (vertex)->vy = (y), (vertex)->vz = (z)	


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

		FntPrint("VP: %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);

		FntPrint("proj: %d\n", ProjectionDistance);
	
		//RegisterTextStringForDisplay ("display test", -50, 80);
		
		FntPrint("ship %d %d %d\n", TheShip.position.vx,
			TheShip.position.vy, TheShip.position.vz);				

		frameNumber++;

		if (frameNumber % 20 == 0)
			{
			// matrices: when updated in this way,
			// ship matrix VERY explicit:
			// rows show object's own axes expressed in world coordinate system
			// columns show world axes expressed in object's own coordinate system
			// given these, object-relative movements and 3D navigation are nice and easy
			// printf("ship matrix\n");
			// dumpMATRIX( &TheShip.coord.coord);
			}


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

#if 0			// very odd results. Basically, be wary of coord.c in current form;
				// needs more testing and sorting out.
	{
	GsCOORDINATE2 coord, coord2;
	VECTOR p1, p2, p3, p4;
	SVECTOR rot1;

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &coord2);

	setVECTOR( &p1, 0, 0, 1024);
	setVECTOR( &rot1, 0, ONE/32, 0);

	DeriveNewCoordSystemFromRotation (&coord,
					&rot1, &coord2);

	ConvertPointObjectToWorld (&p1, &coord2, &p2);

	ConvertPointWorldToObject (&p1, &coord2, &p3);
	ApplyMatrixLV( &coord2.coord, &p1, &p4);

	dumpVECTOR(&p1);
	dumpVECTOR(&p2);
	dumpVECTOR(&p3);
	dumpVECTOR(&p4);
	}
#endif


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

	// OLD TESTER CreateExtraTMDs();

	InitialiseObjects();

	InitialiseTheTunnel();

	CreateTheTunnel();		
}





void SortVideoMode (void)
{
	int currentMode;
	
	currentMode = GetVideoMode();

	printf("\n");
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
	printf("Then you need to change video mode\n\n");
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
	TheView.vpz = 220;
	
	TheView.rz = 0;

	TheView.super = &TheShip.coord;

	GsSetRefView2(&TheView);
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();



	InitSingleObject(&TheShip);

	BringObjectToLife (&TheShip, SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);

	setVECTOR( &TheShip.position, 0, 0, SQUARE_SIZE*6);
	//setVECTOR( &TheShip.rotate, 0, ONE/2, 0);
	//UpdateObjectCoordinates2 ( &TheShip.rotate,
	//						&TheShip.position, &TheShip.coord);
	setVECTOR( &TheShip.rotate, 0, 0, 0); 

	TheShip.movementSpeed = 20;
	TheShip.rotationSpeed = 25;

	RegisterObjectIntoObjectArray(&TheShip);



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

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		InitSingleObject(&ThePolygons[i]);

		BringObjectToLife(&ThePolygons[i], POLYGON, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&ThePolygons[i]);

			// Don't link with TMD yet, save that for later dynamic linking
		ThePolygons[i].alive = FALSE;
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
}



 




void CleanupAndExit (void)
{
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

	exit(1);

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


		// L1 and L2: step slowly
	if (pad & PADL1 && pad & PADL2)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		}


		// how fast controls act
	if (pad & PADselect)
		controlSpeed = 5;
	else
		controlSpeed = 1;

	if (pad & PADL2)
		{
		if (pad & PADRup)
			{
			ProjectionDistance += 5 * controlSpeed;
			GsSetProjection(ProjectionDistance);
			return;
			}
		if (pad & PADRdown)
			{
			ProjectionDistance -= 5 * controlSpeed;
			GsSetProjection(ProjectionDistance);
			return;
			}
		}
	
	if (pad & PADL1)
		{
		if (pad & PADRup)
			{
			TheView.vpy += 10 * controlSpeed;
			return;
			}
		if (pad & PADRdown)
			{
			TheView.vpy -= 10 * controlSpeed;
			return;
			}
		if (pad & PADR2)
			{
			TheView.vpz -= 10 * controlSpeed;
			return;
			}
		if (pad & PADR1)
			{
			TheView.vpz += 10 * controlSpeed;
			return;
			}
		if (pad & PADRleft)
			{
			TheView.vpx -= 10 * controlSpeed;
			return;
			}
		if (pad & PADRright)
			{
			TheView.vpx += 10 * controlSpeed;
			return;
			}
		}

		// movement
	if (pad & PADLup)
		TheShip.velocity.vz -= TheShip.movementSpeed * controlSpeed;
	if (pad & PADLdown)
		TheShip.velocity.vz += TheShip.movementSpeed * controlSpeed;

		// rotation
	if (pad & PADRup)
		TheShip.twist.vx += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRdown)
		TheShip.twist.vx -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR2)
		TheShip.twist.vz -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR1)
		TheShip.twist.vz += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRleft)
		TheShip.twist.vy -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRright)
		TheShip.twist.vy += TheShip.rotationSpeed * controlSpeed;
}





void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

	if (frameNumber % 30 == 0)
		{
		for (i = 0; i < MAX_CUBES; i++)
			{
			if (TheCubes[i].alive == TRUE)
				{
				printf("index: %d\n", i);
				dumpVECTOR( &TheCubes[i].position);
				}
			}
		}

		// call these two before or after the main business?
	CheckCollisions();

	UpdateTheTunnel();

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
					case SHIP:
						HandleTheShip(object);
						break;
					case CUBE:
						HandleACube(object);
						break;
					case SQUARE:
						HandleASquare(object);
						break;
					case POLYGON:
						HandleAPolygon(object);
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
}




void HandleTheShip (ObjectHandler* object)
{
	assert(object->type == SHIP);

	UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

	//KeepWithinRange(object->position.vx, -SQUARE_SIZE*8, SQUARE_SIZE*8);
	//KeepWithinRange(object->position.vy, -SQUARE_SIZE*8, SQUARE_SIZE*8);
	//KeepWithinRange(object->position.vz, -SQUARE_SIZE*8, SQUARE_SIZE*8);

		// momentum or not? 
	if (object->movementMomentumFlag == FALSE)
		{
		object->velocity.vx = 0;
		object->velocity.vy = 0;
		object->velocity.vz = 0;
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		object->twist.vx = 0;
		object->twist.vy = 0;
		object->twist.vz = 0;
		}
}






void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);

#if 0
	UpdateObjectCoordinates2 ( &object->rotate,
							&object->position, &object->coord);
#endif
}



	// NULL FUNCTION
void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);
}


	// NULL FUNCTION	
void HandleAPolygon (ObjectHandler* object)
{
	assert(object->type == POLYGON);
}


 


void CheckCollisions (void)
{
#if 0		// one way ....
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
#endif
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

	if (twist->vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(matrix, &xMatrix, matrix);
		}
	else if (twist->vy != 0)
		{
		yVector.vx = 0;
		yVector.vy = twist->vy;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(matrix, &yMatrix, matrix);
		}
	else if (twist->vz != 0)
		{
		zVector.vx = 0;
		zVector.vy = 0;
		zVector.vz = twist->vz;
			
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






void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output)
{
	SVECTOR realRotation;
	MATRIX rotationMatrix;
	SVECTOR xVector, yVector;

		// z component of rotation has no place in cylindrical tunnel
	setVECTOR( &realRotation, rotation->vx, rotation->vy, 0);

	if (realRotation.vx == 0)		 // just a theta-Y rotation
		{
		RotMatrix(&realRotation, &rotationMatrix);
		MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
		}
	else
		{
		if (realRotation.vy == 0)		 // just a theta-X rotation
			{
			RotMatrix(&realRotation, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
			}
		else	 // compound rotation: first by Y, then by X
			{
			setVECTOR( &yVector, 0, realRotation.vy, 0);	   // just y rotation
			RotMatrix(&yVector, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);

			setVECTOR( &xVector, realRotation.vx, 0, 0);	  // just x rotation
			RotMatrix(&xVector, &rotationMatrix);
			MulMatrix0(&output->coord, &rotationMatrix, &output->coord);
			}
		}

		// tell GTE that coordinate system has been updated
	output->flg = 0;
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








#if 0		// OLD: simply tests TMD creation, which is successful
	// once the TMD has been created, can be used in the usual way
	// e.g. Linked using the object.c interface ....
void CreateExtraTMDs (void)
{
	TMD_NORM nm;
	TMD_VERT v0, v1, v2, v3;

		// make a single-polygon textured TMD
	setNORMAL(&nm, 0, ONE, 0);

	setVECTOR(&v0, 0,    0, 0); 		
	setVECTOR(&v1, 1024,    0, 1024); 
	setVECTOR(&v2, 1024, 0, 1024); 		
	setVECTOR(&v3, 1024, 0, 1024);

	makeTMD_F_4T(CreatedTMDPointer, 30720, 10, 
		0, 0, 
		0, 63, 
		63, 0, 
		63, 63, 
		&nm, &v0, &v1, &v2, &v3);
}
#endif





	


		 