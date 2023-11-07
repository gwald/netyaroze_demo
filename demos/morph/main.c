/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 8th May 97                    *
*                                                              *
***************************************************************/

#include <libps.h>
#include "pad.h"
#include "tmd.h"
#include "cylinder.h"
#include "sphere.h"
#include "flat.h"
#include "torus.h"

	// GPU packet space
#define PACKETMAX		2500
#define PACKETMAX2		(PACKETMAX*24)

#define pictureTIM  (u_long *)0x80090000
#define pictureTMD  (u_long *)0x800c0000

#define lengthOT  12

#define setVector(v,x,y,z) (v)->vx = (x), (v)->vy = (y), (v)->vz = (z)
#define setNormal(n,x,y,z) (n)->nx = (x), (n)->ny = (y), (n)->nz = (z)
#define setColor(c,_r,_g,_b) (c)->r = (_r), (c)->g = (_g), (c)->b = (_b)
#define copyVector(v0,v1) (v1)->vx = (v0)->vx, (v1)->vy = (v0)->vy, (v1)->vz = (v0)->vz
#define addVector(v0,v1) (v0)->vx += (v1)->vx, (v0)->vy += (v1)->vy, (v0)->vz += (v1)->vz
#define subVector(v0,v1) (v0)->vx -= (v1)->vx, (v0)->vy -= (v1)->vy, (v0)->vz -= (v1)->vz
#define applyVector(v,x,y,z,op)	(v)->vx op x, (v)->vy op y, (v)->vz op z
#define dotProduct(v1,v2) ((v1)->vx * (v2)->vx / 4096 + (v1)->vy * (v2)->vy / 4096 + (v1)->vz * (v2)->vz / 4096)

#define gridw 20
#define gridh 16

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
GsDOBJ2 pictureOBJ;



//*****************************************************************************
// tie a handler to a TMD
void initialiseTMD(u_long *tmdAddr, char objNum, GsDOBJ2 *obj)
{
	GsMapModelingData((u_long *)(tmdAddr + 1));

	GsLinkObject4((u_long)(tmdAddr + 3), obj, objNum);
	obj->coord2 = WORLD;
}

//*****************************************************************************
// initialise texture and clut from TIM
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
void setupTexturesAndTMDs(void)
	{
	InitialiseTexture(pictureTIM);
	}


//*****************************************************************************
// draw a TMD
void drawObject(GsDOBJ2 *obj, MATRIX transformMatrix, GsCOORDINATE2 *super, GsOT *ot)
	{
	MATRIX coordMatrix;

	obj->coord2->coord = transformMatrix;	// perform transformation on object
	obj->coord2->super = super; 			// move relative to coord system super
 	obj->coord2->flg = 0;					// flag object as moved

	GsGetLs(obj->coord2, &coordMatrix);		// do local screen matrix (performs
	GsSetLsMatrix(&coordMatrix);			// perspective, etc)

	// add object to ordering table
	GsSortObject4(obj, ot, 14 - ot->length, (u_long *)getScratchAddr(0));
	}

//*****************************************************************************
// make a TMD of textured quads without light sourcing, using a TIM as the
// texture (assumes TIM already loaded into VRAM)
void makeBigTMDTexQuad(u_long *addr, u_long *timAddr)
	{
	TMD_OBJ *obj_table;	// pointer to object table
	u_long *vert_table;	// pointer to vertices table
	u_long *norm_table;	// pointer to normals table
	u_long *prim_table;	// pointer to primative table
	int i,j;
	TMD_VERT v;
	u_char u0,v0,u1,v1,u2,v2,u3,v3;	 // texture coordinates
	int vcnt;
	int p;
	u_short CBA,TSB,pmode,pixScale;	 // texture info as used for TMD
	GsIMAGE image;					 // texture info

	GsGetTimInfo(++timAddr,&image);

	pmode = (image.pmode & 3);
	CBA = (image.cy << 6) + (image.cx >> 4);
	TSB = (image.px >> 6) + (image.py >> 10) + (pmode << 7);

	if (pmode == 0)		  // 4 bit has 4 texels to 1 pixel
		pixScale = 4;
	else if (pmode == 1)  // 8 bit has 2 texels to 1 pixel
		pixScale = 2;
	else 				  // 16 bit has a 1 to 1 mapping
		pixScale = 1;

	// ******** TMD header info ********
	setTMD_HDR((TMD_HDR *)addr, 0x00000041, 1, 1);		
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // move to end of header

	// ******** skip object table, values setup later ********
	obj_table = (TMD_OBJ *)addr;
	addr = (u_long *)((TMD_OBJ *)addr + 1);

	// ******** primative table ********
	prim_table = addr;

	for(j = 0; j < gridh; j++)
		for(i = 0; i < gridw; i++)
			{
			// set up texture coordinates
			u0 = i * (image.pw * pixScale) / gridw;           	v0 = j * image.ph / gridh;
			u1 = (i + 1) * (image.pw * pixScale) / gridw - 1;	v1 = j * image.ph / gridh;
			u2 = i * (image.pw * pixScale) / gridw;           	v2 = (j + 1) * image.ph / gridh - 1;
			u3 = (i + 1) * (image.pw * pixScale) / gridw - 1; 	v3 = (j + 1) * image.ph / gridh - 1;

			vcnt = (j * (gridw + 1) + i);	  // calculate which vertices to point to

			// primative info, specific to type of primative
			setPRIM_HDR((PRIM_HDR *)addr, F_4T_NL, 1, 6, 7);
			addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

			// primative data
			setTMD_F_4T_NL((TMD_F_4T_NL *)addr, CBA, TSB, u0, v0, u1, v1, u2, v2, u3, v3, 127, 127, 127, vcnt, vcnt+1, vcnt+(gridw+1), vcnt+(gridw+1)+1);
			addr = (u_long *)((TMD_F_4T_NL *)addr + 1);  // move to end of primative data

			// second side

			// primative info, specific to type of primative
			setPRIM_HDR((PRIM_HDR *)addr, F_4T_NL, 1, 6, 7);
			addr = (u_long *)((PRIM_HDR *)addr + 1);  // move to end of primative header

			// primative data
			setTMD_F_4T_NL((TMD_F_4T_NL *)addr, CBA, TSB, u1, v1, u0, v0, u3, v3, u2, v2, 127, 127, 127, vcnt+1, vcnt, vcnt+(gridw+1)+1, vcnt+(gridw+1));
			addr = (u_long *)((TMD_F_4T_NL *)addr + 1);  // move to end of primative data
			}

	// ******** vertices table ********
	vert_table = addr;

	p = 0;
	for(j = 0; j <= gridh; j++)
		for(i = 0; i <= gridw; i++)
			{
			// use flat mesh to start TMD
			setVector(&v, flat[p], flat[p + 1], flat[p + 2]);

			// vertices and normal data
			setTMD_VERT((TMD_VERT *)addr, v.vx, v.vy, v.vz);
			addr = (u_long *)((TMD_VERT *)addr + 1);

			p += 3;
			}

	// ******** normals table ********
	norm_table = addr;

	// fill in object table details
	setTMD_OBJ(obj_table, vert_table, (gridw + 1) * (gridh + 1), norm_table, 0, prim_table, 2 * gridw * gridh, 0);
	}


//*****************************************************************************
// change current vertices at 'pvert' to those given in an array 'shape'
void changeTo(TMD_VERT *pvert, short *shape)
	{
	int i,j;
	TMD_VERT v;

	for(j = 0; j <= gridh; j++)
		for(i = 0; i <= gridw; i++)
			{
			v = *pvert;				   // get next vertex

			if (v.vx < *shape)		   // move x component closer to 'shape'
				v.vx++;
			else if (v.vx > *shape)
				v.vx--;
			
			shape++;

			if (v.vy < *shape)		   // move y component closer to 'shape'
				v.vy++;
			else if (v.vy > *shape)
				v.vy--;

			shape++;

			if (v.vz < *shape)		   // move z component closer to 'shape'
				v.vz++;
			else if (v.vz > *shape)
				v.vz--;

			shape++;

			*pvert++ = v;			   // store altered vertex
			}
	}


int main(void)
{
	SVECTOR rotate;
	VECTOR translate;
	long hsync;
	u_long *addr;
	TMD_VERT *pvert;
	int i;
	int angle = 0;	// amount of rotation
	int state = 0;	// morphing decision 

	ResetGraph(0);	// reset the graphics

	GsInitGraph(640 ,480, GsOFSGPU, 1, 0);	// screen 640x480
									
	GsDefDispBuff(0, 0, 0, 0);   // display buffer
	GsInit3D();					   // 3D

	mainViewMatrix = GsIDMATRIX;

	for (i = 0; i < 2; i++)				 // setup ordering tables
		{
		OT[i].length = lengthOT;
		OT[i].org = zTable[i];
		}

	GsSetProjection(1000);			   // perpsective

	FntLoad(960, 256);					 // fonts
	fntPrintID = FntOpen(-280, -200, 128, 16, 0, 32); 

	PadStatus = 0;	// set up controller pad
	PadInit(); 	

	setupTexturesAndTMDs();

	makeBigTMDTexQuad(pictureTMD, pictureTIM);	   // make and initialise TMD
	initialiseTMD(pictureTMD,0,&pictureOBJ);

	addr = pictureTMD;
	addr = (u_long *)((TMD_HDR *)addr + 1);	  // skip TMD header

	pvert = (TMD_VERT *)(((TMD_OBJ *)addr)->vert_top);     // vertex pointer

	do
		{
		buffIdx = GsGetActiveBuff();		// current buffer

		GsSetWorkBase((PACKET*)GpuOutputPacket[buffIdx]);  // work area

		GsClearOt(0, 0, &OT[buffIdx]);		  // clear ordering table

		setVector(&rotate,0,angle,angle/2);		 // set up display transformation
		setVector(&translate,0,0,2000);
		RotMatrix(&rotate,&mainViewMatrix);
		TransMatrix(&mainViewMatrix,&translate);

		mainView.view = mainViewMatrix;		  // set view
		mainView.super = WORLD;

		GsSetView2(&mainView);

		if (state < 500)					  // manipulate vertices
			changeTo(pvert,cylinder);
		else if (state < 1000)
			changeTo(pvert,torus);
		else if (state < 1500)
			changeTo(pvert,sphere);
		else if (state < 2000)
			changeTo(pvert,flat);
		else if (state > 2500)
			state = 0;

 		drawObject(&pictureOBJ, GsIDMATRIX, WORLD, &OT[buffIdx]);  // draw TMD

		DrawSync(0);	   // finish any drawing
		hsync = VSync(1);
		VSync(0);
		
		GsSwapDispBuff();  // swap screens

		GsSortClear(0, 0, 0, &OT[buffIdx]);  // add cls to ordering table

 		GsDrawOt(&OT[buffIdx]); 	// draw 

		FntPrint(fntPrintID,"hsync %d\n",hsync);
		FntFlush(-1);

		state++;

		angle += 32;

		PadStatus = PadRead();		// read the pad

	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

	ResetGraph(1);		// cleanup
	return 0;
}


