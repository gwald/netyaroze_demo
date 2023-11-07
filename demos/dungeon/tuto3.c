/************************************************************
 *															*
 *						Tuto3.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     13/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/








#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "sincos.h"
#include "atan.h"
#include "pad.h"
#include "tmd.h"
#include "asssert.h"
#include "object.h"
#include "dump.h"




#define PIH 320
#define	PIV	240



#define PACKETMAX	2048		/* Max GPU packets */
#define OBJMAX		400		/* Max Objects */	  // USED TO BE 256
#define PACKETMAX2	(PACKETMAX*24)
#define TEX1_ADDR	0x800a0000	/* Top Address of texture data (TIM FORMAT)*/
#define MODEL1_ADDR	0x80090000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL2_ADDR	0x80091000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL3_ADDR	0x80092000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL4_ADDR	0x80093000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL5_ADDR	0x80094000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL6_ADDR	0x80095000	/* Top Address of modeling data (TMD FORMAT)*/


#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000


#define OT_LENGTH	9		


static GsOT	Sot[2];			/* Handler of OT */
static GsOT_TAG	stags[2][16];		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 
static GsDOBJ2	Models[OBJMAX]; 	/* Array of Object Handler : */
static GsCOORDINATE2 DWorld[OBJMAX];



	// This vector: vy expresses angle theta-y, i.e. angle on plane
static SVECTOR	PVect;			
static GsRVIEW2 view;			


static GsF_LIGHT TheLights[3];		


int ReferenceDistance, ViewpointZDistance, ViewPointYDistance;

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

static int nModels;

static long* dop1;
static long* dop2;
static long* dop3;
static long* dop4;
static long* dop5;
static long* dop6;



static GsFOGPARAM fogparam;
static GsLINE line;
static GsBOXF box[2];
static short lightmode;



#define	FACT		128
#define WLDX_MAX	60
#define WLDY_MAX	60
#define	WEST		112
#define	POSY		28
#define	WALL		0x01



static char worldmaps[WLDX_MAX][WLDY_MAX];
static GsCOORDINATE2 World[WLDX_MAX][WLDY_MAX];




#define MOVE_VIEW 0
#define MOVE_SHIP 1
#define ALTER_VIEW 2
int MainMode;



#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif


int frameNumber = 0;



// object types
#define PLAYER 0
#define CUBE 1
#define ENEMY 2

ObjectHandler PlayersShip;

#define MAX_CUBES 50
ObjectHandler TheCubes[MAX_CUBES];


#define MAX_SHIPS 20
ObjectHandler TheShips[MAX_SHIPS];



int MovementSpeed;
int RotationSpeed;

int PlayerBulletSpeedFactor;
int EnemyBulletSpeedFactor;			  


int PlayerFiringRate;
int FramesSinceLastFire;




	// simple enemy strategies
#define WAITING 0
#define STATIONARY_TRACK 1
#define SEEKING 2
#define RANDOM 3

#define NUMBER_ENEMY_STRATEGIES 4



	// more business for strategies


#define SWIVEL_LEFT 0
#define SWIVEL_RIGHT 1
#define MOVE_FORWARDS 2
#define MOVE_BACKWARDS 3
#define FIRING 4

#define NUMBER_RANDOM_ACTIONS 5


	







/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void draw_world_maps (GsOT* ot);


void DealWithControllerPad(void);
void PlayerFiresAShot (void);
void EnemyShipFiresAShot (ObjectHandler* enemy);

void move_lighting_spot(void);
void NewMoveLightingSpot (void);


void InitialiseAll(void);
void InitialiseObjects (void);
void InitialiseShipAccordingToStrategy (ObjectHandler* object);

void HandleAllObjects (void);

void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);


void InitialiseView(void);
void InitialiseLighting(void);


int texture_init(long addr);
long* model_init(long* adrs);
void make_world_maps(void);

int calc_world_data(void);
int NewCalculateWorldData (void);
int MinOfFour (int a, int b, int c, int d);
int MaxOfFour (int a, int b, int c, int d);


int calc_model_data(int n, int px, int py, int fact, int atrb);
void draw_world_maps( GsOT* ot );
int collision( int x, int y );
void clear_view_point(void);

void PrintObject (ObjectHandler* object);

void PositionSomewhereNotInAWall (ObjectHandler* object);


void HandleEnemyBehaviour (ObjectHandler* object);

void HandlePlayersDeath (void);
void HandleEnemysDeath (ObjectHandler* object);

int CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);





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





/****************************************************************************
					 functions
****************************************************************************/



void main( void )
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;
	MATRIX	tmpls, tmplw;
	ObjectHandler** pointer;
	ObjectHandler* object;



	InitialiseAll();

   

	side = GsGetActiveBuff();

	while(1)
		{
		//FntPrint("X = %d\nZ = %d\nV = %d\nN = %d\nH = %d\n",view.vpx,view.vpz,PVect.vy,nModels,hsync );
		//FntPrint("X = %d\nZ = %d\nV = %d\n", view.vpx,view.vpz,PVect.vy);
		//FntPrint("Num models: %d\nHsync = %d\n",nModels,hsync );
		//FntPrint("frame: %d\n", frameNumber);
		//FntPrint("r: %d, p: %d\n", ReferenceDistance, ViewpointZDistance);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);		
		FntPrint("mode: ");
		printf("position 8\n");
		switch(MainMode)
			{
			case MOVE_VIEW:
				FntPrint("move view\n");
				break;
			case MOVE_SHIP:
				FntPrint("move ship\n");
				break;
			case ALTER_VIEW:
				FntPrint("alter view\n");
				break;
			default:
				assert(FALSE);
			}
		FntPrint("ship pos: \n%d %d %d\n", 
			PlayersShip.position.vx, 
			PlayersShip.position.vy,
			PlayersShip.position.vz); 
		frameNumber++;
		printf("position 9\n");
		DealWithControllerPad();
		printf("position 10\n");

		if (view.super == WORLD)
			move_lighting_spot();
		else
			NewMoveLightingSpot();
		printf("position 11\n");


		if (view.super == WORLD)
			nModels = calc_world_data();
		else if (view.super == &PlayersShip.coord)
			nModels = NewCalculateWorldData();
		printf("position 12\n");


		GsSetRefView2(&view);
		GsSetWorkBase((PACKET*)packetArea[side]);
		printf("position 13\n");
		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);
		op = Models;
		printf("position 14\n");
		for( i=0; i<nModels; i++ ) {
			GsGetLws(op->coord2,&tmplw, &tmpls);	
			GsSetLightMatrix(&tmplw);	
			GsSetLsMatrix(&tmpls);	   
			GsSortObject4(op,&Wot[side],3,getScratchAddr(0));
			op++;
		}
		printf("position 15\n");


		HandleAllObjects();
		printf("position 16\n");

		pointer = &(ObjectArray[0]);
		for (i = 0; i < MAX_OBJECTS; i++) 
			{
			if (*pointer != NULL)
				{
				if ( (*pointer)->alive == TRUE)
					{
					object = *pointer;

					GsGetLs(&(object->coord), &tmpls);
			   
					GsSetLightMatrix(&tmpls);
				
					GsSetLsMatrix(&tmpls);
				
					GsSortObject4( &(object->handler), 
						&Wot[side], 
							3, getScratchAddr(0));
					}	 
				}
			pointer++;
			}
		printf("position 17\n");

		draw_world_maps( &Sot[side] );
		printf("position 18\n");
		hsync = VSync(2);
		ResetGraph(1);
		GsSwapDispBuff();
		printf("position 19\n");
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		GsDrawOt(&Sot[side]);
		side ^= 1;
		FntFlush(-1);
		printf("position 20\n");
		}

	ResetGraph(3);
}






	// draw line to indicate angle of view on plane
void draw_world_maps( GsOT* ot )
{

	line.attribute = 0;
	line.r = line.g = line.b = 128;
	line.x0 = PIH/2-48; line.y0 = -PIV/2+48;
	line.x1 = line.x0 + (rsin( PVect.vy )/256);
	line.y1 = line.y0 - (rcos( PVect.vy )/256);

	setRECT( &box[0], line.x0-16, line.y0-16, 32, 32 );
	box[0].x += ( rsin( PVect.vy )/256 );
	box[0].y -= ( rcos( PVect.vy )/256 );
	box[0].r = 0; box[0].g = 0; box[0].b = 128;

	box[1].attribute = 1<<30;	  // semi-transparency
	setRECT( &box[1], line.x0-32, line.y0-32, 64, 64 );
	box[1].r = 128; box[1].g = 128; box[1].b = 128;

	GsSortLine( &line, ot, 2 );
	GsSortBoxFill( &box[0], ot, 2 );
	GsSortBoxFill( &box[1], ot, 2 );
}





#define	SPEED	2


void DealWithControllerPad (void)
{
	int	spd;
	long	pad;
	long	range;
	short	vx, vz;	   // world-relative x, z displacements for this frame
	int speedMultiplier;
	static int framesSinceLastModeToggle = 0;
  
	pad = PadRead(0);
	
		// actions depend on mode
	switch(MainMode)
		{
		case MOVE_VIEW:
			{
			spd = SPEED;
			if( pad & PADRright ) spd = spd*4;

				// Lleft and Lright: change angle on horizontal plane
			if(pad & PADLleft) {
				PVect.vy = (PVect.vy-spd*8)&4095;
			}

			if(pad & PADLright) {
				PVect.vy = (PVect.vy+spd*8)&4095;
			}

				// R2 and L2 (m and o): alter vx
			if((pad & PADm)&&(PVect.vx+spd*8<960)) {
				PVect.vx = (PVect.vx+spd*8);
			}
			if((pad & PADo)&&(PVect.vx-spd*8>-960)) {
				PVect.vx = (PVect.vx-spd*8);
			}

			vz = vx = 0;
			if(pad & PADLup) {
				vz += (rcos( PVect.vy )/64*spd)/16;
				vx += (rsin( PVect.vy )/64*spd)/16;
			}
			if(pad & PADLdown) {
				vz -= (rcos( PVect.vy )/64*spd)/16;
				vx -= (rsin( PVect.vy )/64*spd)/16;
			}
				// R1 and L1: sidestepping: inline conversion to world coords
			if(pad & PADl) {
				vz += (rcos((PVect.vy+1024)&4095)/64*spd)/16;
				vx += (rsin((PVect.vy+1024)&4095)/64*spd)/16;
			}
			if(pad & PADn) {
				vz += (rcos((PVect.vy-1024)&4095)/64*spd)/16;
				vx += (rsin((PVect.vy-1024)&4095)/64*spd)/16;
			}

				// prevent moving into walls
			if( vz>0 ) {
				if(collision(    0, vz+WEST)) vz = 0;
				if(collision(-WEST, vz+WEST)) vz = 0;
				if(collision( WEST, vz+WEST)) vz = 0;
			}
			if( vz<0 ) {
				if(collision(    0, vz-WEST)) vz = 0;
				if(collision(-WEST, vz-WEST)) vz = 0;
				if(collision( WEST, vz-WEST)) vz = 0;
			}
			if( vx>0 ) {
				if(collision( vx+WEST,     0)) vx = 0;
				if(collision( vx+WEST,  WEST)) vx = 0;
				if(collision( vx+WEST, -WEST)) vx = 0;
			}
			if( vx<0 ) {
				if(collision( vx-WEST,     0)) vx = 0;
				if(collision( vx-WEST,  WEST)) vx = 0;
				if(collision( vx-WEST, -WEST)) vx = 0;
			}

			if( vz || vx ) {
				view.vpz += vz;
				view.vpx += vx;
			}

			range = rcos( PVect.vx );
			view.vrz = (view.vpz+rcos( PVect.vy )*range/ONE);
			view.vrx = (view.vpx+rsin( PVect.vy )*range/ONE);
			view.vry = rsin( PVect.vx ) + POSY;
			}
			break;
		case MOVE_SHIP:
			speedMultiplier = 4;

				// movement
			if (pad & PADLup)
				PlayersShip.velocity.vz -= MovementSpeed * speedMultiplier;
			if (pad & PADLdown)
				PlayersShip.velocity.vz += MovementSpeed * speedMultiplier;


				// firing
			if (pad & PADL2)
				{
				if (FramesSinceLastFire > PlayerFiringRate)
					{
					PlayerFiresAShot();
					FramesSinceLastFire = 0;
					}
				}
			FramesSinceLastFire++;


				// rotation
			if (pad & PADRright)
				PlayersShip.twist.vy += RotationSpeed * speedMultiplier;
			if (pad & PADRleft)
				PlayersShip.twist.vy -= RotationSpeed * speedMultiplier;
					
				// flip viewpoint
			if (pad & PADR2)
				{
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				view.rz = 0;
				view.super = &PlayersShip.coord;
				GsSetRefView2(&view);
				}
			if (pad & PADR1)
				{
				view.vpx = WLDX_MAX/2*FACT; view.vpy = POSY; view.vpz = WLDY_MAX/2*FACT;

			  	/* Setting focus point location : �����_�p�����[�^�ݒ� */
				view.vrx = 0; view.vry = POSY; view.vrz = 0;
				/* Setting bank of SCREEN : ���_�̔P��p�����[�^�ݒ� */
				view.rz=0;
				view.super = WORLD;
				GsSetRefView2(&view);
				}
			break;
		case ALTER_VIEW:
			if (pad & PADRup)
				{
				ReferenceDistance += 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRdown)
				{
				ReferenceDistance -= 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADLup)
				{
				ViewPointYDistance += 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADLdown)
				{
				ViewPointYDistance -= 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRleft)
				{
				ViewpointZDistance -= 20;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRright)
				{
				ViewpointZDistance += 20;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			break;
		default:
			assert(FALSE);
		}

		// select: toggles main mode
	if (pad & PADselect)
		{
		if (framesSinceLastModeToggle > 10)
			{
			if (MainMode == ALTER_VIEW)	
				MainMode = MOVE_VIEW;
			else
				MainMode++;
			switch(MainMode)
				{
				case MOVE_VIEW:
					view.super = WORLD;
					break;
				case MOVE_SHIP:
					view.super = &PlayersShip.coord;
					break;
				}
			framesSinceLastModeToggle = 0;
			}
		}
	framesSinceLastModeToggle++;


		// select and R2: step slowly
	if (pad & PADselect && pad & PADR2)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		}

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		DrawSync(0);
		exit(1);
		}


		// pause
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead(0);
			}
		}
}





void PlayerFiresAShot (void)
{
	int cubeID = -1;
	int i;	

		// find next available cube
	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == FALSE)
			{
			cubeID = i;
			break;
			}
		}

	if (cubeID == -1)		// no free shots
		return;
	else
		{
		TheCubes[cubeID].alive = TRUE;
		TheCubes[cubeID].lifeTime = 0;
		TheCubes[cubeID].allegiance = PLAYER;

			// start spinning
		TheCubes[cubeID].rotationMomentumFlag = TRUE;
		TheCubes[cubeID].twist.vx = rand() % 20;
		TheCubes[cubeID].twist.vy = rand() % 20;
			// send in direction of ship at time of fire
			// NOTE: this goes badly askew when ship not parallel to xz plane
		TheCubes[cubeID].movementMomentumFlag = TRUE;
		TheCubes[cubeID].velocity.vx = PlayersShip.coord.coord.m[2][0] >> PlayerBulletSpeedFactor;
		TheCubes[cubeID].velocity.vy = PlayersShip.coord.coord.m[2][1] >> PlayerBulletSpeedFactor;
		TheCubes[cubeID].velocity.vz = -PlayersShip.coord.coord.m[2][2] >> PlayerBulletSpeedFactor; 
		
					// emitted from nose of ship
		TheCubes[cubeID].position.vx = PlayersShip.position.vx;
		TheCubes[cubeID].position.vy = PlayersShip.position.vy;
		TheCubes[cubeID].position.vz = PlayersShip.position.vz;
		}
}






void EnemyShipFiresAShot (ObjectHandler* enemy)
{
	int cubeID = -1;
	int i;	

		// find next available cube
	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == FALSE)
			{
			cubeID = i;
			break;
			}
		}

	if (cubeID == -1)		// no free shots
		return;
	else
		{
		TheCubes[cubeID].alive = TRUE;
		TheCubes[cubeID].lifeTime = 0;
		TheCubes[cubeID].allegiance = ENEMY;

			// start spinning
		TheCubes[cubeID].rotationMomentumFlag = TRUE;
		TheCubes[cubeID].twist.vx = rand() % 20;
		TheCubes[cubeID].twist.vy = rand() % 20;
			// send in direction of ship at time of fire
			// NOTE: this method goes badly askew when ship not parallel to xz plane
		TheCubes[cubeID].movementMomentumFlag = TRUE;
		TheCubes[cubeID].velocity.vx = enemy->coord.coord.m[2][0] >> EnemyBulletSpeedFactor;
		TheCubes[cubeID].velocity.vy = enemy->coord.coord.m[2][1] >> EnemyBulletSpeedFactor;
		TheCubes[cubeID].velocity.vz = -enemy->coord.coord.m[2][2] >> EnemyBulletSpeedFactor; 
		
					// emitted from nose of ship
		TheCubes[cubeID].position.vx = enemy->position.vx;
		TheCubes[cubeID].position.vy = enemy->position.vy;
		TheCubes[cubeID].position.vz = enemy->position.vz;
		}
}





/* �Փ˔��� */
int collision (int vx, int vz )
{
	int	wx, wz;

	wx = ((view.vpx+FACT/2)+vx);
	wz = ((view.vpz+FACT/2)+vz);
	if( wx<0 || wz<0 ) return(1);

	wx = wx/FACT; wz = wz/FACT;
	if( wx>=WLDX_MAX || wz<0 || wz>=WLDY_MAX ) return(1);

	return( worldmaps[wx][wz] );
}




/* ���_�ړ��ɂ������x�N�g���␳ */
void move_lighting_spot( void )
{
	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x70;
	TheLights[0].vx = view.vrx - view.vpx;
	TheLights[0].vy = 4096;			 // from above
	TheLights[0].vz = view.vrz - view.vpz;
	GsSetFlatLight(0,&TheLights[0]);

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x70;
	TheLights[1].vx = view.vrx - view.vpx;
	TheLights[1].vy = -4096;			 // from below
	TheLights[1].vz = view.vrz - view.vpz;
	GsSetFlatLight(1,&TheLights[1]);
}


   
void NewMoveLightingSpot (void)
{
	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x70;
	TheLights[0].vx = PlayersShip.coord.coord.t[0];
	TheLights[0].vy = 4096;			 // from above
	TheLights[0].vz = PlayersShip.coord.coord.t[2];
	GsSetFlatLight(0,&TheLights[0]);

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x70;
	TheLights[1].vx = PlayersShip.coord.coord.t[0];
	TheLights[1].vy = -4096;			 // from below
	TheLights[1].vz = PlayersShip.coord.coord.t[2];
	GsSetFlatLight(1,&TheLights[1]);
}




/* �����̎��͂̃��f���f�[�^���� */
int calc_world_data (void)
{
	int	n;
	int	x, y;
	int	px, py;
	int	sx, sy, ex, ey;
	int	fact;

	fact = FACT;

	px = (view.vpx+fact/2)/fact;		// grid square coordinates
	py = (view.vpz+fact/2)/fact;


	sx = px - 4; ex = px + 4;		   // box around where view is now
	sy = py - 4; ey = py + 4;

	sx += ( rsin( PVect.vy )/1000 );
	sy += ( rcos( PVect.vy )/1000 );
	ex += ( rsin( PVect.vy )/1000 );
	ey += ( rcos( PVect.vy )/1000 );

	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WLDX_MAX ) ex = WLDX_MAX-1;
	if( ey>=WLDY_MAX ) ey = WLDY_MAX-1;

#if 0
	printf("OLD calc world data\n");
	printf("sx: %d, sy: %d, ex: %d, ey: %d\n", sx, sy, ex, ey);
#endif

	n = 0;				  // for every square in the around-view box of squares
	for( x=sx; x<=ex; x++ ) {
		for( y=sy; y<=ey; y++ ) {
			if( abs(px-x)<3 && abs(py-y)<3 ) {
				/* �߂��̃��f���̓|���S���������� */
				n = calc_model_data( n, x, y, fact, GsDIV1 );
			} else	{
				n = calc_model_data( n, x, y, fact, 0 );
			}
		}
	}
	return( n );
}





VECTOR p1, p2, p3, p4;
VECTOR radius, clockTgt, antiClockTgt;


int NewCalculateWorldData (void)
{	
	int numberOfModels;
	int	x, y;
	int px, py;
	int	sx, sy, ex, ey;		  // start x and y; end x and y
	int	fact;				  // distance

	fact = FACT;

	radius.vx = rsin(PlayersShip.rotate.vy);
	radius.vy = 0;
	radius.vz = rcos(PlayersShip.rotate.vy);

	clockTgt.vx = -radius.vz;
	clockTgt.vy = POSY;
	clockTgt.vz = radius.vx;

	antiClockTgt.vx = radius.vz;
	antiClockTgt.vy = POSY;
	antiClockTgt.vz = -radius.vx;


	p1.vx = PlayersShip.position.vx - (antiClockTgt.vx >> 3);
	p1.vy = PlayersShip.position.vy;
	p1.vz = PlayersShip.position.vz - (antiClockTgt.vz >> 3);
	p2.vx = PlayersShip.position.vx - (clockTgt.vx >> 3);
	p2.vy = PlayersShip.position.vy;
	p2.vz = PlayersShip.position.vz - (clockTgt.vz >> 3);
	p3.vx = PlayersShip.position.vx - ((2*radius.vx + antiClockTgt.vx) >> 3);
	p3.vy = PlayersShip.position.vy;
	p3.vz = PlayersShip.position.vz - ((2*radius.vz + antiClockTgt.vz) >> 3);
	p4.vx = PlayersShip.position.vx - ((2*radius.vx + clockTgt.vx) >> 3);
	p4.vy = PlayersShip.position.vy;
	p4.vz = PlayersShip.position.vz - ((2*radius.vz + clockTgt.vz) >> 3);	

	sx = (MinOfFour(p1.vx, p2.vx, p3.vx, p4.vx) + fact/2)/fact;
	sy = (MinOfFour(p1.vz, p2.vz, p3.vz, p4.vz) + fact/2)/fact;
	ex = (MaxOfFour(p1.vx, p2.vx, p3.vx, p4.vx) + fact/2)/fact;
	ey = (MaxOfFour(p1.vz, p2.vz, p3.vz, p4.vz) + fact/2)/fact;
   
	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WLDX_MAX ) ex = WLDX_MAX-1;
	if( ey>=WLDY_MAX ) ey = WLDY_MAX-1;

#if 0
	printf("NEW calc world data\n");
	printf("sx: %d, sy: %d, ex: %d, ey: %d\n", sx, sy, ex, ey);
	printf("angle vy: %d\nHERE is the radius", PlayersShip.rotate.vy);
	dumpVECTOR(&radius);
#endif

	px = (sx + ex) / 2;
	py = (sy + ey) / 2;

	//printf("px: %d, py: %d\n", px, py);

	numberOfModels = 0;				  // for every square in the around-view box of squares
	for (x = sx; x <= ex; x++) 
		{
		for (y = sy; y <= ey; y++) 
			{
			if( abs(px-x)<4 && abs(py-y)<4 ) 
				{
				numberOfModels = calc_model_data( numberOfModels, x, y, fact, GsDIV1 );
				} 
			else	
				{
				numberOfModels = calc_model_data( numberOfModels, x, y, fact, 0 );
				}

			if (numberOfModels > 256)
				{
				numberOfModels = 256;
				printf("Forcibly holding number of models at 256\n");
				goto end;
				}
			}
		}


end:

	//printf("About to return num.models: %d\n", numberOfModels);

	return(numberOfModels);
}





int MinOfFour (int a, int b, int c, int d)
{
	int result, t1, t2;

	t1 = min(a,b); t2 = min(c,d);
	result = min(t1, t2);

	return result;
}


int MaxOfFour (int a, int b, int c, int d)
{
	int result, t1, t2;

	t1 = max(a,b); t2 = max(c,d);
	result = max(t1, t2);

	return result;
}




	// calculate models around this single<px,py> square
int calc_model_data (int n, int px, int py, int fact, int atrb )
{
	if( worldmaps[px][py]&WALL ) return(n);

	if(( worldmaps[px-1][py]&WALL)|| px==0 ) {
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop4, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px+1][py]&WALL)|| px==WLDX_MAX-1 ) {
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop3, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py-1]&WALL) || py==0 ) {
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop1, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py+1]&WALL) || py==WLDY_MAX-1 ) {
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop2, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	GsInitCoordinate2(&World[px][py], &DWorld[n] );
	DWorld[n].coord.t[0] = 0;
	DWorld[n].coord.t[1] = - ( fact );
	DWorld[n].coord.t[2] = 0;
	GsLinkObject4((long)dop5, &Models[n],0);
	Models[n].coord2 = &DWorld[n];
	Models[n].attribute = atrb;
	n++;

	GsInitCoordinate2(&World[px][py], &DWorld[n] );
	DWorld[n].coord.t[0] = 0;
	DWorld[n].coord.t[1] = fact;
	DWorld[n].coord.t[2] = 0;
	GsLinkObject4((long)dop6, &Models[n],0);
	Models[n].coord2 = &DWorld[n];
	Models[n].attribute = atrb;
	n++;

	return( n );
}







void InitialiseAll (void)
{
	PadInit(0);


	GsInitGraph(PIH,PIV,GsINTER|GsOFSGPU,1,0);
	if( PIV<480 )
		GsDefDispBuff(0,0,0,PIV);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();		   

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	Sot[0].length=4;
	Sot[0].org=stags[0];
	Sot[1].length=4;
	Sot[1].org=stags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);
	GsClearOt(0,0,&Sot[0]);
	GsClearOt(0,0,&Sot[1]);

	InitialiseView();			
	InitialiseLighting();		   

	texture_init(TEX1_ADDR);	
	dop1 = model_init((long*)MODEL1_ADDR);	
	dop2 = model_init((long*)MODEL2_ADDR);	
	dop3 = model_init((long*)MODEL3_ADDR);	
	dop4 = model_init((long*)MODEL4_ADDR);	
	dop5 = model_init((long*)MODEL5_ADDR); 
	dop6 = model_init((long*)MODEL6_ADDR);	

	make_world_maps();
	clear_view_point();
	nModels = calc_world_data();

	InitialiseObjects();
	printf("position 3\n");

	MainMode = MOVE_VIEW;
	printf("position 4\n");

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before
	 printf("position 5\n");
	MovementSpeed = 12;
	RotationSpeed = 15;
	//PlayersShip.movementMomentumFlag = TRUE;

	PlayerFiringRate = 5;
	FramesSinceLastFire = 0;
	printf("position 6\n");

	PlayerBulletSpeedFactor = 7;
	EnemyBulletSpeedFactor = 8;
	printf("position 7\n");
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();

	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	RegisterObjectIntoObjectArray(&PlayersShip);

	//PlayersShip.position.vx = 4000;
	//PlayersShip.position.vz = 4000;
	PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.initialHealth = 10;
	PlayersShip.currentHealth = 10;
	PlayersShip.allegiance = PLAYER;
	PlayersShip.position.vy += 20;


		

	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>5, ONE>>5, ONE>>5);
		TheCubes[i].position.vy += 20;

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}



	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitSingleObject(&TheShips[i]);

		BringObjectToLife(&TheShips[i], ENEMY, 
			SHIP_MODEL_ADDRESS, 0, NONE);

		TheShips[i].initialHealth = 1;
		TheShips[i].currentHealth = 1;
			// set one of three strategies randomly
		TheShips[i].strategyFlag = rand() % NUMBER_ENEMY_STRATEGIES;
		InitialiseShipAccordingToStrategy(&TheShips[i]);

		PositionSomewhereNotInAWall(&TheShips[i]);
		TheShips[i].allegiance = ENEMY;

		TheShips[i].position.vy += 20;

		RegisterObjectIntoObjectArray(&TheShips[i]);
		}



	LinkAllObjectsToModels();
	LinkAllObjectsToTheirCoordinateSystems();



	printf("position 1\n");
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
	printf("position 2\n");
}




void InitialiseShipAccordingToStrategy (ObjectHandler* object)
{
	switch(object->strategyFlag)
		{
		case WAITING:	 
			object->specialTimeLimit = 400 + (100 * (rand() % 4));
			break;
		case STATIONARY_TRACK:		
		case SEEKING:
			break;
		case RANDOM:
			object->specialTimeLimit = 10 + (rand() % 10);
			break;
		default:
			assert(FALSE);
		}
}




 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int gridX, gridY;
	int i;

	printf("position 31\n");
	CheckCollisions();
	printf("position 32\n");

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		printf("i: %d\n", i);
		if (ObjectArray[i] != NULL)
			{
			printf("not null\n");
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];
				object->lifeTime++;
				printf("position 33\n");

				switch(object->type)
					{
					case PLAYER:
						printf("position 34\n");
						//UpdateObjectCoordinates(&object->twist, &object->position, 
						//		&object->velocity, &object->coord, &object->matrix);

						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

							// rotate wrt world, move wrt object
						UpdateObjectCoordinates3 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, WLDX_MAX*FACT);
						KeepWithinRange(object->position.vz, 0, WLDY_MAX*FACT);

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

						#if 0		// OLD DEBUG
						if (frameNumber % 20 == 0)
							{
							dumpMATRIX(&object->matrix);
							}
						#endif

						if (object->currentHealth < 1)
							HandlePlayersDeath();
						printf("position 35\n");
						break;
					case CUBE:
						printf("position 36\n");
						UpdateObjectCoordinates2(&object->rotate,  
							&object->position, &object->coord);

						SortObjectSize(object);

							// update angle
						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

							// update position
						object->position.vx += object->velocity.vx;
						object->position.vy += object->velocity.vy;
						object->position.vz += object->velocity.vz;

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

							
						gridX = object->position.vx / FACT;
						gridY = object->position.vz / FACT;   
						
						if (gridX < 0 || gridX >= WLDX_MAX
							|| gridY < 0 || gridY >= WLDY_MAX)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}
						else if (worldmaps[gridX][gridY] == WALL)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							} 

							// kill off after short time
						if (object->lifeTime > 100)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}

						printf("position 37\n");
						break;
					case ENEMY:
						printf("position 38\n");
						HandleEnemyBehaviour(object);

						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

							// rotate wrt world, move wrt object
						UpdateObjectCoordinates3 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, WLDX_MAX*FACT);
						KeepWithinRange(object->position.vz, 0, WLDY_MAX*FACT);

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
								
						if (object->currentHealth < 1)
							HandleEnemysDeath(object);
						printf("position 39\n");
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
printf("position 40\n");
}




VECTOR realMovement;
MATRIX xMatrix, yMatrix, zMatrix;
SVECTOR xVector, yVector, zVector;



	// this function does object-relative movement and rotation
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, velocity, &realMovement);
	
		// update position by super-relative movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;


	xVector.vx = twist->vx;
	xVector.vy = 0;
	xVector.vz = 0;
	yVector.vx = 0;
	yVector.vy = twist->vy;
	yVector.vz = 0;
	zVector.vx = 0;
	zVector.vy = 0;
	zVector.vz = twist->vz;

	RotMatrix(&xVector, &xMatrix);
	RotMatrix(&yVector, &yMatrix);
	RotMatrix(&zVector, &zMatrix);

		// to get world-relative rotations instead:
		// use same function with order of first two arguments switched

	MulMatrix0(matrix, &xMatrix, matrix);
	MulMatrix0(matrix, &yMatrix, matrix);
	MulMatrix0(matrix, &zMatrix, matrix);

	coordSystem->coord = *matrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}






MATRIX tempMatrix;

	// this does world-relative movement and rotation
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
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




VECTOR realMovement;


	// this does world-relative rotation and object-relative movement
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{ 
	int gridX, gridY;

		// copy the translating vector (position)
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];

		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the velocity in world coordinate terms
	ApplyMatrixLV(&tempMatrix, velocity, &realMovement);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// collision detection with walls: times two to be on safe side
	gridX = (position->vx + (realMovement.vx << 1)) / FACT;
	gridY = (position->vz + (realMovement.vz << 1)) / FACT;

	if (worldmaps[gridX][gridY] == WALL)	 // if about to hit wall, stop completely
		{
		velocity->vx = 0;
		velocity->vy = 0;
		velocity->vz = 0;
		}
	else
		{
			// update position by actual movement
		position->vx += realMovement.vx;
		position->vy += realMovement.vy;
		position->vz += realMovement.vz;
		} 

		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}








/* ���_�ݒ� */
void InitialiseView( void )
{
	GsSetProjection(192);	/* �v���W�F�N�V�����ݒ� */


	PVect.vx = PVect.vy = PVect.vz = 0;

#if 1
	view.vpx = WLDX_MAX/2*FACT; view.vpy = POSY; view.vpz = WLDY_MAX/2*FACT;

  	/* Setting focus point location : �����_�p�����[�^�ݒ� */
	view.vrx = 0; view.vry = POSY; view.vrz = 0;
	/* Setting bank of SCREEN : ���_�̔P��p�����[�^�ݒ� */
	view.rz=0;
	view.super = WORLD;		
#endif

	ReferenceDistance = 50;
	ViewpointZDistance = 220;
	ViewPointYDistance = -40;

#if 0
	view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
	view.vpx = 0; view.vpy = ViewPointYDistance; view.vpz = ViewpointZDistance;
	view.rz = 0;
	view.super = &PlayersShip.coord;
#endif

	GsSetRefView2(&view);
}





/* ���s�����ݒ� */
void InitialiseLighting(void)
{
	/* ���C�gID�O �ݒ� */
	TheLights[0].vx = 1; TheLights[0].vy= 1; TheLights[0].vz= 1;

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x80;

	GsSetFlatLight(0,&TheLights[0]);


	/* ���C�gID�P �ݒ� */
	TheLights[1].vx = -1; TheLights[1].vy= -1; TheLights[1].vz= 1;

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x80;

	GsSetFlatLight(1,&TheLights[1]);



	GsSetAmbient( ONE/4,ONE/4,ONE/4 );




	lightmode = 1;	  
	GsSetLightMode(lightmode);


		// fogging
	fogparam.dqa = -960;
	fogparam.dqb = 5120*5120;
		// inky-blue/black background colour
	fogparam.rfc = 0; fogparam.gfc = 0; fogparam.bfc = 4;
	GsSetFogParam(&fogparam);
}





/* �e�N�X�`���f�[�^��VRAM�Ƀ��[�h���� */
int texture_init(long addr)
{
	RECT rect;
	GsIMAGE tim1;

	/* TIM�f�[�^�̃w�b�_����e�N�X�`���̃f�[�^�^�C�v�̏��𓾂� */
	GsGetTimInfo((u_long *)(addr+4),&tim1);

	rect.x=tim1.px;	/* �e�N�X�`�������VRAM�ł�X���W */
	rect.y=tim1.py;	/* �e�N�X�`�������VRAM�ł�Y���W */
	rect.w=tim1.pw;	/* �e�N�X�`���� */
	rect.h=tim1.ph;	/* �e�N�X�`������ */

	/* VRAM�Ƀe�N�X�`�������[�h���� */
	LoadImage(&rect,tim1.pixel);
	/* Exist Color Lookup Table : �J���[���b�N�A�b�v�e�[�u�������݂��� */
	if((tim1.pmode>>3)&0x01) {
		rect.x=tim1.cx;	/* �N���b�g�����VRAM�ł�X���W */
		rect.y=tim1.cy;	/* �N���b�g�����VRAM�ł�Y���W */
		rect.w=tim1.cw;	/* Width of CLUT : �N���b�g�̕� */
		rect.h=tim1.ch;	/* Height of CLUT : �N���b�g�̍��� */
		/* �N���b�g�����[�h���� */
		LoadImage(&rect,tim1.clut);
	}
	DrawSync(0);
	return(0);
}



long* model_init( long* adrs )
{
	long *dop;

	dop=adrs;
	dop++;
	GsMapModelingData(dop);
	dop++;
	dop++;
	return( dop );
}







/* �n�`�f�[�^�쐬 */
void make_world_maps( void )
{
	int	x;
	int	y;
	int	fact;

		// these two chunks below:
		// for each block in world, set to WALL or nothing
	fact = FACT;
	for( x=0; x<WLDX_MAX; x+=3 ) {
		for( y=0; y<WLDY_MAX; y+=3 ) {
			worldmaps[x][y] = (rand()%4==0)?WALL:0;
			worldmaps[x][y+1] = worldmaps[x][y];
			worldmaps[x][y+2] = worldmaps[x][y];
			worldmaps[x+1][y] = worldmaps[x][y];
			worldmaps[x+2][y] = worldmaps[x][y];
			worldmaps[x+1][y+1] = worldmaps[x][y];
			worldmaps[x+1][y+2] = worldmaps[x][y];
			worldmaps[x+2][y+1] = worldmaps[x][y];
			worldmaps[x+2][y+2] = worldmaps[x][y];
		}
	}

	for( x=0; x<WLDX_MAX; x++ ) {
		for( y=0; y<WLDY_MAX; y++ ) {
			GsInitCoordinate2(WORLD, &World[x][y]);
			World[x][y].coord.t[0] = x*fact;
			World[x][y].coord.t[1] = 0;
			World[x][y].coord.t[2] = y*fact;
		}
	}
}




void clear_view_point( void )
{
	int	x, y;
	int	px, py;
	int	sx, sy, ex, ey;
	int	fact;

	fact = FACT;

	px = view.vpx/fact;
	py = view.vpz/fact;
	sx = px - 4; ex = px + 4;
	sy = py - 4; ey = py + 4;
	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WLDX_MAX ) ex = WLDX_MAX-1;
	if( ey>=WLDY_MAX ) ey = WLDY_MAX-1;

	for( x=sx; x<=ex; x++ ) {
		for( y=sy; y<=ey; y++ ) {
			worldmaps[x][y] = 0;
		}
	}
}


  


void PrintObject (ObjectHandler* object)
{
	printf("id: %d, alive: %d, type: %d, which: %d\n",
		object->id, object->alive, object->type, object->which);

	dumpOBJ2(&object->handler);
	printf("model: addr %u, which one %d\n",
		object->modelAddress, object->whichModel);

	printf("scaling flag: %d\n", object->scalingFlag);
	dumpVECTOR(&object->scalingVector);

	printf("movementTypeFlag: %d\n", object->movementTypeFlag);

	dumpVECTOR(&object->position);
	dumpVECTOR(&object->velocity);
	printf("movement momentum: %d\n",
		object->movementMomentumFlag);
		
	dumpVECTOR(&object->rotate);
	dumpVECTOR(&object->twist);
	printf("rotation momentum: %d\n",
		object->rotationMomentumFlag); 

	dumpCOORD2(&object->coord);
	printf("super coord object id: %d\n",
		object->superCoordinateObjectID);
	dumpMATRIX(&object->matrix);

	printf("inOrbit: %d, centralBodyID: %d\n, \
		angle: %d, angleIncrement: %d\n",
		object->inOrbit, object->centralBodyID,
		object->angle, object->angleIncrement);
	printf("whichWay: %d, radius: %d\n",
		object->whichWay, object->radius);
	dumpVECTOR(&object->firstVector);
	dumpVECTOR(&object->secondVector);

	printf("specialMovement: %d\n", object->specialMovement);

	printf("initialHealth: %d\n", object->initialHealth);
	printf("currentHealth: %d\n", object->currentHealth);

	printf("strategyFlag: %d\n", object->strategyFlag);

	printf("meritRating: %d\n", object->meritRating);
}






void PositionSomewhereNotInAWall (ObjectHandler* object)
{
	int gridX, gridY;
	int xOffset, yOffset;

	for (;;)
		{
		gridX = rand() % WLDX_MAX;
		gridY = rand() % WLDY_MAX;

		if (worldmaps[gridX][gridY] != WALL)
			break;
		}

	xOffset = rand() % FACT;
	yOffset = rand() % FACT;

	object->position.vx = (FACT * gridX) + xOffset;
	object->position.vz = (FACT * gridY) + yOffset;
}

	  



	// STUB ONLY
void HandleEnemyBehaviour (ObjectHandler* object)
{
	int x, z;
	int distance;
	int positionalAngle, turnAngle;
	int whichRandomAction;

	switch(object->strategyFlag)
		{
		case WAITING:	 // when player's ship comes close,
						// activate the sleeper -> active pursuer
			x = PlayersShip.position.vx - object->position.vx;
			z = PlayersShip.position.vz - object->position.vz;

			if (abs(x) < 500 && abs(z) < 500)
				object->strategyFlag = SEEKING;

			if (object->lifeTime > object->specialTimeLimit)
				object->strategyFlag = SEEKING;
			break;
		case STATIONARY_TRACK:		// position constant, point towards player
									// fire if clear line and close range
			x = PlayersShip.position.vx - object->position.vx;
			z = PlayersShip.position.vz - object->position.vz;

			positionalAngle = SUBratan(x,z);
			// NEW
			positionalAngle += 2048;
			positionalAngle &= 4095;
			// END NEW
			turnAngle = positionalAngle - object->rotate.vy;
			
			if (abs(turnAngle) >= 57)	// 5 degrees
				{
				if (turnAngle >= 0)
					object->twist.vy += (RotationSpeed << 2);	  // speedMultiplier
				else
					object->twist.vy -= (RotationSpeed << 2);
				}
			else			// small angle
				{
				if (abs(x) < 500 && abs(z) < 500)	 // if close up
					{
					if (frameNumber % 3 == 0)		 // fire often
						EnemyShipFiresAShot(object);
					}
				}
			break;
		case SEEKING:
			// NOT YET			
			// will turn towards as stat_track, but move if too far away
			break;
		case RANDOM:
			if (object->lifeTime % object->specialTimeLimit == 0
				|| object->currentActionFlag == NONE)		// start new action
				{
				whichRandomAction = rand() % NUMBER_RANDOM_ACTIONS;
				switch(whichRandomAction)
					{
					case 0:
						object->currentActionFlag = SWIVEL_LEFT;
						break;
					case 1:
						object->currentActionFlag = SWIVEL_RIGHT;
						break;
					case 2:
						object->currentActionFlag = MOVE_FORWARDS;
						break;
					case 3:
						object->currentActionFlag = MOVE_BACKWARDS;
						break;
					case 4:
						object->currentActionFlag = FIRING;
						break;
					}
				}
			else
				{		   // do action
				switch(object->currentActionFlag)
					{
					case SWIVEL_LEFT:
						object->twist.vy -= RotationSpeed << 2;
						break;
					case SWIVEL_RIGHT:
						object->twist.vy += RotationSpeed << 2;
						break;
					case MOVE_FORWARDS:
						object->velocity.vz += MovementSpeed << 2;
						break;
					case MOVE_BACKWARDS:
						object->velocity.vz -= MovementSpeed << 2;
						break;
					case FIRING:
						if (frameNumber % 2 == 0)			// rapid fire
							EnemyShipFiresAShot(object);
					break;
					default:
						assert(FALSE);
					}
				}
			break;
		default:
			assert(FALSE);
		}
}



			// STUB ONLY
void HandlePlayersDeath (void)
{
	printf("you have died\n");
	exit(1);
}




void HandleEnemysDeath (ObjectHandler* object)
{
	PlayersShip.meritRating 
		+= (object->initialHealth * (object->strategyFlag + 1));

	object->alive = FALSE;
}





	// collisions: enemy ships, player's ship, cubic bullets
int CheckCollisions (void)
{
	ObjectHandler* object;
	int i, j;

	printf("pos 50\n");
	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&TheShips[i], &PlayersShip))
				{
					// should kill both really
				printf("Player and Enemy collision\n");
				}
			}
		}
	printf("pos 51\n");

	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == TRUE && TheCubes[i].allegiance == ENEMY)
			{
			if (ObjectsVeryClose(&TheCubes[i], &PlayersShip))
				{
					// should kill both really
				printf("Player and bullet collision\n");
				}
			}
		} 
	printf("pos 52\n");

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			for (j = 0; j < MAX_CUBES; j++)
				{
				if (TheCubes[j].alive == TRUE && TheCubes[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&TheCubes[j], &TheShips[i]))
						{
							// should kill both really
						printf("ship and bullet collision\n");
						printf("cube %d, ship %d\n", j, i);
						}
					}
				}
			}
		}
	printf("pos 53\n");
}


	  





int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->position.vx - second->position.vx) > 50)
		return FALSE;
	if (abs(first->position.vy - second->position.vy) > 50)
		return FALSE;
	if (abs(first->position.vz - second->position.vz) > 50)
		return FALSE;

	return TRUE; 
}


   





#if 0		  // UNFINISHED

int ClearLineBetweenObjects	(ObjectHandler* first, ObjectHandler* second)
{
	int gridx1, gridy1;
	int gridx2, gridy2;
	int x, y;
	int distance;
	int booleanResult = TRUE;

	distance = pow( (pow(first->position.vx - second.position.vx,2) 
					+ pow(first->position.vz - second.position.vz,2)), 0.5);

	if (first->position.vx > second->position.vx)
		{
		for (x = first->position.vx; x < second->position.vx; x += FACT/2)
			{
			if (first->position.vz > second->position.vz)
				{
				for (y = first->position.vz; y < second->position.vz; y += FACT/2)
					{
					gridX = x / FACT;
					gridY = y / FACT;
					if (worldmaps[gridX][gridY] == WALL)
						{
						booleanResult = FALSE;
						goto end;
						}
					}
				}
			}
		}

end:
	return booleanResult;
}


#endif

	