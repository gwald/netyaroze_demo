#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "sin.h"
#include "pad.h"
#include "tmd.h"
#include "asssert.h"
#include "object.h"
#include "dump.h"




#define PIH 320
#define	PIV	240



#define PACKETMAX	2048		/* Max GPU packets */
#define OBJMAX		256		/* Max Objects */
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


#define OT_LENGTH	9		/* オーダリングテーブルの解像度 */


static GsOT	Sot[2];			/* Handler of OT */
static GsOT_TAG	stags[2][16];		/* オーダリングテーブル実体 */
static GsOT 	Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; /* オーダリングテーブル実体 */
static GsDOBJ2	Models[OBJMAX]; 	/* Array of Object Handler : */
static GsCOORDINATE2 DWorld[OBJMAX];



	// This vector: vy expresses angle theta-y, i.e. angle on plane
static SVECTOR	PVect;			/* 視点ベクトル	*/
static GsRVIEW2 view;			/* 視点を設定するための構造体 */
static GsF_LIGHT TheLights[3];		/* 平行光源を設定するための構造体 */
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
int MainMode;



#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif


int frameNumber = 0;



// object types
#define SHIP 0
#define CUBE 1

ObjectHandler PlayersShip;

#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];



int MovementSpeed;
int RotationSpeed;			  


int PlayerFiringRate;
int FramesSinceLastFire;


/****************************************************************************
					prototypes
****************************************************************************/
static void DealWithControllerPad(void);
void PlayerFiresAShot (void);
static void move_lighting_spot(void);
static void InitialiseAll(void);
void InitialiseObjects (void);
void HandleAllObjects (void);
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
static void InitialiseView(void);
static void InitialiseLighting(void);
static int texture_init(long addr);
static long* model_init(long* adrs);
static void make_world_maps(void);
static int calc_world_data(void);
static int calc_model_data(int n, int px, int py, int fact, int atrb);
static void draw_world_maps( GsOT* ot );
static int collision( int x, int y );
static void clear_view_point(void);

void PrintObject (ObjectHandler* object);
/****************************************************************************

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


	//FntLoad( 960, 256);
	//FntOpen( 64, 32, 256, 200, 0, 512);


	side = GsGetActiveBuff();

	while(1)
	{
		//if( PadRead(0)==PADselect ) break;
		//FntPrint("X = %d\nZ = %d\nV = %d\nN = %d\nH = %d\n",view.vpx,view.vpz,PVect.vy,nModels,hsync );
		FntPrint("X = %d\nZ = %d\nV = %d\n", view.vpx,view.vpz,PVect.vy);
		FntPrint("Num models: %d\nHsync = %d\n",nModels,hsync );
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("mode: ");
		switch(MainMode)
			{
			case MOVE_VIEW:
				FntPrint("move view\n");
				break;
			case MOVE_SHIP:
				FntPrint("move ship\n");
				break;
			default:
				assert(FALSE);
			}
		FntPrint("ship pos: \n%d %d %d\n", 
			PlayersShip.position.vx, 
			PlayersShip.position.vy,
			PlayersShip.position.vz); 
		frameNumber++;
		DealWithControllerPad();
		move_lighting_spot();
		nModels = calc_world_data();

		GsSetRefView2(&view);
		GsSetWorkBase((PACKET*)packetArea[side]);
		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);
		op = Models;
		for( i=0; i<nModels; i++ ) {
			GsGetLws(op->coord2,&tmplw, &tmpls);	/* ライトマトリックスをGTEにセットする */
			GsSetLightMatrix(&tmplw);	/* スクリーン／ローカルマトリックスを計算する */
			/*GsGetLs(op->coord2,&tmpls);	/* スクリーン／ローカルマトリックスをGTEにセットする */
			GsSetLsMatrix(&tmpls);		/* オブジェクトを透視変換しオーダリングテーブルに登録する */
			GsSortObject4(op,&Wot[side],3,getScratchAddr(0));
			op++;
		}

		HandleAllObjects();

		pointer = &(ObjectArray[0]);
		for (i = 0; i < MAX_OBJECTS; i++) 
			{
			if (*pointer != NULL)
				{
				if ( (*pointer)->alive == TRUE)
					{
					object = *pointer;

					#if 0
					if (frameNumber % 20 == 0)
						{
						PrintObject(object);
						}
					#endif

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


		draw_world_maps( &Sot[side] );
		hsync = VSync(2);
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		GsDrawOt(&Sot[side]);
		side ^= 1;
		FntFlush(-1);
	}
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
void DealWithControllerPad( void )
{
	int	spd;
	long	pad;
	long	range;
	short	vx, vz;	   // world-relative x, z displacements for this frame
	int speedMultiplier;
	static int framesSinceLastModeToggle = 0;
  
	pad = PadRead(0);
	
		// actions depend on main mode
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
			#if 0
			if (pad & PADselect)
				speedMultiplier = 10;
			else
				speedMultiplier = 1;
			#endif

			speedMultiplier = 4;

#if 0		// OLD: any xyz movement
				// movement
			if (pad & PADLright)
				PlayersShip.velocity.vx -= MovementSpeed * speedMultiplier;
			if (pad & PADLleft)
				PlayersShip.velocity.vx += MovementSpeed * speedMultiplier;
			if (pad & PADLup)
				PlayersShip.velocity.vy -= MovementSpeed * speedMultiplier;
			if (pad & PADLdown)
				PlayersShip.velocity.vy += MovementSpeed * speedMultiplier;
			if (pad & PADL2)
				PlayersShip.velocity.vz -= MovementSpeed * speedMultiplier;
			if (pad & PADL1)
				PlayersShip.velocity.vz += MovementSpeed * speedMultiplier;
#endif

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
				PlayersShip.twist.vy -= RotationSpeed * speedMultiplier;
			if (pad & PADRleft)
				PlayersShip.twist.vy += RotationSpeed * speedMultiplier;
			if (pad & PADRup)
				PlayersShip.twist.vx -= RotationSpeed * speedMultiplier;
			if (pad & PADRdown)
				PlayersShip.twist.vx += RotationSpeed * speedMultiplier;
			if (pad & PADR2)
				PlayersShip.twist.vz -= RotationSpeed * speedMultiplier;
			if (pad & PADR1)
				PlayersShip.twist.vz += RotationSpeed * speedMultiplier;
			break;
		default:
			assert(FALSE);
		}

		// select: toggles main mode
	if (pad & PADselect)
		{
		if (framesSinceLastModeToggle > 10)
			{
			MainMode = 1 - MainMode;
			framesSinceLastModeToggle = 0;
			}
		}
	framesSinceLastModeToggle++;


#if 0		// DISABLED
		// PADselect: step slowly
	if (pad & PADselect)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		}
#endif

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

		printf("firing now\n");
		TheCubes[cubeID].alive = TRUE;
		TheCubes[cubeID].lifeTime = 0;

			// start spinning
		TheCubes[cubeID].rotationMomentumFlag = TRUE;
		TheCubes[cubeID].twist.vx = rand() % 20;
		TheCubes[cubeID].twist.vy = rand() % 20;
			// send in direction of ship at time of fire
			// NOTE: this goes badly askew when ship not parallel to xz plane
		TheCubes[cubeID].movementMomentumFlag = TRUE;
		TheCubes[cubeID].velocity.vx = PlayersShip.coord.coord.m[2][0] >> 8;
		TheCubes[cubeID].velocity.vy = PlayersShip.coord.coord.m[2][1] >> 8;
		TheCubes[cubeID].velocity.vz = -PlayersShip.coord.coord.m[2][2] >> 8; 
		dumpVECTOR(&TheCubes[cubeID].velocity);
		
					// emitted from nose of ship
		TheCubes[cubeID].position.vx = PlayersShip.position.vx;
		TheCubes[cubeID].position.vy = PlayersShip.position.vy;
		TheCubes[cubeID].position.vz = PlayersShip.position.vz;
		dumpVECTOR(&TheCubes[cubeID].position);
		}
}




/* 衝突判定 */
int collision( int vx, int vz )
{
	int	wx, wz;

	wx = ((view.vpx+FACT/2)+vx);
	wz = ((view.vpz+FACT/2)+vz);
	if( wx<0 || wz<0 ) return(1);

	wx = wx/FACT; wz = wz/FACT;
	if( wx>=WLDX_MAX || wz<0 || wz>=WLDY_MAX ) return(1);

	return( worldmaps[wx][wz] );
}




/* 視点移動による光源ベクトル補正 */
void	move_lighting_spot( void )
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





/* 自分の周囲のモデルデータ生成 */
int calc_world_data(void)
{
	int	n;
	int	x, y;
	int	px, py;
	int	sx, sy, ex, ey;
	int	fact;

	fact = FACT;

	px = (view.vpx+fact/2)/fact;
	py = (view.vpz+fact/2)/fact;


	sx = px - 4; ex = px + 4;
	sy = py - 4; ey = py + 4;

	sx += ( rsin( PVect.vy )/1000 );
	sy += ( rcos( PVect.vy )/1000 );
	ex += ( rsin( PVect.vy )/1000 );
	ey += ( rcos( PVect.vy )/1000 );

	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WLDX_MAX ) ex = WLDX_MAX-1;
	if( ey>=WLDY_MAX ) ey = WLDY_MAX-1;

	n = 0;
	for( x=sx; x<=ex; x++ ) {
		for( y=sy; y<=ey; y++ ) {
			if( abs(px-x)<3 && abs(py-y)<3 ) {
				/* 近くのモデルはポリゴン分割する */
				n = calc_model_data( n, x, y, fact, GsDIV1 );
			} else	{
				n = calc_model_data( n, x, y, fact, 0 );
			}
		}
	}
	return( n );
}




int calc_model_data( int n, int px, int py, int fact, int atrb )
{
		// if wall at that spot, return input <n>
	if( worldmaps[px][py]&WALL ) return(n);

	/* 左壁 */
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
	/* 右壁 */
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

	/* 後壁 */
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
	/* 前壁 */
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

	/* 天井 */
	GsInitCoordinate2(&World[px][py], &DWorld[n] );
	DWorld[n].coord.t[0] = 0;
	DWorld[n].coord.t[1] = - ( fact );
	DWorld[n].coord.t[2] = 0;
	GsLinkObject4((long)dop5, &Models[n],0);
	Models[n].coord2 = &DWorld[n];
	Models[n].attribute = atrb;
	n++;

	/* 床 */
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

/* 初期化ルーチン群 */
void InitialiseAll( void )
{
	/* コントローラ値初期化 */
	PadInit(0);

	/* 解像度設定（ノンインターレースモード） */
	GsInitGraph(PIH,PIV,GsINTER|GsOFSGPU,1,0);
	if( PIV<480 )
		GsDefDispBuff(0,0,0,PIV);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();			/* ３Dシステム初期化 */

	Wot[0].length=OT_LENGTH;	/* オーダリングテーブルハンドラに解像度設定 */
	Wot[0].org=wtags[0];		/* オーダリングテーブルハンドラにオーダリングテーブルの実体設定 */
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

	InitialiseView();			/* 視点設定 */
	InitialiseLighting();			/* 平行光源設定 */

	texture_init(TEX1_ADDR);	/* テクスチャ読み込み */
	dop1 = model_init((long*)MODEL1_ADDR);	/* モデリングデータ読み込み */
	dop2 = model_init((long*)MODEL2_ADDR);	/* モデリングデータ読み込み */
	dop3 = model_init((long*)MODEL3_ADDR);	/* モデリングデータ読み込み */
	dop4 = model_init((long*)MODEL4_ADDR);	/* モデリングデータ読み込み */
	dop5 = model_init((long*)MODEL5_ADDR);	/* モデリングデータ読み込み */
	dop6 = model_init((long*)MODEL6_ADDR);	/* モデリングデータ読み込み */

	make_world_maps();
	clear_view_point();
	nModels = calc_world_data();

	InitialiseObjects();

	MainMode = MOVE_VIEW;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before

	MovementSpeed = 10;
	RotationSpeed = 10;

	PlayerFiringRate = 5;
	FramesSinceLastFire = 0;
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();

	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	RegisterObjectIntoObjectArray(&PlayersShip);

	PlayersShip.position.vx = 4000;
	PlayersShip.position.vz = 4000;

		

	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>6, ONE>>6, ONE>>6);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}



	LinkAllObjectsToModels();
	LinkAllObjectsToTheirCoordinateSystems();



	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
}




 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];
				object->lifeTime++;

				switch(object->type)
					{
					case SHIP:
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

						SortObjectSize(object);

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

						if (frameNumber % 20 == 0)
							{
							dumpMATRIX(&object->matrix);
							}
						break;
					case CUBE:
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

							// kill off after short time
						if (object->lifeTime > 100)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
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







/* 視点設定 */
void InitialiseView( void )
{
	GsSetProjection(192);	/* プロジェクション設定 */


	PVect.vx = PVect.vy = PVect.vz = 0;

#if 1
	view.vpx = WLDX_MAX/2*FACT; view.vpy = POSY; view.vpz = WLDY_MAX/2*FACT;

  	/* Setting focus point location : 注視点パラメータ設定 */
	view.vrx = 0; view.vry = POSY; view.vrz = 0;
	/* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
	view.rz=0;
	view.super = WORLD;		
#endif

#if 0
	view.vrx = 0; view.vry = 0; view.vrz = -3000;
	view.vpx = 0; view.vpy = 0; view.vpz = - 500;
	view.rz = 0;
	view.super = &PlayersShip.coord;
#endif

	GsSetRefView2(&view);
}

/* 平行光源設定 */
void InitialiseLighting(void)
{
	/* ライトID０ 設定 */
	TheLights[0].vx = 1; TheLights[0].vy= 1; TheLights[0].vz= 1;

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x80;

	GsSetFlatLight(0,&TheLights[0]);


	/* ライトID１ 設定 */
	TheLights[1].vx = -1; TheLights[1].vy= -1; TheLights[1].vz= 1;

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x80;

	GsSetFlatLight(1,&TheLights[1]);



	/* アンビエント設定 */
	GsSetAmbient( ONE/4,ONE/4,ONE/4 );
	/*GsSetAmbient(0,0,0);*/



	/* 光源計算のデフォルトの方式設定 */
	lightmode = 1;
	GsSetLightMode(lightmode);



	fogparam.dqa = -960;
	/*fogparam.dqb = 5120*4096;*/
	fogparam.dqb = 5120*5120;
	fogparam.rfc = 0; fogparam.gfc = 0; fogparam.bfc = 4;
	GsSetFogParam(&fogparam);
}


/* テクスチャデータをVRAMにロードする */
int texture_init(long addr)
{
	RECT rect;
	GsIMAGE tim1;

	/* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */
	GsGetTimInfo((u_long *)(addr+4),&tim1);

	rect.x=tim1.px;	/* テクスチャ左上のVRAMでのX座標 */
	rect.y=tim1.py;	/* テクスチャ左上のVRAMでのY座標 */
	rect.w=tim1.pw;	/* テクスチャ幅 */
	rect.h=tim1.ph;	/* テクスチャ高さ */

	/* VRAMにテクスチャをロードする */
	LoadImage(&rect,tim1.pixel);
	/* Exist Color Lookup Table : カラールックアップテーブルが存在する */
	if((tim1.pmode>>3)&0x01) {
		rect.x=tim1.cx;	/* クラット左上のVRAMでのX座標 */
		rect.y=tim1.cy;	/* クラット左上のVRAMでのY座標 */
		rect.w=tim1.cw;	/* Width of CLUT : クラットの幅 */
		rect.h=tim1.ch;	/* Height of CLUT : クラットの高さ */
		/* クラットをロードする */
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

/* 地形データ作成 */
void make_world_maps( void )
{
	int	x;
	int	y;
	int	fact;

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

	printf("inOrbit: %d, centralBodyID: %d, \
		angle: %d, angleIncrement: %d\n",
		object->inOrbit, object->centralBodyID,
		object->angle, object->angleIncrement);
	printf("whichWay: %d, radius: %d\n",
		object->whichWay, object->radius);
	dumpVECTOR(&object->firstVector);
	dumpVECTOR(&object->secondVector);

	printf("specialMovement: %d\n", object->specialMovement);
}

	  