/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 24th Apr 97                   *
*                                                              *
***************************************************************/

#include <libps.h>
#include "pad.h"
#include "tmd.h"

	// GPU packet space
#define PACKETMAX		2500
#define PACKETMAX2		(PACKETMAX*24)

#define textureTIM  (u_long *)0x80090000
#define torusTMD    (u_long *)0x800a0000

#define lengthOT  12

#define setVector(v,x,y,z) (v)->vx = (x), (v)->vy = (y), (v)->vz = (z)
#define setNormal(n,x,y,z) (n)->nx = (x), (n)->ny = (y), (n)->nz = (z)
#define setColor(c,_r,_g,_b) (c)->r = (_r), (c)->g = (_g), (c)->b = (_b)
#define copyVector(v0,v1) (v1)->vx = (v0)->vx, (v1)->vy = (v0)->vy, (v1)->vz = (v0)->vz
#define addVector(v0,v1) (v0)->vx += (v1)->vx, (v0)->vy += (v1)->vy, (v0)->vz += (v1)->vz
#define subVector(v0,v1) (v0)->vx -= (v1)->vx, (v0)->vy -= (v1)->vy, (v0)->vz -= (v1)->vz
#define applyVector(v,x,y,z,op)	(v)->vx op x, (v)->vy op y, (v)->vz op z
#define dotProduct(v1,v2) ((v1)->vx * (v2)->vx / 4096 + (v1)->vy * (v2)->vy / 4096 + (v1)->vz * (v2)->vz / 4096)

// ordering tables
GsOT		OT[2];
GsOT_TAG	zTable[2][1 << lengthOT];

PACKET		GpuOutputPacket[2][PACKETMAX2];	   // GPU packet work area

u_long PadStatus;	 // for the pad

// the TMD object handles
GsDOBJ2 torusOBJ;

GsVIEW2 TheMainView;  	  // viewing handle

int	buffIdx;							// current output buffer

//*****************************************************************************
// tie a handler to a TMD
void initialiseTMD(u_long *tmdAddr, char objNum, GsDOBJ2 *obj)
{
	GsMapModelingData((u_long *)(tmdAddr + 1));

	GsLinkObject4((u_long)(tmdAddr + 3), obj, objNum);
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
// draw a TMD, obj, in the scene. perform translation, t, a rotation about Y
// of ang, and add to ordering table, ot
void drawObject(GsDOBJ2 *obj, MATRIX transformMatrix, GsCOORDINATE2 *super, GsOT *ot)
	{
	MATRIX coordMatrix;
	long adx, adz;	 // absolute difference between object and current position

	obj->coord2->coord = transformMatrix;	// perform transformation on object
	obj->coord2->super = super; 			// move relative to coord system super
 	obj->coord2->flg = 0;					// flag object as moved

	GsGetLs(obj->coord2, &coordMatrix);		// do local screen matrix (performs
	GsSetLsMatrix(&coordMatrix);			// perspective, etc)

	// add object to ordering table
	GsSortObject4(obj, ot, 14 - ot->length, (u_long *)getScratchAddr(0));
	}

//*****************************************************************************
void setupTexturesAndTMDs(void)
	{
	InitialiseTexture(textureTIM);
	initialiseTMD(torusTMD, 0, &torusOBJ);
	}


//*****************************************************************************
// change the texture coordinates
// this all assumes the object is made of only quad gouraud textured polygons
void changeTexture(u_long *addr, MATRIX *tm)
	{
	int i,n;
	TMD_G_4T *p;
	TMD_NORM *norm;
	VECTOR n0,n1,n2,n3;
	VECTOR v;

	addr = (u_long *)((TMD_HDR *)addr + 1);
	n = ((TMD_OBJ *)addr)->n_prim;			// number of primatives
	norm =(TMD_NORM *)(((TMD_OBJ *)addr)->norm_top); // normals pointer
	addr =((TMD_OBJ *)addr)->prim_top;				 // primative pointer

	for (i = 0; i < n; i++)
		{
		p = (TMD_G_4T *)((PRIM_HDR *)addr + 1);	 // skip header

		// modify normals, dependent on transform matrix
		setVector(&v,norm[p->norm0].nx,norm[p->norm0].ny,norm[p->norm0].nz);
		ApplyMatrixLV(tm,&v,&n0);
		setVector(&v,norm[p->norm1].nx,norm[p->norm1].ny,norm[p->norm1].nz);
		ApplyMatrixLV(tm,&v,&n1);
		setVector(&v,norm[p->norm2].nx,norm[p->norm2].ny,norm[p->norm2].nz);
		ApplyMatrixLV(tm,&v,&n2);
		setVector(&v,norm[p->norm3].nx,norm[p->norm3].ny,norm[p->norm3].nz);
		ApplyMatrixLV(tm,&v,&n3);

		p->u0 = 128 - (n0.vx >> 5); p->v0 = 128 - (n0.vy >> 5);
		p->u1 = 128 - (n1.vx >> 5); p->v1 = 128 - (n1.vy >> 5);
		p->u2 = 128 - (n2.vx >> 5); p->v2 = 128 - (n2.vy >> 5);
		p->u3 = 128 - (n3.vx >> 5); p->v3 = 128 - (n3.vy >> 5);

		addr = (u_long *)(p + 1);
		}
	}


int main(void)
{
	MATRIX transformMatrix;
	int i;
	int angle = 0;
	SVECTOR rotate;
	VECTOR translate;
	GsF_LIGHT flatLight;  		   	// light source

	ResetGraph(0);	// reset the graphics

	PadStatus = 0;	// set up controller pad
	PadInit(); 	

   	GsInitGraph(640 ,480, GsOFSGPU, 1, 0);	// screen 640x480
									
	GsDefDispBuff(0, 0, 0, 0);   // display buffer
	GsInit3D();					   // 3D

	for (i = 0; i < 2; i++)				 // setup ordering tables
		{
		OT[i].length = lengthOT;
		OT[i].org = zTable[i];
		}

	setupTexturesAndTMDs();	

	setColor(&flatLight, 128, 128, 128); // setup light, overwrite any
	setVector(&flatLight, 0, 4096, 0); 	 // previous light
	GsSetFlatLight(0, &flatLight);  	
	GsSetFlatLight(1, &flatLight);
	GsSetFlatLight(2, &flatLight);

	GsSetLightMode(0);				   // normal lighting
	GsSetAmbient(ONE/2,ONE/2,ONE/2);   // ambient light (RGB)
	
	GsSetProjection(1000);			   // perpsective

	do
		{
		buffIdx = GsGetActiveBuff();

		GsSetWorkBase((PACKET*)GpuOutputPacket[buffIdx]);

		GsClearOt(0, 0, &OT[buffIdx]);

		TheMainView.view = GsIDMATRIX;
		TheMainView.super = WORLD;

		GsSetView2(&TheMainView);

		transformMatrix = GsIDMATRIX;			  // move object
		setVector(&translate, 0, 0, 1000);
		TransMatrix(&transformMatrix, &translate);
		setVector(&rotate, 0, angle >> 1, angle);
		RotMatrix(&rotate, &transformMatrix);
		
		GsSetLightMatrix(&transformMatrix);

		changeTexture(torusTMD, &transformMatrix);
		drawObject(&torusOBJ, transformMatrix, WORLD, &OT[buffIdx]);

		DrawSync(0);	   // finish any drawing
		VSync(0);
		
		GsSwapDispBuff();  // swap screens

		GsSortClear(0, 0, 0, &OT[buffIdx]);  // add cls to ordering table

 		GsDrawOt(&OT[buffIdx]); 	// draw 

		PadStatus = PadRead();		// read the pad

		angle += 32;
	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

	ResetGraph(1);		// cleanup
	return 0;
}


