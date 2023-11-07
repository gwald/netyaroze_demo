/***************************************************************
*                                                              *
*      Copyright (C) 1997 by Sony Computer Entertainment       *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 5th Aug 97                    *
*                                                              *
***************************************************************/

#include <libps.h>

	// GPU packet space
#define PACKETMAX		2500
#define PACKETMAX2		(PACKETMAX*24)

#define myTMD    (u_long *)0x80090000
#define myTIM    (u_long *)0x800a0000

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

GsDOBJ2 myOBJ;		// the TMD object handles

GsRVIEW2 myView;  	// viewing handle

int	buffIdx;		// current output buffer

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
void initialiseTexture(u_long *timAddr)
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

int main(void)
{
	MATRIX transformMatrix;
        int i;
	short angle;
	SVECTOR rotate;
	GsF_LIGHT flatLight;
	MATRIX coordMatrix;
	RECT r={0,0,32,24};
	RECT r1={0,256,32,24};

	ResetGraph(0);	// reset the graphics

	if (GetVideoMode() == MODE_NTSC)
		GsInitGraph(320 ,240, GsOFSGPU, 1, 0);
	else
		GsInitGraph(320 ,256, GsOFSGPU, 1, 0);

setRECT(&GsDRAWENV.tw,0,0,32,32);
	//--display buffers
	GsDefDispBuff(0, 0, 0, 256);

	//--initialise 3D
	GsInit3D();					

	//--ordering tables
	for (i = 0; i < 2; i++)
		{
        OT[i].length = lengthOT;
		OT[i].org = zTable[i];
		}

	//--move texture to VRAM
	initialiseTexture(myTIM);
	//--attach handler to TMD
	initialiseTMD(myTMD, 0, &myOBJ);

	//--set up light
    setColor(&flatLight, 255, 0, 0);
	setVector(&flatLight, 0, 4096, 0); 	
	GsSetFlatLight(0, &flatLight);  	

    GsSetLightMode(0);                 // normal lighting
	GsSetAmbient(ONE/2,ONE/2,ONE/2);   // ambient light (RGB)
	
	//--set perspective
        GsSetProjection(500);

        for(i = -1000; i < 1000; i++)
		{
		buffIdx = GsGetActiveBuff();

		GsSetWorkBase((PACKET*)GpuOutputPacket[buffIdx]);

		GsClearOt(0, 0, &OT[buffIdx]);

		//--sets up viewing
        myView.vpx = 0;    // from
		myView.vpy = 0;
        myView.vpz = i;

        myView.vrx = 0;    // to
        myView.vry = 0;
        myView.vrz = 0;

		myView.rz = 0; // twist

		myView.super = WORLD;  // in WORLD (no hierachy)

		GsSetRefView2(&myView);

		//--rotates the object
		transformMatrix = GsIDMATRIX;			  // move object
		setVector(&rotate, 0, angle >> 1, angle);
		RotMatrix(&rotate, &transformMatrix);
		
		GsSetLightMatrix(&transformMatrix);

		myOBJ.coord2->coord = transformMatrix;	// set transformation on object
        myOBJ.coord2->super = WORLD;            // object in WORLD coordinate system
        myOBJ.coord2->flg = 0;                  // flag object as moved

        //--find local coords to screen coords matrix
        GsGetLs(myOBJ.coord2, &coordMatrix);
        GsSetLsMatrix(&coordMatrix);

		//--add object to ordering table
		GsSortObject4(&myOBJ, &OT[buffIdx], 14 - OT[buffIdx].length, (u_long *)getScratchAddr(0));

		DrawSync(0);	   // finish any drawing
		VSync(0);

		GsSwapDispBuff();  // swap screens

		GsSortClear(0, 0, 0, &OT[buffIdx]);  // add cls to ordering table

        GsDrawOt(&OT[buffIdx]);         // draw
		angle += 32;
	}

	ResetGraph(1);		// cleanup
	return 0;
}


