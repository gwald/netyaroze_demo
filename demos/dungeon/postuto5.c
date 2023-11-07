/************************************************************
 *															*
 *						Tuto5.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     15/11/96							*		
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

#define	PIH	320
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



static SVECTOR	PVect;			/* 視点ベクトル	*/

static GsRVIEW2 view;			/* 視点を設定するための構造体 */


static GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */


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
int QuitFrameNumber = -1;




// object types
#define PLAYER 0
#define BULLET 1
#define ENEMY 2
#define CUBE 3

ObjectHandler PlayersShip;

#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];


#define MAX_SHIPS 20
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


   
int PlayerBulletSpeedFactor;
int EnemyBulletSpeedFactor;			  

	 



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


	


	// text handling
#define MAX_TEXT_STRINGS 20
#define MAX_STRING_LENGTH 50

char TextStrings[MAX_TEXT_STRINGS][MAX_STRING_LENGTH];	


int LevelNumber = 1;

int NumberEnemiesLeft = MAX_SHIPS;







#define NORTH_8_FOLD 0
#define NORTH_WEST_8_FOLD 1
#define WEST_8_FOLD 2
#define SOUTH_WEST_8_FOLD 3
#define SOUTH_8_FOLD 4
#define SOUTH_EAST_8_FOLD 5
#define EAST_8_FOLD 6
#define NORTH_EAST_8_FOLD 7


int DirectionToNearestEnemy = NONE;


/****************************************************************************

****************************************************************************/


static void obj_interactive( void );
static void move_lighting_spot( void );
static void init_all( void );
static void view_init( void );
static void light_init(void);
static int texture_init(long addr);
static long* model_init(long* adrs);
static void make_world_maps(void);
static int calc_world_data(void);
static int calc_model_data(int n, int px, int py, int fact, int atrb);
static void draw_world_maps( GsOT* ot );
static int collision( int x, int y );
static void clear_view_point( void );


/****************************************************************************

****************************************************************************/



void main( void )
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;
	MATRIX	tmpls, tmplw;

	init_all();

	//FntLoad( 960, 256);
	//FntOpen( 64, 32, 256, 200, 0, 512);

	side = GsGetActiveBuff();
	while(1)
	{
		if( PadRead(0)==PADselect ) break;
		FntPrint("X = %d\nZ = %d\nV = %d\nN = %d\nH = %d\n",view.vpx,view.vpz,PVect.vy,nModels,hsync );
		obj_interactive();
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

	box[1].attribute = 1<<30;
	setRECT( &box[1], line.x0-32, line.y0-32, 64, 64 );
	box[1].r = 128; box[1].g = 128; box[1].b = 128;

	GsSortLine( &line, ot, 2 );
	GsSortBoxFill( &box[0], ot, 2 );
	GsSortBoxFill( &box[1], ot, 2 );
}

#define	SPEED	2
void obj_interactive( void )
{
	int	spd;
	long	pad;
	long	range;
	short	vx, vz;

	pad = PadRead(0);

	spd = SPEED;
	if( pad & PADRright ) spd = spd*2;

	if(pad & PADLleft) {
		PVect.vy = (PVect.vy-spd*8)&4095;
	}

	if(pad & PADLright) {
		PVect.vy = (PVect.vy+spd*8)&4095;
	}

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
	if(pad & PADl) {
		vz += (rcos((PVect.vy+1024)&4095)/64*spd)/16;
		vx += (rsin((PVect.vy+1024)&4095)/64*spd)/16;
	}
	if(pad & PADn) {
		vz += (rcos((PVect.vy-1024)&4095)/64*spd)/16;
		vx += (rsin((PVect.vy-1024)&4095)/64*spd)/16;
	}

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
	pslt[0].r = pslt[0].g = pslt[0].b = 0x70;
	pslt[0].vx = view.vrx - view.vpx;
	pslt[0].vy = 4096;
	pslt[0].vz = view.vrz - view.vpz;;
	GsSetFlatLight(0,&pslt[0]);

	pslt[1].r = pslt[1].g = pslt[1].b = 0x70;
	pslt[1].vx = view.vrx - view.vpx;
	pslt[1].vy = -4096;
	pslt[1].vz = view.vrz - view.vpz;;
	GsSetFlatLight(1,&pslt[1]);
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
void init_all( void )
{
	/* コントローラ値初期化 */
	PadInit(0);

	FntLoad( 960, 256);
	FntOpen( 64, 32, 256, 200, 0, 512);

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

	view_init();			/* 視点設定 */
	light_init();			/* 平行光源設定 */

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
}




/* 視点設定 */
void view_init( void )
{
	GsSetProjection(192);	/* プロジェクション設定 */

	PVect.vx = PVect.vy = PVect.vz = 0;
	view.vpx = WLDX_MAX/2*FACT; view.vpy = POSY; view.vpz = WLDY_MAX/2*FACT;

  	/* Setting focus point location : 注視点パラメータ設定 */
	view.vrx = 0; view.vry = POSY; view.vrz = 0;
	/* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
	view.rz=0;
	view.super = WORLD;
	GsSetRefView2(&view);
}




/* 平行光源設定 */
void light_init(void)
{
	/* ライトID０ 設定 */
	pslt[0].vx = 1; pslt[0].vy= 1; pslt[0].vz= 1;
	pslt[0].r = pslt[0].g = pslt[0].b = 0x80;
	GsSetFlatLight(0,&pslt[0]);

	/* ライトID１ 設定 */
	pslt[1].vx = -1; pslt[1].vy= -1; pslt[1].vz= 1;
	pslt[1].r = pslt[1].g = pslt[1].b = 0x80;
	GsSetFlatLight(1,&pslt[1]);

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






	// random world initialisation
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




	// ensure all is clear around the viewpoint
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