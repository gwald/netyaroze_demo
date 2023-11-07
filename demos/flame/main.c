/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 8th May 97                    *
*                                                              *
***************************************************************/

#include <libps.h>
#include <rand.h>
#include "pad.h"
#include "tmd.h"

	// GPU packet space
#define PACKETMAX		2500
#define PACKETMAX2		(PACKETMAX*24)

#define lightTMD    (u_long *)0x800a0000

#define lengthOT  8

#define setVector(v,x,y,z) (v)->vx = (x), (v)->vy = (y), (v)->vz = (z)
#define setNormal(n,x,y,z) (n)->nx = (x), (n)->ny = (y), (n)->nz = (z)
#define setColor(c,_r,_g,_b) (c)->r = (_r), (c)->g = (_g), (c)->b = (_b)
#define copyVector(v0,v1) (v1)->vx = (v0)->vx, (v1)->vy = (v0)->vy, (v1)->vz = (v0)->vz
#define addVector(v0,v1) (v0)->vx += (v1)->vx, (v0)->vy += (v1)->vy, (v0)->vz += (v1)->vz
#define subVector(v0,v1) (v0)->vx -= (v1)->vx, (v0)->vy -= (v1)->vy, (v0)->vz -= (v1)->vz
#define applyVector(v,x,y,z,op)	(v)->vx op x, (v)->vy op y, (v)->vz op z
#define dotProduct(v1,v2) ((v1)->vx * (v2)->vx / 4096 + (v1)->vy * (v2)->vy / 4096 + (v1)->vz * (v2)->vz / 4096)

#define mapx 40
#define mapy 34

u_char fracmap[mapx+1][mapy+1];

// ******************************* GLOBALS ************************************

// ordering tables
GsOT		OT[2];
GsOT_TAG	zTable[2][1 << lengthOT];

// GPU packet work area
PACKET GpuOutputPacket[2][PACKETMAX2];

// current output buffer
int	buffIdx;

// viewing handle and viewing transform
GsVIEW2 mainView;
MATRIX mainViewMatrix;

// font print id
long fntPrintID;

// pad status
u_long PadStatus;

// the TMD object handles
GsDOBJ2 lightOBJ;

SVECTOR rotateView = {0,0,0};

// A list of the colours to be used on the TMD, BGR order
u_long colorMap[256] ={
	0x000000,0x000010,0x000020,0x000030,0x000040,0x000050,0x000060,0x000070,
	0x000080,0x000090,0x0000a0,0x0000b0,0x0000c0,0x0000d0,0x0000e0,0x0000f0,
	0x0000ff,0x0010ff,0x0020ff,0x0030ff,0x0040ff,0x0050ff,0x0060ff,0x0070ff,
	0x0080ff,0x0090ff,0x00a0ff,0x00b0ff,0x00c0ff,0x00d0ff,0x00e0ff,0x00f0ff,
	0x00ffff,0x10ffff,0x20ffff,0x30ffff,0x40ffff,0x50ffff,0x60ffff,0x70ffff,
	0x80ffff,0x90ffff,0xa0ffff,0xb0ffff,0xc0ffff,0xd0ffff,0xe0ffff,0xf0ffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,
	0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff,0xffffff
};


//*****************************************************************************
// tie a handler to a TMD
void initialiseTMD(u_long *tmdAddr, char objNum, GsDOBJ2 *obj)
{
	GsMapModelingData((u_long *)(tmdAddr + 1));

	GsLinkObject4((u_long)(tmdAddr + 3), obj, objNum);
	obj->coord2 = WORLD;
}

//*****************************************************************************
// draw a TMD
void drawObject(GsDOBJ2 *obj, MATRIX tmat, GsCOORDINATE2 *super, GsOT *ot)
	{
	MATRIX coordMatrix;

	obj->coord2->coord = tmat;				// perform transformation on object
	obj->coord2->super = super; 			// move relative to coord system super
 	obj->coord2->flg = 0;					// flag object as moved

	GsGetLs(obj->coord2, &coordMatrix);		// do local screen matrix (performs
	GsSetLsMatrix(&coordMatrix);			// perspective, etc)

	// add object to ordering table
	GsSortObject4(obj, ot, 14 - ot->length, (u_long *)getScratchAddr(0));
	}

//*****************************************************************************
// create the fire effect
void fireGrid()
	{
	int i,j;
	int r0,r1,r2;	 // rows of filter

	// hot spots added to bottom
	for (i = 0; i < 10; i ++)				     
		fracmap[rand() % (mapx - 1) + 1][mapy] = rand();

	// filter points
	for (i = 1; i < mapx; i++)						
		{
		r0 = fracmap[i-1][0] + fracmap[i][0] + fracmap[i+1][0];
		r1 = fracmap[i-1][1] + fracmap[i][1] + fracmap[i+1][1];
		r2 = fracmap[i-1][2] + fracmap[i][2] + fracmap[i+1][2];

		for(j = 1; j < mapy; j++)
			{
			fracmap[i][j] = (r0 + r1 + r2) / 9;

			r0 = r1;
			r1 = r2;
			r2 = fracmap[i-1][j+2] + fracmap[i][j+2] + fracmap[i+1][j+2];
			}
		}

	// scroll up
	for (i = 1; i < mapx; i++)
		for(j = 1; j < mapy; j++)
			fracmap[i][j] = fracmap[i][j+1];

	// clear bottom line
	for(i = 0; i <= mapx; i++)
		fracmap[i][mapy] = 0;
	}

//*****************************************************************************
void controls()
	{
	VECTOR translate;
	SVECTOR rotate;
	MATRIX t1,tmp;

	if (PadStatus & PADLleft)
		{
		setVector(&rotate,0,64,0);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADLright)
		{
		setVector(&rotate,0,-64,0);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADLup)
		{
		setVector(&rotate,-64,0,0);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADLdown)
		{
		setVector(&rotate,64,0,0);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADL1)
		{
		setVector(&rotate,0,0,64);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADL2)
		{
		setVector(&rotate,0,0,-64);
		RotMatrix(&rotate,&t1);
		tmp = mainViewMatrix;
		MulMatrix0(&t1,&tmp,&mainViewMatrix);
		}

	if (PadStatus & PADR1)
		{
		setVector(&translate, mainViewMatrix.t[0],	mainViewMatrix.t[1], mainViewMatrix.t[2] - 64);
		TransMatrix(&mainViewMatrix,&translate);
		}

	if (PadStatus & PADR2)
		{
		setVector(&translate, mainViewMatrix.t[0],	mainViewMatrix.t[1], mainViewMatrix.t[2] + 64);
		TransMatrix(&mainViewMatrix,&translate);
		}
	}

//*****************************************************************************
// make flat TMD gridw by gridh in size using gouraud quads with no lighting
void makeBigTMDGouraudQuad(u_long *addr)
	{
	TMD_OBJ *obj_table;	// pointer to object table
	u_long *vert_table;	// pointer to vertices table
	u_long *norm_table;	// pointer to normals table
	u_long *prim_table;	// pointer to primative table
	int i,j;
	TMD_VERT v;
	int vcnt;

	// ******** TMD header info ********
	setTMD_HDR((TMD_HDR *)addr, 0x00000041, 1, 1);		
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // move to end of header

	// ******** skip object table, values setup later ********
	obj_table = (TMD_OBJ *)addr;
	addr = (u_long *)((TMD_OBJ *)addr + 1);

	// ******** primative table ********
	prim_table = addr;

	for(j = 0; j < mapy - 2; j++)
		for(i = 0; i < mapx; i++)
			{
			vcnt = (j * (mapx + 1) + i);

			// primative info, specific to type of primative
			setPRIM_HDR((PRIM_HDR *)addr, G_4_NL, 1, 6, 8);
			addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

			// primative data
			setTMD_G_4_NL((TMD_G_4_NL *)addr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, vcnt, vcnt + 1, vcnt + (mapx + 1), vcnt + (mapx + 1) + 1);
			addr = (u_long *)((TMD_G_4_NL *)addr + 1);  // move to end of primative data
			}

	// ******** vertices table ********
	vert_table = addr;

	for(j = 0; j <= mapy - 2; j++)
		for(i = 0; i <= mapx; i++)
			{
			// make a flat grid
			setVector(&v, (i - mapx / 2) << 4, (j - mapy / 2) << 4, 0);

			// vertices and normal data
			setTMD_VERT((TMD_VERT *)addr, v.vx, v.vy, v.vz);
			addr = (u_long *)((TMD_VERT *)addr + 1);
			}

	// ******** normals table ********
	norm_table = addr;

	// fill in object table details
	setTMD_OBJ(obj_table, vert_table, (mapx + 1)*(mapy-1), norm_table, 0, prim_table, mapx*(mapy-2), 0);
	}


//*****************************************************************************
// change the colours of the primatives within the TMD
void alterColors(u_long *addr)
	{
	int i,j;
	u_long col0, col1, col2, col3;	  // colours for the 4 vertices

	for(j = 0; j < mapy - 2; j++)
		for(i = 0; i < mapx; i++)
			{
			col0 = colorMap[fracmap[i][j]];
			col1 = colorMap[fracmap[i+1][j]];
			col2 = colorMap[fracmap[i][j+1]];
			col3 = colorMap[fracmap[i+1][j+1]];

			addr = (u_long *)((PRIM_HDR *)addr + 1);  // skip prim header

			col0 |= 0x39000000;   // necessary mode info replaced in col0
			*addr++ = col0;
			*addr++ = col1;
			*addr++ = col2;
			*addr = col3;

			addr += 3;
			}
	}


int main(void)
{
	int i;
	SVECTOR rotate;
	VECTOR translate;
	long hsync;
	u_long *addr;

	srand(54634);	// random number seed

	ResetGraph(0);	// reset the graphics

	PadStatus = 0;	// set up controller pad
	PadInit(); 	

	GsInitGraph(640 ,480, GsOFSGPU, 1, 0);	// screen 640x480
									
	GsDefDispBuff(0, 0, 0, 0);   // display buffer
	GsInit3D();					   // 3D

	FntLoad(960, 256);					 // fonts
	fntPrintID = FntOpen(-280, -200, 128, 16, 0, 32); 

	for (i = 0; i < 2; i++)				 // setup ordering tables
		{
		OT[i].length = lengthOT;
		OT[i].org = zTable[i];
		}

	GsSetProjection(1000);			   // perpsective

	makeBigTMDGouraudQuad(lightTMD);
	initialiseTMD(lightTMD,0,&lightOBJ);

	mainViewMatrix = GsIDMATRIX;
	setVector(&translate, 0, 0, 1000);
	TransMatrix(&mainViewMatrix, &translate);

	// move to start of primative data
	addr = lightTMD;
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // skip TMD header
	addr = (u_long *)((TMD_OBJ *)addr + 1);	 // skip object table

	do
		{
		buffIdx = GsGetActiveBuff();

		GsSetWorkBase((PACKET*)GpuOutputPacket[buffIdx]);

		GsClearOt(0, 0, &OT[buffIdx]);

		mainView.view = mainViewMatrix;
		mainView.super = WORLD;

		GsSetView2(&mainView);

		fireGrid();			 // update intensity of points in TMD

		alterColors(addr);	 // alter colours in primative data

 		drawObject(&lightOBJ, GsIDMATRIX, WORLD, &OT[buffIdx]); //  draw TMD

		DrawSync(0);	   // finish any drawing
		hsync = VSync(1);
		VSync(0);
		
		GsSwapDispBuff();  // swap screens

		GsSortClear(0, 0, 0, &OT[buffIdx]);  // add cls to ordering table

 		GsDrawOt(&OT[buffIdx]); 	// draw 

		PadStatus = PadRead();		// read the pad

		controls();

		FntPrint(fntPrintID,"%d\n",hsync);
		FntFlush(-1);

	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

	ResetGraph(1);		// cleanup
	return 0;
}


