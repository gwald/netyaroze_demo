/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 28th Feb 97                   *
*                                                              *
***************************************************************/

#include <libps.h>
#include "pad.h"
#include "tmd.h"
#include "sincos.h"

	// GPU packet space
#define PACKETMAX		5000
#define PACKETMAX2		(PACKETMAX*24)

#define VHAddr (u_char *)0x80090000
#define VBAddr (u_char *)0x80092400

#define MaxMapRows 20	 // maximum number of rows in the map
#define MaxMapCols 20	 // maximum number of columns in the map
#define MaxBadGuys 20	 // maximum number of bad guys
#define MaxShots  6	     // no. of shots that can exist at any time (good guy)

#define roofTIM     (u_long *)0x800d0000	// 4-bit, size 0x840
#define wallTIM     (u_long *)0x800d2000	// 4-bit
#define floorTIM    (u_long *)0x800d4000	// 4-bit
#define bulletTIM   (u_long *)0x800d6000	// 4-bit
#define bloodTIM    (u_long *)0x800d8000	// 4-bit
#define ricochetTIM (u_long *)0x800e0000    // 4-bit
#define healthTIM   (u_long *)0x800e2000    // 4-bit
#define bullet2TIM  (u_long *)0x800e4000    // 4-bit
#define eroofTIM    (u_long *)0x800e6000
#define ewallTIM    (u_long *)0x800e8000
#define efloorTIM   (u_long *)0x800ea000
#define fwallTIM    (u_long *)0x800ec000
#define faceTIM     (u_long *)0x800ee000
#define statusTIM   (u_long *)0x800c8000

#define man1TIM     (u_long *)0x800da000	// 8-bit, size 0x1220
#define man2TIM     (u_long *)0x800c2000	// 8-bit
#define deadmanTIM  (u_long *)0x800ca000	// 8-bit

#define map1Addr (u_char *)0x800c0000
#define map2Addr (u_char *)0x800c0800
#define map3Addr (u_char *)0x800c1000

#define floorTMD    (u_long *)0x800f0000
#define roofTMD     (u_long *)0x800f1000
#define wallTMD     (u_long *)0x800f2000
#define monster1TMD (u_long *)0x800f3000
#define monster2TMD (u_long *)0x800f4000
#define bulletTMD   (u_long *)0x800f5000
#define bloodTMD    (u_long *)0x800f6000
#define deadTMD     (u_long *)0x800f7000
#define ricochetTMD (u_long *)0x800f8000
#define healthTMD   (u_long *)0x800f9000
#define bullet2TMD  (u_long *)0x800fa000
#define eroofTMD    (u_long *)0x800fb000
#define ewallTMD    (u_long *)0x800fc000
#define efloorTMD   (u_long *)0x800fd000
#define fwallTMD    (u_long *)0x800fe000

#define lengthOT1  1
#define lengthOT2  10

#define setVector(v,_x,_y,_z) (v)->vx = (_x), (v)->vy = (_y), (v)->vz = (_z)
#define setNormal(n,_x,_y,_z) (n)->nx = (_x), (n)->ny = (_y), (n)->nz = (_z)
#define setColor(c,_r,_g,_b) (c)->r = (_r), (c)->g = (_g), (c)->b = (_b)

#define North 1     // bit 0 - north wall (0 no, 1 yes)
#define South 2     // bit 1 - south wall
#define East  4     // bit 2 - east wall
#define West  8     // bit 3 - west wall
#define Health 16	// bit 4 - first aid box
#define Fake 32     // bit 5 - fake (destroyable) block
#define Exit 64     // bit 6 - exit location

#define Border 200  // size of border

typedef struct
	{
	short active;	   // 1 means bullet is moving
	short available;   // 1 means bullet is ready to be fired
	short hit;         // 1 means bullet hit something
	int vsync;		   // to pause bullet after hit (allows blood, ricochet)
	VECTOR position;   // bullet position
	VECTOR increment;  // movement vector
	GsDOBJ2 *object;   // TMD to use for bullet
	} projectile;

typedef struct
	{
	short active;	   // 1 means bad guy is moving around
	long activeDist;   // distance at which enemy becomes active
	short direction;   // 0 north, 1 south, 2 east, 3 west, -1 none
	short facing;	   // direction facing, (toward good guy, until dead)
	short health;	   // health points
	short hitsize;	   // size of target, (0 never hit, 1024 within a square)
	VECTOR position;   // position
	projectile bullet; // each enemy has one bullet to fire
	int pvsync;         // pause for shooting
	GsDOBJ2 *object;   // TMD to use for enemy
	} BadGuy;

typedef struct
	{
	VECTOR position;			   // position in maze
	short health;				   // health points
	short kills;				   // number of bad guys killed
	short level;				   // current level
	long top;					   // top and bottom of bounce, and amount
	long bottom;
	long bounce;
	projectile bullet[MaxShots]; 	// bullets
	} GoodGuy;

// ordering tables for floor & roof, (so floor & roof can be drawn first)
GsOT		OT1[2];		       // Ordering Table header
GsOT_TAG	zTable1[2][1 << lengthOT1];       // actual Ordering Tables themselves

// ordering tables for rest of world
GsOT		OT2[2];		       // Ordering Table header
GsOT_TAG	zTable2[2][1 << lengthOT2];       // actual Ordering Tables themselves

PACKET		GpuOutputPacket[2][PACKETMAX2];	   // GPU packet work area

u_long PadStatus;	 // for the pad

// the TMD object handles
GsDOBJ2 floorOBJ, roofOBJ, wallOBJ;
GsDOBJ2 monster1OBJ, monster2OBJ, deadOBJ;
GsDOBJ2 bulletOBJ, bloodOBJ, ricochetOBJ, bullet2OBJ;
GsDOBJ2 healthOBJ,efloorOBJ,eroofOBJ,ewallOBJ,fwallOBJ;

GsVIEW2 TheMainView;  	  // viewing handle

unsigned short map[MaxMapRows][MaxMapCols]; // the map
unsigned char visited[MaxMapRows][MaxMapCols]; // areas of the map explored

unsigned char mapRows;					   // number of rows in map
unsigned char mapCols;					   // number of columns in map

BadGuy badguy[MaxBadGuys];				   // the bad guys
unsigned char numBadGuys;				   // number of bad guys
GoodGuy goodguy;						   // the good guy (the in-game you)

GsCOORDINATE2 present;	  // coordinate system for view from current position

long viewAngle = 0;	 	  // angle of rotation
int	buffIdx;							// current output buffer

short VAB;
short heartvoice;		  // voice used by heart beat
short levelvoice;
int playVsync;
char playingMusic;

// is map being displayed
char displayMap = 0;

// stuff to change exit colours
RECT exitFloorCLUTRect;
RECT exitWallCLUTRect;
RECT exitRoofCLUTRect;
u_short exitFloorCLUT[16];
u_short exitWallCLUT[16];
u_short exitRoofCLUT[16];
char exitCLUTInc = 1;
char exitCLUTVal = 0;
GsSPRITE status, face;
long fntPrintID1,fntPrintID2;

int main(void);
void initialiseTMD(u_long *tmdAddr, char objNum, GsDOBJ2 *obj);
void InitialiseTexture(u_long *timAddr);
void initialiseSprite(u_long *tmdAddr, GsSPRITE *sprite);
void makeTMD_G_4T(u_long *addr, u_long * timAddr, u_short u0, u_short v0,
					u_short u1, u_short v1, u_short u2, u_short v2, u_short u3, u_short v3,
					TMD_NORM norm0, TMD_VERT vert0, TMD_VERT vert1, TMD_VERT vert2, TMD_VERT vert3, char trans);
void makeSplitTMD_G_4T(u_long *addr, u_long *timAddr, u_short u0, u_short v0,
					u_short u1, u_short v1, u_short u2, u_short v2, u_short u3,
					u_short v3, TMD_NORM norm0, TMD_VERT vert0, TMD_VERT vert1,
					TMD_VERT vert2, TMD_VERT vert3, char trans);
int collisionX(VECTOR position, VECTOR increment);
int collisionZ(VECTOR position, VECTOR increment);
void fakeWalls(projectile *b);
void checkGoodGuyBullets(void);
int bulletCollision(projectile *b);
void checkBadGuys(void);
void moveBadGuy(BadGuy *b, VECTOR p);
void initProjectile(projectile *p, GsDOBJ2 *o);
void initMap(u_char *mp);
void drawObject(GsDOBJ2 *obj, short ang, VECTOR t, GsOT *ot);
void drawMap(short angle, VECTOR pos);
void getExitCLUTRect(void);
void grabExitCLUT(void);
void alterExitCLUT(void);
void checkForHealthPickUp(void);
void poundingHeart(void);
void drawHealthBox(int h);
void checkLevel(void);
void viewControl(void);
int inLineOfSight(long x1, long z1, long x2, long z2, short ang);
void initGoodGuy(GoodGuy *g, u_long x, u_long z);
void initBadGuy(BadGuy *b, u_long x, u_long z);
void initProjectile(projectile *p, GsDOBJ2 *o);
void setupTexturesAndTMDs(void);
void animateBadGuys(void);
void drawWalls(void);
	
int main(void)
{
	MATRIX transformMatrix;
	int i;
	VECTOR translate;
	SVECTOR rotate;
	long dx, dz;
	GsF_LIGHT flatLight;						   	// light source
	GsFOGPARAM fogging = {-1400,20971520L,0,0,0};  	// fogging
	int hsync;
	int maxHsync = 0;
	SVECTOR scale = {ONE, 3 * ONE / 4, ONE};

	ResetGraph(0);	// reset the graphics

	PadStatus = 0;	// set up controller pad
	PadInit(); 	
	
	if (GetVideoMode() == MODE_NTSC)
		GsInitGraph(320 ,240, GsOFSGPU|GsNONINTER, 1, 0);	// screen 320x240
	else
		GsInitGraph(320 ,256, GsOFSGPU|GsNONINTER, 1, 0);	// screen 320x256
									
	GsDefDispBuff(0, 0, 0, 256);   // display buffer
	GsInit3D();					   // 3D

	FntLoad(960, 256);					 // fonts
	fntPrintID1 = FntOpen(-30, 91, 128, 8, 0, 32); 
	fntPrintID2 = FntOpen(-137, -90, 128, 32, 0, 128); 

	for (i = 0; i < 2; i++)				 // setup ordering tables
		{
		OT1[i].length = lengthOT1;
		OT1[i].org = zTable1[i];
		OT2[i].length = lengthOT2;
		OT2[i].org = zTable2[i];
		}

	setupTexturesAndTMDs();				 // create polygon stuff
	getExitCLUTRect();

	VAB = SsVabTransfer(VHAddr, VBAddr, -1, 1);  // open sound stuff
	if (VAB < 0) 
		{
		printf("VAB open failed\n");
		exit(1);
		}

	SsUtKeyOn(VAB, 10, 0, 60, 0, 100, 100);			// background noise
	heartvoice = SsUtKeyOn(VAB, 0, 0, 60, 0, 0, 0);	 // heartbeat noise

	goodguy.level = 1;						   // start level, music
	initMap(map1Addr);
	levelvoice = SsUtKeyOn(VAB, 11, 0, 60, 0, 127, 127);
	playVsync = VSync(-1);
	playingMusic = 1;

	setColor(&flatLight, 0, 0, 0);  // flat light set up to remove any
	GsSetFlatLight(0, &flatLight);  // previous lighting effects
	GsSetFlatLight(1, &flatLight);
	GsSetFlatLight(2, &flatLight);
	GsSetLightMatrix(&GsIDMATRIX);

	GsSetProjection(280);			   // perpsective

	GsSetLightMode(1);				   // normal lighting, fogging on
	GsSetFogParam(&fogging);		   // degree of fogging
	GsSetAmbient(ONE/2,ONE/2,ONE/2);   // ambient light (RGB)

	do
		{
		// set up viewing coordinate system
		transformMatrix = GsIDMATRIX;
		TransMatrix(&transformMatrix,&goodguy.position);
		present.flg = 0;
		present.coord = transformMatrix;
		present.super = WORLD;

		// set up viewing rotation
		transformMatrix = GsIDMATRIX;
		setVector(&rotate,0,viewAngle,0);
		RotMatrix(&rotate,&transformMatrix);

		// set view transformation
		TheMainView.view = transformMatrix; 
		TheMainView.super = &present;
		GsSetView2(&TheMainView);

		// sort out stuff for ordering tables
		buffIdx = GsGetActiveBuff();

		// where all the polygons are copied to make up a scene
		GsSetWorkBase((PACKET*)GpuOutputPacket[buffIdx]);

		// clear out ordering table
		GsClearOt(0, 0, &OT1[buffIdx]);
		GsClearOt(0, 0, &OT2[buffIdx]);

		// scale the screen
		GsScaleScreen(&scale);

		// the game stuff
		checkForHealthPickUp();
		checkGoodGuyBullets();
		poundingHeart();
		checkBadGuys();
		checkLevel();
		animateBadGuys();
		drawWalls();
		drawHealthBox(goodguy.health);
		GsSortFastSprite(&face,&OT2[buffIdx],0);
		GsSortFastSprite(&status,&OT2[buffIdx],0);

		if (goodguy.health <= 0)   // back to the beginning if you die
			{
			goodguy.level = 1;
			initMap(map1Addr);
			levelvoice = SsUtKeyOn(VAB, 11, 0, 60, 0, 127, 127);
			playVsync = VSync(-1);
			playingMusic = 1;
			}

		if (displayMap)
			drawMap(viewAngle,goodguy.position);

		grabExitCLUT();	   // get colour of exit so it can flash

		DrawSync(0);	   // finish any drawing
		hsync = VSync(1);  // how long since last vsync
		VSync(0);
		
		GsSwapDispBuff();  // swap screens

		GsSortClear(0, 0, 0, &OT1[buffIdx]);  // add cls to ordering table
  		
 		GsDrawOt(&OT1[buffIdx]); 	// draw floor roof
		GsDrawOt(&OT2[buffIdx]); 	  // draw walls bullets bad guys

		PadStatus = PadRead();		// read the pad

		// info about draw speed
/*		FntPrint(fntPrintID2,"~cFFFhsync %d\n",hsync);
		// the hsync < 1000 is so the initialisation time is not included
		if (hsync > maxHsync && hsync < 1000)
			maxHsync = hsync;
		FntPrint(fntPrintID2,"max hsync %d\n",maxHsync);
*/
		FntPrint(fntPrintID1,"~cF00%d         %d/%d",goodguy.level,goodguy.kills,numBadGuys);

		alterExitCLUT();	// change the colour of the exit

		displayMap = 0;		// map is not displayed
		viewControl();		// do controls

		FntFlush(fntPrintID1);		// write info
		FntFlush(fntPrintID2);		// write info

	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

	SsUtAllKeyOff(0);	// switch off sounds
	SsVabClose(VAB);	// close sound
	ResetGraph(1);		// cleanup
	return 0;
}


//*****************************************************************************
// tie a handler to a TMD
void initialiseTMD(u_long *tmdAddr, char objNum, GsDOBJ2 *obj)
{
	u_long *otable;

	otable = tmdAddr + 3;

	GsLinkObject4((u_long)otable, obj, objNum);
	obj->coord2 = WORLD;
}


//*****************************************************************************
// initialise texture and clut
void InitialiseTexture(u_long *timAddr)
	{
	GsIMAGE image;
	RECT t,c;

	GsGetTimInfo(++timAddr,&image);

	setRECT(&t,image.px,image.py,image.pw,image.ph);
	LoadImage(&t,image.pixel);
	DrawSync(0);
		
	// load CLUT as necessary
	if (image.pmode & 8)
		{
		setRECT(&c,image.cx,image.cy,image.cw,image.ch);
		LoadImage(&c,image.clut);
		DrawSync(0);
		}
	}


//*****************************************************************************
// initialise sprite
void initialiseSprite(u_long *timAddr, GsSPRITE *sprite)
	{
	GsIMAGE image;
	RECT t,c;

	GsGetTimInfo(++timAddr,&image);

	setRECT(&t,image.px,image.py,image.pw,image.ph);
	LoadImage(&t,image.pixel);
	DrawSync(0);
		
	// load CLUT as necessary
	if (image.pmode & 8)
		{
		setRECT(&c,image.cx,image.cy,image.cw,image.ch);
		LoadImage(&c,image.clut);
		DrawSync(0);
		}

	sprite->attribute |= (image.pmode & 3) << 24;
	sprite->w = image.pw << (2 - (image.pmode & 3));
	sprite->h = image.ph;
	sprite->u = image.px & 31;
	sprite->v = image.py & 511;
	sprite->cx = image.cx;
	sprite->cy = image.cy;
	setColor(sprite,100,100,100);
	sprite->tpage = (image.px >> 6) + (image.py >> 10);
	}


//*****************************************************************************
// make a gouraud shaded textured 4 sided polygon within a TMD
// needs the address to put the TMD
// the CBA (thats the clutx / 16 + cluty *64, 0 if in 16 bit mode)
// the TSB (the texture page x / 64 + y / 256, plus info about
// semi trans 0 32 64 or 96, plus colour mode 0 128 or 256)
// u v coords within the texture (0,0 top left in the texture) for the vertices
// the normal direction (TMD_NORM is basically a SVECTOR)
// the vertices (TMD_VERT is again little more than a SVECTOR)
// order of vertices, this goes for texture coords aswell
//		0 +----+ 1
// 		 |     |
//     2 +----+ 3
// this procedure actually divides the given polygon into 4 smaller polygons
// the reason is there is less near clipping than with automatic sub division
void makeSplitTMD_G_4T(u_long *addr, u_long *timAddr, u_short u0, u_short v0,
					u_short u1, u_short v1, u_short u2, u_short v2, u_short u3,
					u_short v3, TMD_NORM norm0, TMD_VERT vert0, TMD_VERT vert1,
					TMD_VERT vert2, TMD_VERT vert3, char trans)
	{
	TMD_OBJ *obj_table;	// pointer to object table
	u_long *vert_table;	// pointer to vertices table
	u_long *norm_table;	// pointer to normals table
	u_long *prim_table;	// pointer to primative table
	u_short CBA,TSB,pmode,pixScale;
	GsIMAGE image;
	RECT t,c;

	GsGetTimInfo(++timAddr,&image);

	pmode = (image.pmode & 3);
	CBA = (image.cy << 6) + (image.cx >> 4);
	TSB = (image.px >> 6) + (image.py >> 10) + (pmode << 7);

	if (trans > 0)
		TSB += trans << 5;
		
	// add on offset from start of texture page
	if (pmode == 0)
		pixScale = 4;
	else if (pmode == 1)
		pixScale = 2;
	else 
		pixScale = 1;

	u0 += (image.px & 63) * pixScale; v0 += (image.py) & 511;
	u1 += (image.px & 63) * pixScale; v1 += (image.py) & 511;
	u2 += (image.px & 63) * pixScale; v2 += (image.py) & 511;
	u3 += (image.px & 63) * pixScale; v3 += (image.py) & 511;

	// ******** TMD header info ********
	setTMD_HDR((TMD_HDR *)addr, 0x00000041, 1, 1);		
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // move to end of header

	// ******** skip object table, values setup later ********
	obj_table = (TMD_OBJ *)addr;
	addr = (u_long *)((TMD_OBJ *)addr + 1);

	// ******** primative table ********
	prim_table = addr;
	// primative info, specific to type of primative
	if (trans < 0)
		setPRIM_HDR((PRIM_HDR *)addr, G_4T, 0, 8, 12);
	else
		setPRIM_HDR((PRIM_HDR *)addr, G_4T | ABE, 0, 8, 12);
	addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

	// primative data
	setTMD_G_4T((TMD_G_4T *)addr,CBA,TSB,u0,v0,u0+(u1-u0)/2,v0+(v1-v0)/2,u0+(u2-u0)/2,v0+(v2-v0)/2,u0+(u3-u0)/2,v0+(v3-v0)/2,0,0,0,0,0,1,3,4);
	addr = (u_long *)((TMD_G_4T *)addr + 1);  // move to end of primative data

	// primative info, specific to type of primative
	if (trans < 0)
		setPRIM_HDR((PRIM_HDR *)addr, G_4T, 0, 8, 12);
	else
		setPRIM_HDR((PRIM_HDR *)addr, G_4T | ABE, 0, 8, 12);
	addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

	// primative data
	setTMD_G_4T((TMD_G_4T *)addr,CBA,TSB,u0+(u1-u0)/2,v0+(v1-v0)/2,u1,v1,u0+(u3-u0)/2,v0+(v3-v0)/2,u1+(u3-u1)/2,v1+(v3-v1)/2,0,0,0,0,1,2,4,5);
	addr = (u_long *)((TMD_G_4T *)addr + 1);  // move to end of primative data

	// primative info, specific to type of primative
	if (trans < 0)
		setPRIM_HDR((PRIM_HDR *)addr, G_4T, 0, 8, 12);
	else
		setPRIM_HDR((PRIM_HDR *)addr, G_4T | ABE, 0, 8, 12);
	addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

	// primative data
	setTMD_G_4T((TMD_G_4T *)addr,CBA,TSB,u0+(u2-u0)/2,v0+(v2-v0)/2,u0+(u3-u0)/2,v0+(v3-v0)/2,u2,v2,u2+(u3-u2)/2,v2+(v3-v2)/2,0,0,0,0,3,4,6,7);
	addr = (u_long *)((TMD_G_4T *)addr + 1);  // move to end of primative data

	// primative info, specific to type of primative
	if (trans < 0)
		setPRIM_HDR((PRIM_HDR *)addr, G_4T, 0, 8, 12);
	else
		setPRIM_HDR((PRIM_HDR *)addr, G_4T | ABE, 0, 8, 12);
	addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

	// primative data
	setTMD_G_4T((TMD_G_4T *)addr,CBA,TSB,u0+(u3-u0)/2,v0+(v3-v0)/2,u1+(u3-u1)/2,v1+(v3-v1)/2,u2+(u3-u2)/2,v2+(v3-v2)/2,u3,v3,0,0,0,0,4,5,7,8);
	addr = (u_long *)((TMD_G_4T *)addr + 1);  // move to end of primative data

	// ******** vertices table ********
	vert_table = addr;
	// vertices and normal data
	setTMD_VERT((TMD_VERT *)addr, vert0.vx, vert0.vy, vert0.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert0.vx + (vert1.vx - vert0.vx) / 2,	vert0.vy + (vert1.vy - vert0.vy) / 2, vert0.vz + (vert1.vz - vert0.vz) / 2);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert1.vx, vert1.vy, vert1.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert0.vx + (vert2.vx - vert0.vx) / 2, vert0.vy + (vert2.vy - vert0.vy) / 2, vert0.vz + (vert2.vz - vert0.vz) / 2);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert0.vx + (vert3.vx - vert0.vx) / 2, vert0.vy + (vert3.vy - vert0.vy) / 2, vert0.vz + (vert3.vz - vert0.vz) / 2);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert1.vx + (vert3.vx - vert1.vx) / 2, vert1.vy + (vert3.vy - vert1.vy) / 2, vert1.vz + (vert3.vz - vert1.vz) / 2);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert2.vx, vert2.vy, vert2.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert2.vx + (vert3.vx - vert2.vx) / 2, vert2.vy + (vert3.vy - vert2.vy) / 2, vert2.vz + (vert3.vz - vert2.vz) / 2);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert3.vx, vert3.vy, vert3.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	// ******** normals table ********
	norm_table = addr;
	setTMD_NORM((TMD_NORM *)addr, norm0.nx, norm0.ny, norm0.nz);

	// fill in object table details
	setTMD_OBJ(obj_table, vert_table, 9, norm_table, 1, prim_table, 4, 0);
	}

// this does the same job as the above, but doesn't worry about dividing the polygon
void makeTMD_G_4T(u_long *addr, u_long *timAddr, u_short u0, u_short v0,
					u_short u1, u_short v1, u_short u2, u_short v2, u_short u3,
					u_short v3, TMD_NORM norm0, TMD_VERT vert0, TMD_VERT vert1,
					TMD_VERT vert2, TMD_VERT vert3, char trans)
	{
	TMD_OBJ *obj_table;	// pointer to object table
	u_long *vert_table;	// pointer to vertices table
	u_long *norm_table;	// pointer to normals table
	u_long *prim_table;	// pointer to primative table
	u_short CBA,TSB,pmode,pixScale;
	GsIMAGE image;
	RECT t,c;

	GsGetTimInfo(++timAddr,&image);

	pmode = (image.pmode & 3);
	CBA = (image.cy << 6) + (image.cx >> 4);
	TSB = (image.px >> 6) + (image.py >> 10) + (pmode << 7);

	if (trans > 0)
		TSB += trans << 5;
		
	// add on offset from start of texture page
	if (pmode == 0)
		pixScale = 4;
	else if (pmode == 1)
		pixScale = 2;
	else 
		pixScale = 1;

	u0 += (image.px & 63) * pixScale; v0 += (image.py) & 511;
	u1 += (image.px & 63) * pixScale; v1 += (image.py) & 511;
	u2 += (image.px & 63) * pixScale; v2 += (image.py) & 511;
	u3 += (image.px & 63) * pixScale; v3 += (image.py) & 511;

	// ******** TMD header info ********
	setTMD_HDR((TMD_HDR *)addr, 0x00000041, 1, 1);		
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // move to end of header

	// ******** skip object table, values setup later ********
	obj_table = (TMD_OBJ *)addr;
	addr = (u_long *)((TMD_OBJ *)addr + 1);

	// ******** primative table ********
	prim_table = addr;
	// primative info, specific to type of primative
	if (trans < 0)
		setPRIM_HDR((PRIM_HDR *)addr, G_4T, 0, 8, 12);
	else
		setPRIM_HDR((PRIM_HDR *)addr, G_4T | ABE, 0, 8, 12);
	addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

	// primative data
	setTMD_G_4T((TMD_G_4T *)addr,CBA,TSB,u0,v0,u1,v1,u2,v2,u3,v3,0,0,0,0,0,1,2,3);
	addr = (u_long *)((TMD_G_4T *)addr + 1);  // move to end of primative data

	// ******** vertices table ********
	vert_table = addr;
	// vertices and normal data
	setTMD_VERT((TMD_VERT *)addr, vert0.vx, vert0.vy, vert0.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert1.vx, vert1.vy, vert1.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert2.vx, vert2.vy, vert2.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	setTMD_VERT((TMD_VERT *)addr, vert3.vx, vert3.vy, vert3.vz);
	addr = (u_long *)((TMD_VERT *)addr + 1);
	
	// ******** normals table ********
	norm_table = addr;
	setTMD_NORM((TMD_NORM *)addr, norm0.nx, norm0.ny, norm0.nz);

	// fill in object table details
	setTMD_OBJ(obj_table, vert_table, 4, norm_table, 1, prim_table, 1, 0);
	}


//*****************************************************************************
// collision east or west for the good guy
int collisionX(VECTOR position, VECTOR increment)
	{
	static short r,c,nr,nc;

	r = position.vz >> 10;
	c = position.vx >> 10;
	nr = position.vz + increment.vz;
	nc = position.vx + increment.vx;

	if (nc < Border)
		return 1;
	if (map[r][c] & East && (nc + Border) >> 10 > c)	// hit east wall
		return 1;
	if (map[r][c] & West && (nc - Border) >> 10 < c)	// hit west wall
		return 1;
	if ((position.vz & 1023) <= Border)
		{
		if (map[r - 1][c] & East && (nc + Border) >> 10 > c)  // too close to ajoining east wall
			return 1;
		if (map[r - 1][c] & West && (nc - Border) >> 10 < c)  // too close to ajoining west wall
			return 1;
		}
	if ((position.vz & 1023) >= 1024 - Border)
		{
		if (map[r + 1][c] & East && (nc + Border) >> 10 > c)  // too close to ajoining east wall
			return 1;
		if (map[r + 1][c] & West && (nc - Border) >> 10 < c)  // too close to ajoining west wall
			return 1;
		}

	return 0;
	}


//*****************************************************************************
// collision north or south for the good guy
int collisionZ(VECTOR position, VECTOR increment)
	{
	static short r,c,nr,nc;

	r = position.vz >> 10;
	c = position.vx >> 10;
	nr = position.vz + increment.vz;
	nc = position.vx + increment.vx;

	if (nr < Border)
		return 1;
	if (map[r][c] & North && (nr + Border) >> 10 > r)	// hit north wall
		return 1;
	if (map[r][c] & South && (nr - Border) >> 10 < r)   // hit south wall
		return 1;
	if ((position.vx & 1023) <= Border)
		{
		if (map[r][c - 1] & North && (nr + Border) >> 10 > r)  // too close to ajoining north wall
			return 1;
		if (map[r][c - 1] & South && (nr - Border) >> 10 < r)  // too close to ajoining south wall
			return 1;
		}
	if ((position.vx & 1023) >= 1024 - Border)
		{
		if (map[r][c + 1] & North && (nr + Border) >> 10 > r)  // too close to ajoining north wall
			return 1;
		if (map[r][c + 1] & South && (nr - Border) >> 10 < r)  // too close to ajoining south wall
			return 1;
		}
	return 0;
	}


//*****************************************************************************
// check if a fake wall has been hit, and if so remove it
// this assumes that a fake wall is a whole square, and removes it accordingly
void fakeWalls(projectile *b)
	{
	short r,c;
	short nr,nc;

	r = b->position.vz >> 10;					    // current row
	c = b->position.vx >> 10;					    // current column

	nr = (b->position.vz + b->increment.vz) >> 10;
	nc = (b->position.vx + b->increment.vx) >> 10;

	if (nr > r && nr < MaxMapRows && map[nr][c] & Fake)	// hit fake north wall
		{
		map[nr][c] &= ~Fake;
		if (nr - 1 >= 0)
			map[nr - 1][c] &= ~North;
		if (nr + 1 < MaxMapRows)
			map[nr + 1][c] &= ~South;
		if (c - 1 >= 0)
			map[nr][c - 1] &= ~East;
		if (c + 1 < MaxMapCols)
			map[nr][c + 1] &= ~West;
		}
	if (nr < r && nr >= 0 && map[nr][c] & Fake)			// hit fake south wall
		{
		map[nr][c] &= ~Fake;
		if (nr - 1 >= 0)
			map[nr - 1][c] &= ~North;
		if (nr + 1 < MaxMapRows)
			map[nr + 1][c] &= ~South;
		if (c - 1 >= 0)
			map[nr][c - 1] &= ~East;
		if (c + 1 < MaxMapCols)
			map[nr][c + 1] &= ~West;
		}
	if (nc > c && nc < MaxMapCols && map[r][nc] & Fake)	// hit fake east wall
		{
		map[r][nc] &= ~Fake;
		if (r - 1 >= 0)
			map[r - 1][nc] &= ~North;
		if (r + 1 < MaxMapRows)
			map[r + 1][nc] &= ~South;
		if (nc - 1 >= 0)
			map[r][nc - 1] &= ~East;
		if (nc + 1 < MaxMapCols)
			map[r][nc + 1] &= ~West;
		}
	if (nc < c && nc >= 0 && map[r][nc] & Fake)		 	// hit fake west wall
		{
		map[r][nc] &= ~Fake;
		if (r - 1 >= 0)
			map[r - 1][nc] &= ~North;
		if (r + 1 < MaxMapRows)
			map[r + 1][nc] &= ~South;
		if (nc - 1 >= 0)
			map[r][nc - 1] &= ~East;
		if (nc + 1 < MaxMapCols)
			map[r][nc + 1] &= ~West;
		}
	}

//*****************************************************************************
// check if bullet, b, has hit a wall
int bulletCollision(projectile *b)
	{
	short r,c;
	short nr,nc;

	r = b->position.vz >> 10;					    // current row
	c = b->position.vx >> 10;					    // current column

	nr = (b->position.vz + b->increment.vz) >> 10;
	nc = (b->position.vx + b->increment.vx) >> 10;

	if (nr > r && map[r][c] & North)	 // hit north wall
		return 1;
	if (nr < r && map[r][c] & South)	// hit south wall
		return 1;
	if (nc > c && map[r][c] & East) 	// hit east wall
		return 1;
	if (nc < c && map[r][c] & West) 	// hit west wall
		return 1;

	// check if hit any corners
	if (nr > r && nc > c && map[nr][c] & East && map[r][nc] & North)  // hit NE
		return 1; 
	if (nr > r && nc < c && map[nr][c] & West && map[r][nc] & North)  // hit NW
	 	return 1; 
	if (nr < r && nc > c && map[nr][c] & East && map[r][nc] & South)  // hit SE
		return 1; 
	if (nr < r && nc < c && map[nr][c] & West && map[r][nc] & South)  // hit SW
		return 1;

	return 0;
	}

//*****************************************************************************
void checkGoodGuyBullets(void)
	{
	short hitsize;
	long dx, dz;
	int i,j;

	for(i = 0; i < MaxShots; i++)
		{
		if (goodguy.bullet[i].active)
			{
			if (bulletCollision(&goodguy.bullet[i]))
				{
				fakeWalls(&goodguy.bullet[i]);	 // deal with fake walls

				goodguy.bullet[i].active = 0;
				goodguy.bullet[i].hit = 1;
				goodguy.bullet[i].vsync = VSync(-1);
				goodguy.bullet[i].object = &ricochetOBJ;
				}

			// check out bad guys
			for(j = 0; j < numBadGuys; j++)
				if (badguy[j].active)
					{
					hitsize = badguy[j].hitsize >> 1;
					dx = goodguy.bullet[i].position.vx - badguy[j].position.vx;
					dz = goodguy.bullet[i].position.vz - badguy[j].position.vz;

					if (fabs(dx) < hitsize && fabs(dz) < hitsize) // within range
						{
						SsUtKeyOn(VAB, 8, 0, 60, 0, 127, 127); // squish 
						goodguy.bullet[i].active = 0;
						goodguy.bullet[i].vsync = VSync(-1);
						goodguy.bullet[i].hit = 1;
						goodguy.bullet[i].object = &bloodOBJ;

						if (--badguy[j].health <= 0)  // reduce health, check still alive
							{
							SsUtKeyOn(VAB, 6, 0, 60, 0, 127, 127);	// scream
							badguy[j].active = 0;
							badguy[j].object = &deadOBJ;
							goodguy.kills++;
							}
						}
					}

			// after above is the bullet still active, if so inc position
			if (goodguy.bullet[i].active)
				{
				goodguy.bullet[i].position.vx += goodguy.bullet[i].increment.vx;
				goodguy.bullet[i].position.vz += goodguy.bullet[i].increment.vz;
				}
			}

		// draw bullet if it is not available to be fired
		if (!goodguy.bullet[i].available)
			{
			if (inLineOfSight(goodguy.bullet[i].position.vx, goodguy.bullet[i].position.vz, goodguy.position.vx, goodguy.position.vz, viewAngle))
				drawObject(goodguy.bullet[i].object,-viewAngle,goodguy.bullet[i].position,&OT2[buffIdx]);
			// check delay
			if (goodguy.bullet[i].hit && VSync(-1) - goodguy.bullet[i].vsync > 7)
				{
				goodguy.bullet[i].available = 1;
				goodguy.bullet[i].hit = 0;
				goodguy.bullet[i].object = &bulletOBJ;
				}
			}
		}
	}

//*****************************************************************************
// check out bad guys
void checkBadGuys(void)
	{
	int i;
	long dx,dz;

	for(i = 0; i < numBadGuys; i++)
		{
		if (badguy[i].active)
			{
	
			// do shooting
			if (badguy[i].bullet.available)
				{
				dx = goodguy.position.vx - badguy[i].position.vx;
				dz = goodguy.position.vz - badguy[i].position.vz;
				if (inLineOfSight(goodguy.position.vx,goodguy.position.vz,badguy[i].position.vx,badguy[i].position.vz,rinvtan(dz,-dx)))
					{
					badguy[i].bullet.active = 1;
					badguy[i].bullet.available = 0;
					badguy[i].bullet.vsync = VSync(-1);
					badguy[i].bullet.position.vx = badguy[i].position.vx;
					badguy[i].bullet.position.vz = badguy[i].position.vz;
					badguy[i].bullet.increment.vx = rsin(rinvtan(dz, dx)) >> 4;
					badguy[i].bullet.increment.vz = rcos(rinvtan(dz, dx)) >> 4;
					SsUtKeyOn(VAB, 5, 0, 60, 0, 100, 100);
					}
				}

			moveBadGuy(&badguy[i],goodguy.position);
			}
		else if (badguy[i].health > 0)
			{
			if (fabs(badguy[i].position.vx - goodguy.position.vx) < badguy[i].activeDist)
				if (fabs(badguy[i].position.vz - goodguy.position.vz) < badguy[i].activeDist)
					badguy[i].active = 1;
			}

			// draw bad guys
			if (inLineOfSight(badguy[i].position.vx, badguy[i].position.vz, goodguy.position.vx, goodguy.position.vz, viewAngle))
				{
				dx = goodguy.position.vx - badguy[i].position.vx;
				dz = goodguy.position.vz - badguy[i].position.vz;
				drawObject(badguy[i].object,2048 - rinvtan(dz,-dx),badguy[i].position,&OT2[buffIdx]);
				}


			// check out fired bullets
			if (badguy[i].bullet.active)
				{
				if (bulletCollision(&badguy[i].bullet))
					badguy[i].bullet.active = 0;
	
				badguy[i].bullet.position.vx += badguy[i].bullet.increment.vx;
				badguy[i].bullet.position.vz += badguy[i].bullet.increment.vz;

				if (fabs(badguy[i].bullet.position.vx - goodguy.position.vx) < 125 && fabs(badguy[i].bullet.position.vz - goodguy.position.vz) < 125)
					{
					SsUtKeyOn(VAB, 2, 0, 60, 0, 127, 127);	// been shot
					badguy[i].bullet.active = 0;
					goodguy.health--;
					}
				else if (inLineOfSight(badguy[i].bullet.position.vx, badguy[i].bullet.position.vz, goodguy.position.vx, goodguy.position.vz, viewAngle))
					{
					drawObject(badguy[i].bullet.object,-viewAngle,badguy[i].bullet.position,&OT2[buffIdx]);
					}
				}
			else if (!badguy[i].bullet.available && VSync(-1) - badguy[i].bullet.vsync > badguy[i].pvsync)
				badguy[i].bullet.available = 1;
		}
	}		

//*****************************************************************************
// move bad guy, b, toward point, p
void moveBadGuy(BadGuy *b, VECTOR p)
	{
	unsigned short r,c;

	// only move when in the centre of a map location
	if ((b->position.vz & 1023) == 512 && (b->position.vx & 1023) == 512)
		{
		r = b->position.vz >> 10;
		c = b->position.vx >> 10;

		switch(b->direction)
			{
			case 0:							 	   // current movement north
				// go east, or west in that priority if needed, otherwise go
				// north, east, west, or south in that priority, if possible
			    if (!(map[r][c] & East) && b->position.vx < p.vx)
					b->direction = 2;
		   		else if (!(map[r][c] & West) && b->position.vx > p.vx)
					b->direction = 3;
			   	else if (!(map[r][c] & North))
					b->direction = 0;
			   	else if (!(map[r][c] & East))
					b->direction = 2;
				else if (!(map[r][c] & West))
					b->direction = 3;
				else if (!(map[r][c] & South))
					b->direction = 1;
				else
					b->direction = -1;
				break;
			case 1:							 	   // current movement south
				// go west, or east in that priority if needed, otherwise go
				// south, west, east, or north in that priority, if possible
		   		if (!(map[r][c] & West) && b->position.vx > p.vx)
					b->direction = 3;
			    else if (!(map[r][c] & East) && b->position.vx < p.vx)
					b->direction = 2;
				else if (!(map[r][c] & South))
					b->direction = 1;
				else if (!(map[r][c] & West))
					b->direction = 3;
			   	else if (!(map[r][c] & East))
					b->direction = 2;
			   	else if (!(map[r][c] & North))
					b->direction = 0;
				else
					b->direction = -1;
				break;
			case 2:									   // current movement east
				// go south, or north in that priority if needed, otherwise go
				// east, south, north, or west in that priority, if possible
				if (!(map[r][c] & South) && b->position.vz > p.vz)
					b->direction = 1;
			   	else if (!(map[r][c] & North) && b->position.vz < p.vz)
					b->direction = 0;
			    else if (!(map[r][c] & East))
					b->direction = 2;
			    else if (!(map[r][c] & South))
					b->direction = 1;
		   		else if (!(map[r][c] & North))
					b->direction = 0;
		   		else if (!(map[r][c] & West))
					b->direction = 3;
				else
					b->direction = -1;
				break;
			case 3:									   // current movement west
				// go north, or south in that priority if needed, otherwise go
				// west, north, south, or east in that priority, if possible
			   	if (!(map[r][c] & North) && b->position.vz < p.vz)
					b->direction = 0;
				else if (!(map[r][c] & South) && b->position.vz > p.vz)
					b->direction = 1;
		   		else if (!(map[r][c] & West))
					b->direction = 3;
			    else if (!(map[r][c] & North))
					b->direction = 0;
		   		else if (!(map[r][c] & South))
					b->direction = 1;
			    else if (!(map[r][c] & East))
					b->direction = 2;
				else
					b->direction = -1;
				break;
			default: 	 						   // currently standing still
				// go north, east, west, south in that priority if needed
			   	if (!(map[r][c] & North) && b->position.vz < p.vz)
					b->direction = 0;
			    else if (!(map[r][c] & East) && b->position.vx < p.vx)
					b->direction = 2;
		   		else if (!(map[r][c] & West) && b->position.vx > p.vx)
					b->direction = 3;
				else if (!(map[r][c] & South) && b->position.vz > p.vz)
					b->direction = 1;
				else
					b->direction = -1;
			}
		}

	switch(b->direction)				 // perform movement;
		{
		case 0:
			b->position.vz += 32;
			break;
		case 1:
			b->position.vz -= 32;
			break;
		case 2:
			b->position.vx += 32;
			break;
		case 3:
			b->position.vx -= 32;
			break;
		}
	}


//*****************************************************************************
// draw a TMD, obj, in the scene. perform translation, t, a rotation about Y
// of ang, and add to ordering table, ot
void drawObject(GsDOBJ2 *obj, short ang, VECTOR t, GsOT *ot)
	{
	MATRIX coordMatrix;
	long adx, adz;	 // absolute difference between object and current position
	MATRIX transformMatrix = GsIDMATRIX;

	// forget drawing anything if map is being displayed
	if (displayMap)
		return;

	RotMatrixY(ang,&transformMatrix);
	TransMatrix(&transformMatrix,&t);

	obj->coord2->coord = transformMatrix;	// perform transformation on object
	obj->coord2->super = WORLD;				// move relative to world
 	obj->coord2->flg = 0;					// flag object as moved

	GsGetLs(obj->coord2, &coordMatrix);		// do local screen matrix (performs
	GsSetLsMatrix(&coordMatrix);			// perspective, etc)

	adx = fabs(transformMatrix.t[0] - goodguy.position.vx);
	adz = fabs(transformMatrix.t[2] - goodguy.position.vz);

	if (adx < 1024 && adz < 1024)
		obj->attribute |= GsDIV2;
	else if (adx < 2048 && adz < 2048)
		obj->attribute |= GsDIV1;

	// add object to ordering table
	GsSortObject4(obj, ot, 14 - ot->length, (u_long *)getScratchAddr(0));

	obj->attribute &= ~GsDIV3;				// reset sub division to none
	}


//*****************************************************************************
// draw map at an angle relative to a position within it
void drawMap(short angle, VECTOR pos)
	{
	long row,col;
	GsBOXF box;
	GsLINE line;
	long dz, dx;
	short xsin, xcos, xonesin, xonecos;
	short zsin, zcos, zonesin, zonecos;
	short x, y;
	int i;

	box.attribute = 0;
	box.r = 255; box.g = 0; box.b = 0;

	setRECT(&box, -3, -3, 6, 6);
	GsSortBoxFill(&box, &OT2[buffIdx], 0);

// this bit will draw the bad guys on the map
/*	box.r = 0; box.g = 0; box.b = 255;

	for(i = 0; i < numBadGuys; i++)
		if (badguy[i].health > 0)
			{
			dx = badguy[i].position.vx - pos.vx;
			dz = badguy[i].position.vz - pos.vz;
			x = (dx * rcos(angle) >> 18) + (dz * rsin(angle) >> 18);
			y = (dx * rsin(angle) >> 18) - (dz * rcos(angle) >> 18);

			setRECT(&box, x - 3, y - 3, 6, 6);
			GsSortBoxFill(&box, &OT2[buffIdx], 0);
			}
*/
	line.attribute = 0;
	line.r = 255; line.g = 255; line.b = 255;

	for(row = 0; row < mapRows; row++)
		for(col = 0; col < mapCols; col++)
			{
			if (visited[row][col] == 0)	   // don't draw areas unvisited
				continue;

			dx = (col << 4) - (pos.vx >> 6);
			dz = (row << 4) - (pos.vz >> 6);

			xsin = dx * rsin(angle) >> 12;
			xcos = dx * rcos(angle) >> 12;

			xonesin = (dx + 16) * rsin(angle) >> 12;
			xonecos = (dx + 16) * rcos(angle) >> 12;

			zsin = dz * rsin(angle) >> 12;
			zcos = dz * rcos(angle) >> 12;

			zonesin = (dz + 16) * rsin(angle) >> 12;
			zonecos = (dz + 16) * rcos(angle) >> 12;

			if (map[row][col] & North)
				{
				line.x0 = xcos + zonesin;		line.y0 = xsin - zonecos;
				line.x1 = xonecos + zonesin;	line.y1 = xonesin - zonecos;
				GsSortLine(&line, &OT2[buffIdx], 0);
				}
			if (map[row][col] & South)
				{
				line.x0 = xcos + zsin;			line.y0 = xsin - zcos;
				line.x1 = xonecos + zsin;		line.y1 = xonesin - zcos;
				GsSortLine(&line, &OT2[buffIdx], 0);
				}
			if (map[row][col] & East)
				{
				line.x0 = xonecos + zsin;		line.y0 = xonesin - zcos;
				line.x1 = xonecos + zonesin;	line.y1 = xonesin - zonecos;
				GsSortLine(&line, &OT2[buffIdx], 0);
				}
			if (map[row][col] & West)
				{
				line.x0 = xcos + zsin;			line.y0 = xsin - zcos;
				line.x1 = xcos + zonesin;		line.y1 = xsin - zonecos;
				GsSortLine(&line, &OT2[buffIdx], 0);
				}
			}
	}

//*****************************************************************************
void getExitCLUTRect(void)
	{
	GsIMAGE image;

	GsGetTimInfo(efloorTIM + 1,&image);
	setRECT(&exitFloorCLUTRect, image.cx, image.cy, image.cw, image.ch);

	GsGetTimInfo(ewallTIM + 1,&image);
	setRECT(&exitWallCLUTRect, image.cx, image.cy, image.cw, image.ch);

	GsGetTimInfo(eroofTIM + 1,&image);
	setRECT(&exitRoofCLUTRect, image.cx, image.cy, image.cw, image.ch);
	}

//*****************************************************************************
// grab CLUTs of the textures in the exit TMDs
// this allows them to be altered later
void grabExitCLUT(void)
	{
	StoreImage(&exitFloorCLUTRect,(u_long *)exitFloorCLUT);
	StoreImage(&exitWallCLUTRect,(u_long *)exitWallCLUT);
	StoreImage(&exitRoofCLUTRect,(u_long *)exitRoofCLUT);
	}

//*****************************************************************************
// alter the CLUT of the textures in the exit TMDs
// this allows a pulsing exit area
void alterExitCLUT(void)
	{
	int i;

	// decide whether to increase or decrease colour
	if (exitCLUTVal > 20)
		exitCLUTInc = -1;
	else if (exitCLUTVal < 0)
		exitCLUTInc = 1;

	exitCLUTVal += exitCLUTInc;

	// change CLUT values
	for(i=0;i<16;i++)
		{
		if (exitCLUTInc == 1 && (exitFloorCLUT[i] & 31) < 30)
			exitFloorCLUT[i] += 1;
		else if	(exitCLUTInc == -1 && (exitFloorCLUT[i] & 31) > 1)
			exitFloorCLUT[i] -= 1;

		if (exitCLUTInc == 1 && (exitWallCLUT[i] & 31) < 30)
			exitWallCLUT[i] += 1;
		else if	(exitCLUTInc == -1 && (exitWallCLUT[i] & 31) > 1)
			exitWallCLUT[i] -= 1;

		if (exitCLUTInc == 1 && (exitRoofCLUT[i] & 31) < 30)
			exitRoofCLUT[i] += 1;
		else if	(exitCLUTInc == -1 && (exitRoofCLUT[i] & 31) > 1)
			exitRoofCLUT[i] -= 1;
		}

	// replace current CLUT values with altered values
	LoadImage(&exitFloorCLUTRect,(u_long *)exitFloorCLUT);
	LoadImage(&exitWallCLUTRect,(u_long *)exitWallCLUT);
	LoadImage(&exitRoofCLUTRect,(u_long *)exitRoofCLUT);
	}


//*****************************************************************************
// see if first aid box picked up, if so, remove, inc health & make noise
void checkForHealthPickUp(void)
	{
	// ****** first aid *********
	if (map[goodguy.position.vz >> 10][goodguy.position.vx >> 10] & Health)
		{
		SsUtKeyOn(VAB, 4, 0, 60, 0, 127, 127);
		goodguy.health += 30;
		if (goodguy.health > 120)
			goodguy.health = 120;
		map[goodguy.position.vz >> 10][goodguy.position.vx >> 10] -= Health;
		}
	}


//*****************************************************************************
void poundingHeart(void)
	{
	int closest, volume, i;
	long dx, dz;

	for(i = 0; i < numBadGuys; i++)
		{
		dx = goodguy.position.vx - badguy[i].position.vx;
		dz = goodguy.position.vz - badguy[i].position.vz;

		closest = 127 - ((dx * dx + dz * dz) >> 17);

		if (closest < 0)
			closest = 0;

		if (closest > volume)
			volume = closest;
		}

	SsUtSetVVol(heartvoice, volume, volume);
	}

//*****************************************************************************
// draw a health bar using value h to determine colour and size
void drawHealthBox(int h)
	{
	GsBOXF box;

	box.attribute = 0;

	if (h > 100)
		{
		box.r = 0;
		box.g = 200;
		box.b = 0;
		}
	else
		{
		box.r = 255 - h * 255 / 100;
		box.g = h * 200 / 100;
		box.b = 0;
		}

	setRECT(&box, status.x + 120, status.y + 12, goodguy.health, 8);
	GsSortBoxFill(&box, &OT2[buffIdx], 0);
	}

//*****************************************************************************
void checkLevel(void)
	{
	if (map[goodguy.position.vz >> 10][goodguy.position.vx >> 10] & Exit)
		{
		levelvoice = SsUtKeyOn(VAB, 11, 0, 60, 0, 127, 127);
		playVsync = VSync(-1);
		playingMusic = 1;
		goodguy.level++;

		switch (goodguy.level)
			{
			case 1:
				initMap(map1Addr);
				break;
			case 2:
				initMap(map2Addr);
				break;
			case 3:
				initMap(map3Addr);
				break;
			default:
				goodguy.level = 1;
				initMap(map1Addr);
			}
		}

	if (playingMusic && VSync(-1) - playVsync > 300)
		{
		SsUtKeyOff(levelvoice, VAB, 11, 0, 60);
		playingMusic = 0;
		}

	}
//*****************************************************************************
// character controls
void viewControl(void)
	{
	VECTOR increment;
	static u_long oldPad;
	int i;

	if (PadStatus & PADLleft)							// turn left
		viewAngle = (viewAngle + 32) & 4095;

	if (PadStatus & PADLright)							// turn right
		viewAngle = (viewAngle - 32) & 4095;

	if (PadStatus & PADLup)								// forward
		{
		setVector(&increment, - rsin(viewAngle) >> 6, 0, rcos(viewAngle) >> 6);

		if (!collisionX(goodguy.position,increment))	// separate x and z 
			goodguy.position.vx += increment.vx;		// collision detection

		if (!collisionZ(goodguy.position,increment))
			goodguy.position.vz += increment.vz;
		}

	if (PadStatus & PADLdown)							// backward
		{
		setVector(&increment, rsin(viewAngle) >> 6, 0, - rcos(viewAngle) >> 6);

		if (!collisionX(goodguy.position,increment))
			goodguy.position.vx += increment.vx;

		if (!collisionZ(goodguy.position,increment))
			goodguy.position.vz += increment.vz;
		}

	if (PadStatus & PADR1)								// strafe right
		{
		setVector(&increment, rcos(viewAngle) >> 6, 0, rsin(viewAngle) >> 6);

		if (!collisionX(goodguy.position,increment))
			goodguy.position.vx += increment.vx;

		if (!collisionZ(goodguy.position,increment))
			goodguy.position.vz += increment.vz;
		}

	if (PadStatus & PADL1)								// strafe left
		{
		setVector(&increment, - rcos(viewAngle) >> 6, 0, - rsin(viewAngle) >> 6);

		if (!collisionX(goodguy.position,increment))
			goodguy.position.vx += increment.vx;

		if (!collisionZ(goodguy.position,increment))
			goodguy.position.vz += increment.vz;
		}

	if (PadStatus & (PADL1 | PADR1 | PADLup | PADLdown))
		{
		goodguy.position.vy += goodguy.bounce;
		if (goodguy.position.vy > goodguy.top || goodguy.position.vy < goodguy.bottom)
			goodguy.bounce = -goodguy.bounce;
		}

	if (PadStatus & PADRdown && !(oldPad & PADRdown))	// fire (no auto fire)
		for (i = 0; i < MaxShots; i++)
			if (goodguy.bullet[i].available)
				{
				goodguy.bullet[i].active = 1;
				goodguy.bullet[i].available = 0;
				goodguy.bullet[i].position.vx = goodguy.position.vx;
				goodguy.bullet[i].position.vz = goodguy.position.vz;
				goodguy.bullet[i].increment.vx = - rsin(viewAngle) >> 4;
				goodguy.bullet[i].increment.vz = rcos(viewAngle) >> 4;
				 SsUtKeyOn(VAB, 1, 0, 60, 0, 127, 127);
				break;
				}

	if (PadStatus & PADRright)							// map
		displayMap = 1;

	oldPad = PadStatus;
	}


//*****************************************************************************
// load up map data, this is initially a text file that is load into memory
// the location of which is passed into the procedure
void initMap(u_char *mp)
	{
	unsigned char tmap[MaxMapRows][MaxMapCols];	  // temp map for conversion
	short r, c;
	long x, z;

	for(r = 0; r < MaxMapRows; r++)		  // initialise visited map
		for(c = 0; c < MaxMapCols; c++)
			visited[r][c] = 0;

	mapRows = 0;			   // init number of rows, cols and bad guys in map		 
	mapCols = 0;					  
	numBadGuys = 0;

	while(*mp < '0' || *mp > '9')   		  // skip to start of numbers
		mp++;

	while(*mp >= '0' && *mp <= '9')			  // read number of rows
		mapRows = mapRows * 10 + *(mp++) - 48;

	while(*mp < '0' || *mp > '9')			  // skip to next numbers
		mp++;

	while(*mp >= '0' && *mp <= '9')	  		  // read number of columns
		mapCols = mapCols * 10 + *(mp++) - 48;

	while(*mp != 13 && *mp != 10)   		  // skip to next line in file
		mp++;				   				  // supports CR or LF or both

	while(*mp == 13 || *mp == 10)
		mp++;

	for(r = mapRows - 1; r >= 0; r--)  		  // load up map to a temp matrix
		{
		for(c = 0; c < mapCols; c++)
			tmap[r][c] = *(mp++);

		while(*mp != 13 && *mp != 10)   	  // next line
			mp++;

		while(*mp == 13 || *mp == 10)
			mp++;
		}

	for(r = mapRows - 1; r >= 0; r--)  	 // set up map matrix
		for(c = 0; c < mapCols; c++)
			{
			map[r][c] = 0;			   

			if (tmap[r][c] != 'X')	   	 // areas marked X are never visited
				{
				if (r == 0) 			 // put border around edges of map
					map[0][c] |= South;
				if (r == mapRows -1) 
					map[r][c] |= North;
				if (c == 0) 
					map[r][0] |= West;
				if (c == mapCols -1) 
					map[r][c] |= East;

				if (c < mapCols - 1 && tmap[r][c + 1] == 'X')  // walls
					map[r][c] |= East;
				if (c > 0 && tmap[r][c - 1] == 'X')
					map[r][c] |= West;
				if (r < mapRows - 1 && tmap[r + 1][c] == 'X')
					map[r][c] |= North;
				if (r > 0 && tmap[r - 1][c] == 'X')
					map[r][c] |= South;

				if (c < mapCols - 1 && tmap[r][c] == 'O')  // fake block
					map[r][c] |= Fake;
				if (c < mapCols - 1 && tmap[r][c + 1] == 'O')
					map[r][c] |= East;
				if (c > 0 && tmap[r][c - 1] == 'O')
					map[r][c] |= West;
				if (r < mapRows - 1 && tmap[r + 1][c] == 'O')
					map[r][c] |= North;
				if (r > 0 && tmap[r - 1][c] == 'O')
					map[r][c] |= South;

				x = (c << 10) + 512;   // middle of a map location in x,z terms
				z = (r << 10) + 512;

				if (tmap[r][c] == 'S')			  	   // setup start position
					initGoodGuy(&goodguy, x, z);

				if (tmap[r][c] == 'M')			  	   // setup bad guys
					initBadGuy(&badguy[numBadGuys++], x, z);

				if (tmap[r][c] == 'F')			  	   // setup health pickups
					map[r][c] |= Health;

				if (tmap[r][c] == 'E')			  	   // setup exit location
					map[r][c] |= Exit;
				}
			}

	}

//*****************************************************************************
// is x1,z1 visible from x2,z2 facing in the direction ang
// after the initial simple checks for direction and distance, the more
// difficult checking for walls takes place. this is done by imagining a line
// drawn from (x1,z1) to (x2,z2) and then checking if walls are present where
// the line intersects the squares
int inLineOfSight(long x1, long z1, long x2, long z2, short ang)
	{
	long dx, dz;
	long ax, az;
	long bx, bz;
	long xinc, zinc;
	long r, c;

	dx =  x1 - x2;	// difference between 'from' and 'to'
	dz =  z1 - z2;

	// too far away (initial ignore, because other check uses multiply)
	if (fabs(dx) > 3072 || fabs(dz) > 3072)
		return 0;

	// same square
	if ((x1 >> 10) == (x2 >> 10) && (z1 >> 10) == (z2 >> 10))
		return 1;

	// limit displayed area to 45 degrees each side of direction faced
	if (fabs(rinvtan(dz,-dx)- ang) > 512 && fabs(rinvtan(dz,-dx)- ang) < 3584)
		return 0;

	// too far away (9437184 = 3072 ^ 2, avoids square root)
	if (dx * dx + dz * dz > 9437184)
		return 0;

	r = z2 >> 10;  // 'from' point row and column
	c = x2 >> 10;

	if (fabs((z1 >> 10) - r) > fabs((x1 >> 10) - c))
		{
		az = dz < 0 ? - (z2 & 1023) : 1024 - (z2 & 1023);
		ax = x2 + az * dx / dz;
		xinc = 1024 * dx / fabs(dz);

		do
			{
			// check if gone into a new column, check for walls and update column number
			while (ax >> 10 > c)
				if (map[r][c++] & East)
					return 0;

			while (ax >> 10 < c)
				if (map[r][c--] & West)
					return 0;

			// check for walls going into new row
			if (dz > 0 && map[r][c] & North)
				return 0;

			else if (dz < 0 && map[r][c] & South)
				return 0;

			// update row number -1 if dz < 0, 1 otherwise
			r += dz < 0 ? -1 : 1;
			// update x information
			ax += xinc;
			}
		while ((dz < 0 && r > z1 >> 10) || (dz > 0 && r < z1 >> 10));
	
		if ((x1 >> 10) > c && (map[r][c] & East))
			return 0;
		else if ((x1 >> 10) < c && (map[r][c] & West))
			return 0;
		}
	else
		{
		ax = dx < 0 ? - (x2 & 1023) : 1024 - (x2 & 1023);
		az = z2 + ax * dz / dx;
		zinc = 1024 * dz / fabs(dx);

		do
			{
			while (az >> 10 > r)
				if (map[r++][c] & North)
					return 0;

			while (az >> 10 < r)
				if (map[r--][c] & South)
					return 0;

			if (dx > 0 && map[r][c] & East)
				return 0;

			else if (dx < 0 && map[r][c] & West)
				return 0;

			c += dx < 0 ? -1 : 1;
			az += zinc;
			}
		while ((dx < 0 && c > x1 >> 10) || (dx > 0 && c < x1 >> 10));

		if ((z1 >> 10) > r && (map[r][c] & North))
			return 0;
		else if ((z1 >> 10) < r && (map[r][c] & South))
			return 0;
		}

	return 1;  // must be visible
	}

//*****************************************************************************
void initGoodGuy(GoodGuy *g, u_long x, u_long z)
	{
	int i;

	setVector(&(g->position), x, -512, z);
	g->health = 100;
	g->kills = 0;
	g->top = -480;
	g->bottom = -544;
	g->bounce = -8;
	for(i=0;i<6;i++)
		initProjectile(&(g->bullet[i]),&bulletOBJ);
	}

//*****************************************************************************
void initBadGuy(BadGuy *b, u_long x, u_long z)
	{
	b->active = 0;
	b->activeDist = 5120;
	b->direction = -1;
	b->health = 5;
	b->hitsize = 500;
	b->facing = 0;
	b->pvsync = 60;
	setVector(&(b->position), x, 0, z);
	initProjectile(&(b->bullet),&bullet2OBJ);
	b->object = &monster1OBJ;
	}

//*****************************************************************************
void initProjectile(projectile *p, GsDOBJ2 *o)
	{
	p->active = 0;
	p->available = 1;
	p->hit = 0;
	p->vsync = 0;
	setVector(&(p->position),0,0,0);
	setVector(&(p->increment),0,0,0);
	p->object = o;
	}


//*****************************************************************************
void setupTexturesAndTMDs(void)
	{
	TMD_NORM nm;
	TMD_VERT v0,v1,v2,v3;

	InitialiseTexture(roofTIM);
	InitialiseTexture(wallTIM);
	InitialiseTexture(floorTIM);
	InitialiseTexture(man1TIM);
	InitialiseTexture(man2TIM);
	InitialiseTexture(deadmanTIM);
	InitialiseTexture(bulletTIM);
	InitialiseTexture(bloodTIM);
	InitialiseTexture(ricochetTIM);
	InitialiseTexture(healthTIM);
	InitialiseTexture(bullet2TIM);
	InitialiseTexture(eroofTIM);
	InitialiseTexture(ewallTIM);
	InitialiseTexture(efloorTIM);
	InitialiseTexture(fwallTIM);
	initialiseSprite(statusTIM,&status);
	status.x = - status.w >> 1;
	if (GetVideoMode() == MODE_NTSC)
		status.y = 88;
	else
		status.y = 96;
	initialiseSprite(faceTIM,&face);
	face.x = status.x + 16;
	face.y = status.y + 4;

	// make a floor polygon
	setNormal(&nm, 0,  ONE, 0);
	setVector(&v0, -512, 0, -512); 		setVector(&v1, -512, 0, 512); 
	setVector(&v2,  512, 0, -512); 		setVector(&v3,  512, 0, 512);

	makeSplitTMD_G_4T(floorTMD, floorTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(floorTMD, 0, &floorOBJ);

	makeSplitTMD_G_4T(efloorTMD, efloorTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(efloorTMD, 0, &efloorOBJ);

	// make a roof polygon
	setNormal(&nm, 0,  -ONE,    0);
	setVector(&v0, -512, 0, -512); 	setVector(&v1, 512, 0, -512); 
	setVector(&v2, -512, 0,  512); 	setVector(&v3, 512, 0,  512);

	makeSplitTMD_G_4T(roofTMD, roofTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(roofTMD, 0, &roofOBJ);

	makeSplitTMD_G_4T(eroofTMD, eroofTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(eroofTMD, 0, &eroofOBJ);

	// make the wall
	setNormal(&nm, 0,     0, -ONE);
	setVector(&v0, -512, -512, 0); 	setVector(&v1, 512, -512, 0); 
	setVector(&v2, -512,  512, 0); 	setVector(&v3, 512,  512, 0);

	makeSplitTMD_G_4T(wallTMD, wallTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(wallTMD, 0, &wallOBJ);

	makeSplitTMD_G_4T(ewallTMD, ewallTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(ewallTMD, 0, &ewallOBJ);

	makeSplitTMD_G_4T(fwallTMD, fwallTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(fwallTMD, 0, &fwallOBJ);

	// make the bad guy, the polygon is the same for both frames
	setNormal(&nm, 0, 0, -ONE);			
	setVector(&v0, -384, -768, 0);		setVector(&v1,  384, -768, 0); 
	setVector(&v2, -384,    0, 0); 		setVector(&v3,  384,    0, 0);

	makeTMD_G_4T(monster1TMD, man1TIM, 0, 0, 127, 0, 0, 127, 127, 127, nm, v0, v1, v2, v3, -1);
	initialiseTMD(monster1TMD, 0, &monster1OBJ);

	makeTMD_G_4T(monster2TMD, man2TIM, 0, 0, 127, 0, 0, 127, 127, 127, nm, v0, v1, v2, v3, -1);
	initialiseTMD(monster2TMD, 0, &monster2OBJ);

	// make the polygon for the dead bad guy
	setNormal(&nm, 0, ONE, 0);
	setVector(&v0, -384, -50, -384);	setVector(&v1, -384, -50, 384); 
	setVector(&v2,  384, -50, -384); 	setVector(&v3,  384, -50, 384);

	makeTMD_G_4T(deadTMD, deadmanTIM, 0, 0, 127, 0, 0, 127, 127, 127, nm, v0, v1, v2, v3, -1);
	initialiseTMD(deadTMD, 0, &deadOBJ);

	// make the polygon for the bullet, blood, and ricochet
	// same size poly but different textures
	setNormal(&nm, 0, 0, -ONE);
	setVector(&v0, -48, -432, 0);		setVector(&v1,  48, -432, 0); 
	setVector(&v2, -48, -336, 0); 		setVector(&v3,  48, -336, 0);

	makeTMD_G_4T(bulletTMD, bulletTIM, 0, 0, 15, 0, 0, 15, 15, 15, nm, v0, v1, v2, v3, 1);
	initialiseTMD(bulletTMD, 0, &bulletOBJ);

	makeTMD_G_4T(bullet2TMD, bullet2TIM, 0, 0, 15, 0, 0, 15, 15, 15, nm, v0, v1, v2, v3, 1);
	initialiseTMD(bullet2TMD, 0, &bullet2OBJ);

	makeTMD_G_4T(bloodTMD, bloodTIM, 0, 0, 15, 0, 0, 15, 15, 15, nm, v0, v1, v2, v3, -1);
	initialiseTMD(bloodTMD, 0, &bloodOBJ);

	makeTMD_G_4T(ricochetTMD, ricochetTIM, 0, 0, 15, 0, 0, 15, 15, 15, nm, v0, v1, v2, v3, 1);
	initialiseTMD(ricochetTMD, 0, &ricochetOBJ);

	// make poly for first aid box
	setNormal(&nm, 0, 0, -ONE);
	setVector(&v0, -192, -384, 0);		setVector(&v1,  192, -384, 0); 
	setVector(&v2, -192,    0, 0); 		setVector(&v3,  192,    0, 0);

	makeTMD_G_4T(healthTMD, healthTIM, 0, 0, 63, 0, 0, 63, 63, 63, nm, v0, v1, v2, v3, -1);
	initialiseTMD(healthTMD, 0, &healthOBJ);
	}

//****************************************************************************
// change animation frame for bad guys
void animateBadGuys(void)
	{
	static int frameVSync;
	int i;

	if (VSync(-1) - frameVSync > 10)		// if 10 vsyncs past
		for(i = 0; i < numBadGuys; i++)
			if (badguy[i].active)			// if bad guy active
				{
				if (badguy[i].object == &monster1OBJ)
					badguy[i].object = &monster2OBJ;
				else
					badguy[i].object = &monster1OBJ;

				frameVSync = VSync(-1);
				}
	}

//*****************************************************************************
// draw floor, roof, walls and any first aid boxes
void drawWalls(void)
	{
	char r, c;
	long z, x;
	MATRIX transformMatrix;
	SVECTOR rotate;
	VECTOR translate;

	for (r = 0; r < mapRows ; r++)
		for (c = 0; c < mapCols ; c++)
			{
			x = c << 10;	// turn row and column into x and z
			z = r << 10;

			if (fabs((goodguy.position.vx >> 10) - c) > 3 || fabs((goodguy.position.vz >> 10) - r) > 3)
				continue;

			// this big if decides whether a square is visible,
			// this is done by checking 8 points on the square
			if (inLineOfSight(x + 1,z + 16, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 1,z + 1008, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 16,z + 1, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 16,z + 1023, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 1008,z + 1, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 1008,z + 1023, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 1023,z + 16, goodguy.position.vx, goodguy.position.vz, viewAngle) ||
		   	inLineOfSight(x + 1023,z + 1008, goodguy.position.vx, goodguy.position.vz, viewAngle))
				{
				visited[r][c] = 1;

				if (map[r][c] & North)  // north wall
					{
					setVector(&translate, x + 512, -512, z + 1024);

					if (map[r][c] & Exit)
						drawObject(&ewallOBJ,0,translate,&OT2[buffIdx]);
					else if (r < MaxMapRows -1 && (map[r + 1][c] & Fake))
						drawObject(&fwallOBJ,0,translate,&OT2[buffIdx]);
					else
						drawObject(&wallOBJ,0,translate,&OT2[buffIdx]);
					}

				if (map[r][c] & South)  // south wall
					{
					setVector(&translate, x + 512, -512, z);

					if (map[r][c] & Exit)
						drawObject(&ewallOBJ,2048,translate,&OT2[buffIdx]);
					else if (r > 0 && (map[r - 1][c] & Fake))
						drawObject(&fwallOBJ,2048,translate,&OT2[buffIdx]);
					else
						drawObject(&wallOBJ,2048,translate,&OT2[buffIdx]);
					}

				if (map[r][c] & East)  // east wall
					{
					setVector(&translate, x + 1024, -512, z + 512);

					if (map[r][c] & Exit)
						drawObject(&ewallOBJ,1024,translate,&OT2[buffIdx]);
					else if (c < MaxMapCols -1 && (map[r][c + 1] & Fake))
						drawObject(&fwallOBJ,1024,translate,&OT2[buffIdx]);
					else
						drawObject(&wallOBJ,1024,translate,&OT2[buffIdx]);
					}

				if (map[r][c] & West)  // west wall
					{
					setVector(&translate, x, -512, z + 512);

					if (map[r][c] & Exit)
						drawObject(&ewallOBJ,3072,translate,&OT2[buffIdx]);
					else if (c > 0 && (map[r][c - 1] & Fake))
						drawObject(&fwallOBJ,3072,translate,&OT2[buffIdx]);
					else
						drawObject(&wallOBJ,3072,translate,&OT2[buffIdx]);
					}

				setVector(&translate, x + 512, 0, z + 512);

				if (map[r][c] & Exit)
					drawObject(&efloorOBJ,0,translate,&OT1[buffIdx]);
				else
					drawObject(&floorOBJ,0,translate,&OT1[buffIdx]);  // floor

				setVector(&translate, x + 512, -1024, z + 512);

				if (map[r][c] & Exit)
					drawObject(&eroofOBJ,0,translate,&OT1[buffIdx]);
				else
					drawObject(&roofOBJ,0,translate,&OT1[buffIdx]);  // roof

				if (map[r][c] & Health)
					{
					setVector(&translate, x + 512, 0, z + 512);
					drawObject(&healthOBJ,-viewAngle,translate,&OT2[buffIdx]);
					}
				}
			}
	}


