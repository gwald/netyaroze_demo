/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 7th Aug 97                                         *
*                                                              *
***************************************************************/

#include <libps.h>
#include "pad.h"
#include "tmd.h"

	// GPU packet space
#define PACKETMAX		1000
#define PACKETMAX2		(PACKETMAX*24)

#define textureTIM  (u_long *)0x80090000
#define myTMD       (u_long *)0x800b0000

#define lengthOT  10

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

GsDOBJ2 myOBJ;		// the TMD object handle

GsVIEW2 TheMainView;  	  // viewing handle

int	buffIdx;		 	// current output buffer

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

void createMyTMD()
	{
	// create a TMD
	TMD_start(myTMD); // sets up TMD header, etc
	TMD_addPolyF3();  // polys to include in TMD
	TMD_addPolyFT3();
	TMD_addPolyG3();
	TMD_addPolyGT3();
	TMD_addPolyF3N();
	TMD_addPolyFT3N();
	TMD_addPolyG3N();
	TMD_addPolyGT3N();
	TMD_addPolyF4();
	TMD_addPolyFT4();
	TMD_addPolyG4();
	TMD_addPolyGT4();
	TMD_addPolyF4N();
	TMD_addPolyFT4N();
	TMD_addPolyG4N();
	TMD_addPolyGT4N();
	TMD_end();		  // completes TMD information

	// initialise all the polygons in the TMD
	// set up polygon F3
	TMD_setRGB0(myTMD,0,0,255,0);
	TMD_setVert0(myTMD,0,-150,-50,-50);
	TMD_setVert1(myTMD,0,-50,50,-50);
	TMD_setVert2(myTMD,0,-150,50,-50);
	TMD_setNorm0(myTMD,0,0,0,-4096);

	// set up poly FT3
	TMD_setUV3(myTMD,1,0,0,0,255,255,255);
	TMD_setClut(myTMD,1,GetClut(640,256));
	TMD_setTPage(myTMD,1,GetTPage(1,0,640,0));
	TMD_setVert0(myTMD,1,-150,-50,-50);
	TMD_setVert1(myTMD,1,-50,-50,-50);
	TMD_setVert2(myTMD,1,-50,50,-50);
	TMD_setNorm0(myTMD,1,0,0,-4096);

	// set up polygon G3
	TMD_setRGB0(myTMD,2,255,255,0);
	TMD_setVert0(myTMD,2,-150,-50,50);
	TMD_setVert1(myTMD,2,-150,-50,-50);
	TMD_setVert2(myTMD,2,-150,50,-50);
	TMD_setNorm0(myTMD,2,-4096,0,0);
	TMD_setNorm1(myTMD,2,-4096,0,0);
	TMD_setNorm2(myTMD,2,-4096,0,0);

	// GT3
	TMD_setUV3(myTMD,3,0,0,0,255,255,255);
	TMD_setClut(myTMD,3,GetClut(640,256));
	TMD_setTPage(myTMD,3,GetTPage(1,0,640,0));
	TMD_setVert0(myTMD,3,-150,-50,50);
	TMD_setVert1(myTMD,3,-150,50,-50);
	TMD_setVert2(myTMD,3,-150,50,50);
	TMD_setNorm0(myTMD,3,-4096,0,0);
	TMD_setNorm1(myTMD,3,-4096,0,0);
	TMD_setNorm2(myTMD,3,-4096,0,0);

	// set up polygon F3N
	TMD_setRGB0(myTMD,4,0,0,255);
	TMD_setVert0(myTMD,4,-50,-50,50);
	TMD_setVert1(myTMD,4,-50,50,-50);
	TMD_setVert2(myTMD,4,-50,-50,-50);

	// set up poly FT3N
	TMD_setUV3(myTMD,5,0,0,0,255,255,255);
	TMD_setClut(myTMD,5,GetClut(640,256));
	TMD_setTPage(myTMD,5,GetTPage(1,0,640,0));
	TMD_setRGB0(myTMD,5,0,0,255);
	TMD_setVert0(myTMD,5,-50,-50,50);
	TMD_setVert1(myTMD,5,-50,50,50);
	TMD_setVert2(myTMD,5,-50,50,-50);

	// set up polygon G3N
	TMD_setRGB0(myTMD,6,255,255,255);
	TMD_setRGB1(myTMD,6,0,255,0);
	TMD_setRGB2(myTMD,6,255,255,0);
	TMD_setVert0(myTMD,6,-50,-50,50);
	TMD_setVert1(myTMD,6,-150,-50,50);
	TMD_setVert2(myTMD,6,-150,50,50);

	// GT3N
	TMD_setUV3(myTMD,7,0,0,0,255,255,255);
	TMD_setClut(myTMD,7,GetClut(640,256));
	TMD_setTPage(myTMD,7,GetTPage(1,0,640,0));
	TMD_setRGB0(myTMD,7,255,255,0);
	TMD_setRGB1(myTMD,7,0,255,0);
	TMD_setRGB2(myTMD,7,0,255,255);
	TMD_setVert0(myTMD,7,-50,-50,50);
	TMD_setVert1(myTMD,7,-150,50,50);
	TMD_setVert2(myTMD,7,-50,50,50);

	// set up polygon F4
	TMD_setRGB0(myTMD,8,0,127,255);
	TMD_setVert0(myTMD,8,-150,-50,50);
	TMD_setVert1(myTMD,8,-50,-50,50);
	TMD_setVert2(myTMD,8,-150,-50,-50);
	TMD_setVert3(myTMD,8,-50,-50,-50);
	TMD_setNorm0(myTMD,8,0,-4096,0);

	// set up poly FT4
	TMD_setUV4(myTMD,9,0,0,255,0,0,255,255,255);
	TMD_setClut(myTMD,9,GetClut(640,256));
	TMD_setTPage(myTMD,9,GetTPage(1,0,640,0));
	TMD_setVert0(myTMD,9,-150,50,-50);
	TMD_setVert1(myTMD,9,-50,50,-50);
	TMD_setVert2(myTMD,9,-150,50,50);
	TMD_setVert3(myTMD,9,-50,50,50);
	TMD_setNorm0(myTMD,9,0,4096,0);

	// set up polygon G4
	TMD_setRGB0(myTMD,10,0,127,255);
	TMD_setVert0(myTMD,10,50,-50,50);
	TMD_setVert1(myTMD,10,150,-50,50);
	TMD_setVert2(myTMD,10,50,-50,-50);
	TMD_setVert3(myTMD,10,150,-50,-50);
	TMD_setNorm0(myTMD,10,0,-4096,0);
	TMD_setNorm1(myTMD,10,0,-4096,0);
	TMD_setNorm2(myTMD,10,0,-4096,0);
	TMD_setNorm3(myTMD,10,0,-4096,0);

	// set up poly GT4
	TMD_setUV4(myTMD,11,0,0,255,0,0,255,255,255);
	TMD_setClut(myTMD,11,GetClut(640,256));
	TMD_setTPage(myTMD,11,GetTPage(1,0,640,0));
	TMD_setVert0(myTMD,11,50,50,-50);
	TMD_setVert1(myTMD,11,150,50,-50);
	TMD_setVert2(myTMD,11,50,50,50);
	TMD_setVert3(myTMD,11,150,50,50);
	TMD_setNorm0(myTMD,11,0,4096,0);
	TMD_setNorm1(myTMD,11,0,4096,0);
	TMD_setNorm2(myTMD,11,0,4096,0);
	TMD_setNorm3(myTMD,11,0,4096,0);

	// set up polygon F4N
	TMD_setRGB0(myTMD,12,0,127,255);
	TMD_setVert0(myTMD,12,150,-50,50);
	TMD_setVert1(myTMD,12,50,-50,50);
	TMD_setVert2(myTMD,12,150,50,50);
	TMD_setVert3(myTMD,12,50,50,50);

	// set up poly FT4N
	TMD_setUV4(myTMD,13,0,0,255,0,0,255,255,255);
	TMD_setClut(myTMD,13,GetClut(640,256));
	TMD_setTPage(myTMD,13,GetTPage(1,0,640,0));
	TMD_setRGB0(myTMD,13,127,127,255);
	TMD_setVert0(myTMD,13,50,-50,-50);
	TMD_setVert1(myTMD,13,150,-50,-50);
	TMD_setVert2(myTMD,13,50,50,-50);
	TMD_setVert3(myTMD,13,150,50,-50);

	// set up polygon G4N
	TMD_setRGB0(myTMD,14,0,127,255);
	TMD_setRGB1(myTMD,14,255,127,127);
	TMD_setRGB2(myTMD,14,0,127,127);
	TMD_setRGB3(myTMD,14,127,127,255);
	TMD_setVert0(myTMD,14,50,-50,50);
	TMD_setVert1(myTMD,14,50,-50,-50);
	TMD_setVert2(myTMD,14,50,50,50);
	TMD_setVert3(myTMD,14,50,50,-50);

	// set up poly GT4N
	TMD_setUV4(myTMD,15,0,0,255,0,0,255,255,255);
	TMD_setClut(myTMD,15,GetClut(640,256));
	TMD_setTPage(myTMD,15,GetTPage(1,0,640,0));
	TMD_setRGB0(myTMD,15,127,127,255);
	TMD_setRGB1(myTMD,15,127,255,255);
	TMD_setRGB2(myTMD,15,255,127,255);
	TMD_setRGB3(myTMD,15,0,255,255);
	TMD_setVert0(myTMD,15,150,-50,-50);
	TMD_setVert1(myTMD,15,150,-50,50);
	TMD_setVert2(myTMD,15,150,50,-50);
	TMD_setVert3(myTMD,15,150,50,50);

	// polygons done.

	// all the TMD_set.. commands can be used once running to alter the
	// polygons. altering the number of polygons and type once running
	// is more difficult and requires 2 TMDs.
	}

int main(void)
{
	MATRIX transformMatrix;
    int i;
	int angle = 0;
	SVECTOR rotate;
	VECTOR translate;
	GsF_LIGHT flatLight;
    long fntPrintID;
    int hsync;

#ifdef __s
printf("kjgb");
#endif

	ResetGraph(0);

	PadStatus = 0;
	PadInit(); 	

	if (GetVideoMode() == MODE_NTSC)
		GsInitGraph(640 ,480, GsOFSGPU, 1, 0);	// screen 640x480
	else
		{
		GsInitGraph(640 ,512, GsOFSGPU, 1, 0);	// screen 640x512
		GsDRAWENV.isbg = 1;						// fix clear screen problem
		}

	GsDefDispBuff(0, 0, 0, 0);   // display buffer
	GsInit3D();					   // 3D

	for (i = 0; i < 2; i++)				 // setup ordering tables
		{
        OT[i].length = lengthOT;
		OT[i].org = zTable[i];
		}

	FntLoad(960, 256);					 // fonts
	fntPrintID = FntOpen(-240, -100, 180, 100, 0, 256); 

	createMyTMD();

	InitialiseTexture(textureTIM);
	// still need to tie TMD to handler
	initialiseTMD(myTMD, 0, &myOBJ);

	setColor(&flatLight, 128, 128, 128); // setup light
	setVector(&flatLight, 0, 4096, 0);
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

		transformMatrix = GsIDMATRIX;			  // view
		setVector(&translate, 0, 0, 1000);
		TransMatrix(&transformMatrix, &translate);
		
		TheMainView.view = transformMatrix;
		TheMainView.super = WORLD;

		GsSetView2(&TheMainView);

		transformMatrix = GsIDMATRIX;		// move object
		setVector(&rotate, 0, angle >> 1, angle);
		RotMatrix(&rotate, &transformMatrix);
		
		GsSetLightMatrix(&transformMatrix); // object moved, set light matrix

		drawObject(&myOBJ, transformMatrix, WORLD, &OT[buffIdx]);

		DrawSync(0);
        hsync = VSync(1);
		VSync(0);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &OT[buffIdx]);

        GsDrawOt(&OT[buffIdx]);

		PadStatus = PadRead();

        FntPrint(fntPrintID,"hsync %d\n",hsync);
		FntFlush(-1);

		angle += 32;
	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

	ResetGraph(1);		// cleanup
	return 0;
}


