/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                           *
*                                                              *
***************************************************************/

#include <libps.h>
#include "pad.h"
#include "blowup.h"
#include "profile.h"

	// GPU packet space
#define PACKETMAX		10000
#define PACKETMAX2		(PACKETMAX*24)

#define MaxNumberOfObjects 10

#define timAddress1 0x800d0000
#define timAddress2 0x800e0000
#define tmdAddress  0x800f0000
#define copyTmd     0x800c0000

#define OT_LENGTH		(14)   

#define setVector(v,_x,_y,_z) (v)->vx = (_x), (v)->vy = (_y), (v)->vz = (_z)

typedef struct
	{
	u_char pmode;
	short px,py;
	short cx,cy;
	} textureInfo;


GsOT		WorldOrderingTable[2];	// Ordering Table handlers
GsOT_TAG	zSortTable[2][1<<OT_LENGTH];   	// actual Ordering Tables themselves							

PACKET		GpuOutputPacket[2][PACKETMAX2];		// GPU packet work area
u_long PadStatus;

GsDOBJ2 object[MaxNumberOfObjects];
GsRVIEW2 TheMainView;

GsCOORDINATE2 objectCoord;
SVECTOR rotateObj = {0,0,0};
VECTOR translate = {0,0,0};
VECTOR viewFrom = {4000,4000,4000};
char dispObj = 0;
char N_Obj;
VECTOR scale = {ONE,ONE,ONE};
GsF_LIGHT lightSource[3];
short ambientRed = ONE / 2, ambientGreen = ONE / 2, ambientBlue = ONE / 2;
GsFOGPARAM fogging = {-1024,20971520L,0,0,0};
u_char foggingOnOff = 0;
// these two globals allow control over screen flash
u_char bang = 0;
int bangCount = 0;

int main(void);
void delay(long d);
textureInfo *findTexture(TMD_OBJ *otable, int object);
void InitialiseAll(void);
void InitialiseTexture(u_long *timAddr);
void controlsMainMenu(void);
void controlsWRTObject(void);
void controlsWRTWorld(void);
void controlsScaling(void);
void printColourDetails(long r, long g, long b, u_char highlight);
void controlsLighting(u_char light);
void controlsAmbient(void);
void controlsFogging(void);
void controlsExplosion(void);
void TMDLimits(GsDOBJ2 obj,VECTOR *min,VECTOR *max);

int main(void)
{
	int	outputBufferIndex;
	MATRIX temp1Matrix, temp2Matrix;
	MATRIX coordMatrix, transformMatrix;
	int i;
	GsCOORD2PARAM cp;
	TMD_HDR *header;
	TMD_OBJ *otable;
	TMD_OBJ *tempobj;
	int t;
	u_char bgCol;
	VECTOR max,min;
	char chr;

	InitialiseAll();

	GsMapModelingData((u_long *)(tmdAddress + 4));

	separateVertices((u_long *)tmdAddress, (u_long *)copyTmd);

	header = (TMD_HDR *)copyTmd;
	N_Obj = header->nobjs;

	otable = (TMD_OBJ *)((TMD_HDR *)copyTmd + 1);

	for(i = 0; i < N_Obj; i++)
		{
		GsLinkObject4((u_long)otable, &object[i], i);
		GsInitCoordinate2(WORLD,&objectCoord);
		object[i].coord2 = &objectCoord;
		TMDLimits(object[i],&min,&max);
		printf("min %d %d %d max %d %d %d\n",min.vx,min.vy,min.vz,max.vx,max.vy,max.vz);
		}

	GsSetLightMode(foggingOnOff);

	TheMainView.vrx = 0; 
	TheMainView.vry = 0; 
	TheMainView.vrz = 0; 
	TheMainView.rz = 0;
	TheMainView.super = WORLD;

	GsSetProjection(1000);

	do
	{
		TheMainView.vpx = viewFrom.vx; 
		TheMainView.vpy = viewFrom.vy; 
		TheMainView.vpz = viewFrom.vz;

		GsSetRefView2(&TheMainView);

		outputBufferIndex = GsGetActiveBuff();

		GsSetWorkBase((PACKET*)GpuOutputPacket[outputBufferIndex]);

		GsClearOt(0, 0, &WorldOrderingTable[outputBufferIndex]);

		GsSetFlatLight(0, &lightSource[0]);
		GsSetFlatLight(1, &lightSource[1]);
		GsSetFlatLight(2, &lightSource[2]);

		GsSetFogParam(&fogging);
		GsSetAmbient(ambientRed,ambientGreen,ambientBlue);

		temp1Matrix = GsIDMATRIX;
		ScaleMatrix(&temp1Matrix,&scale);

		RotMatrix(&rotateObj, &temp2Matrix);

		MulMatrix0(&temp2Matrix,&temp1Matrix,&transformMatrix);

		TransMatrix(&transformMatrix,&translate);

		object[dispObj].coord2->coord = transformMatrix;

 		object[dispObj].coord2->flg = 0;
	
		GsGetLs(object[dispObj].coord2, &coordMatrix);
		GsSetLightMatrix(&GsIDMATRIX);
		GsSetLsMatrix(&coordMatrix);

		GsSortObject4( &object[dispObj], &WorldOrderingTable[outputBufferIndex], 14-OT_LENGTH, (u_long *)getScratchAddr(0));

		DrawSync(0);
		t = VSync(1);
		VSync(0);  
		
		GsSwapDispBuff();		

		// have screen flash a start of explosion
		if (bangCount < 15 && bang)		
			bgCol = bangCount*10;
		else if (bangCount < 50 && bang)
			bgCol = (50 - bangCount)*4;
		else
			bgCol = 0;

		GsSortClear(bgCol, bgCol, bgCol, &WorldOrderingTable[outputBufferIndex]);
		
		GsDrawOt(&WorldOrderingTable[outputBufferIndex]); 

		PadStatus = PadRead();

		FntPrint("vsync %d\n",t);
		FntPrint("Object %d of %d\n",dispObj + 1,N_Obj);

		controlsMainMenu();

		FntFlush(-1);

	} while(!(PadStatus & PADselect) || !(PadStatus & PADstart));

		// cleanup
	ResetGraph(3);
	return 0;
}


void delay(long d)
{
	long v = VSync(-1);
	while ((VSync(-1) - v) < d);
}


textureInfo *findTexture(TMD_OBJ *otable, int object)
{
	int i,j;
	u_long *primpoint;
	u_long primhead;
	textureInfo *info;
	textureInfo *infoStart;
	u_short CBA[MaxNumberOfObjects];
	u_short TSB[MaxNumberOfObjects];
	int textureCount = 0;
	u_char newTexture;
	u_long *p;

	otable += object;

	primpoint = otable->prim_top;

	for (j = 0; j < otable->n_prim; j++)
		{
		primhead = *primpoint;

		if (primhead & 1<<26)
			{
			// get high 16 bits of next 2 parts of primitive CBA and TSB 
			// for use in finding texture/CLUT position
			CBA[textureCount] = *(primpoint + 1) >> 16;
			TSB[textureCount] = *(primpoint + 2) >> 16;

			// check if texture has already been done
			newTexture = 1;
			for(i = 0; i < textureCount; i++)
				if (TSB[textureCount] == TSB[i] && CBA[textureCount] == CBA[i])
					{
			 		newTexture = 0;
					break;
					}

			if (newTexture)
				textureCount++;
			}

		// use primitive mode and options to find size of primitive data 
		// and skip to next primitive
		switch(primhead >> 16)
			{
			case 0x4001: case 0x6c01: case 0x7401: case 0x7c01:
				primpoint += 3;
				break;
			case 0x2000: case 0x2101: case 0x2901: case 0x5001: case 0x6401:
				primpoint += 4;
				break;
			case 0x2800: case 0x3000:
				primpoint += 5;
				break;
			case 0x2004: case 0x2400: case 0x3101: case 0x3800:
				primpoint += 6;
				break;
			case 0x2501: case 0x3004: case 0x3400: case 0x3901:
				primpoint += 7;
				break;
			case 0x2804: case 0x2c00: case 0x2d01:
				primpoint += 8;
				break;
			case 0x3501: case 0x3804: case 0x3c00:
				primpoint += 9;
				break;
			case 0x3d01:
				primpoint += 11;
				break;
			default:
				printf("unsupported TMD mode %x\n",primhead);
			}
		}

	infoStart = (textureInfo *)malloc(9*textureCount);
	info = infoStart;

	for (i = 0; i < textureCount; i++)
		{
		// turn CLUT mode part of TSB into pmode
		if (TSB[i] >> 7 == 0)
			info->pmode = 8;
		else if (TSB[i] >> 7 == 1)
			info->pmode = 9;
		else if (TSB[i] >> 7 == 2)
			info->pmode = 2;

		// turn Tpage part of TSB into x and y
		info->px = (TSB[i] & 0xffff) << 6;
		info->py = (TSB[i] & 0xf0000) << 8;

		// turn CBA into x and y
		info->cx = (CBA[i] & 63) << 4;
		info->cy = CBA[i] >> 6;

		info++;
		}
}



void InitialiseAll(void)
{
	ResetGraph(0);					
	PadStatus = 0;		
	
		// graphical initialisation:
		// screen resolution 512 by 240, non-interlace, use GsGPU offset
	GsInitGraph(512 ,240, GsOFSGPU|GsNONINTER, 1, 0);	
									
	GsDefDispBuff(0, 0, 0, 256);   
	GsInit3D();						

	FntLoad(960, 256);	
	FntOpen(32, 32, 256, 200, 0, 512); 	

	WorldOrderingTable[0].length = OT_LENGTH;	  
	WorldOrderingTable[1].length = OT_LENGTH;		
	WorldOrderingTable[0].org = zSortTable[0];	 
	WorldOrderingTable[1].org = zSortTable[1];	   

 	// set up controller pad buffers
	PadInit(); 	 		
}


void InitialiseTexture(u_long *timAddr)
{
	RECT rect;
	GsIMAGE image;

	GsGetTimInfo(++timAddr,&image);

	rect.x = image.px;
	rect.y = image.py;
	rect.w = image.pw;
	rect.h = image.ph;

	LoadImage(&rect,image.pixel);
	DrawSync(0);
		
	// load CLUT as necessary
	if (image.pmode & 8)
		{
		rect.x = image.cx;
		rect.y = image.cy;
		rect.w = image.cw;
		rect.h = image.ch;

		LoadImage(&rect,image.clut);
		DrawSync(0);
		}
}


void controlsMainMenu(void)
{
	static char menuOption = 0;

	if (PadStatus & PADR1)
		{
		if (--menuOption < 0)
			menuOption = 8;

		delay(10);
		}

	if (PadStatus &	PADR2)
		{
		if (++menuOption > 8)
			menuOption = 0;

		delay(10);
		}

	if (PadStatus & PADselect)
		{
		if (--dispObj < 0)
			dispObj = (N_Obj - 1);			

		delay(10);
		}

	if (PadStatus & PADstart)
		{
		if (++dispObj >= N_Obj)
			dispObj = 0;

		delay(10);
		}

	FntPrint("Menu %d ",menuOption);

	if (menuOption != 8 && bang)
		{
		bang = 0;
		bangCount = 0;
		resetColors((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
		resetVertices((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
		}

	switch (menuOption)
		{
		case 0:
			FntPrint("Move Object\n");
			controlsWRTObject();
			break;
		case 1:
			FntPrint("Move in World\n");
			controlsWRTWorld();
			break;
		case 2:
			FntPrint("Scale Object\n");
			controlsScaling();
			break;
		case 3:
			FntPrint("Light Source 1\n");
			controlsLighting(0);
			break;
		case 4:
			FntPrint("Light Source 2\n");
			controlsLighting(1);
			break;
		case 5:
			FntPrint("Light Source 3\n");
			controlsLighting(2);
			break;
		case 6:
			FntPrint("Ambient Light\n");
			controlsAmbient();
			break;
		case 7:
			FntPrint("Fogging\n");
			controlsFogging();
			break;
		case 8:
			FntPrint("Explosion\n");
			controlsExplosion();
			break;
		}

}


void controlsWRTObject(void)
{
	FntPrint("Rotation %d %d %d\n",rotateObj.vx, rotateObj.vy, rotateObj.vz);

	if (PadStatus & PADLright)
		rotateObj.vx += 8;

	if (PadStatus & PADLleft) 
		rotateObj.vx -= 8;

	if (PadStatus & PADLup)
		rotateObj.vy += 8;

	if (PadStatus & PADLdown)
		rotateObj.vy -= 8;

	if (PadStatus & PADL2)
		rotateObj.vz += 8;

	if (PadStatus & PADL1)
		rotateObj.vz -= 8;

	if (PadStatus & PADRup)
		{
		viewFrom.vx *= 1.02;
		viewFrom.vy *= 1.02;
		viewFrom.vz *= 1.02;
		}

	if (PadStatus & PADRdown)
		{
		viewFrom.vx /= 1.02;
		viewFrom.vy /= 1.02;
		viewFrom.vz /= 1.02;
		}
}


void controlsWRTWorld(void)
{
	FntPrint("View Point %d %d %d\n",viewFrom.vx, viewFrom.vy, viewFrom.vz);

	if (PadStatus & PADLright) 
		viewFrom.vx += 64;

	if (PadStatus & PADLleft)
		viewFrom.vx -= 64;

	if (PadStatus & PADLup)
		viewFrom.vy += 64;

	if (PadStatus & PADLdown)
		viewFrom.vy -= 64;

	if (PadStatus & PADL2)
		viewFrom.vz += 64;

	if (PadStatus & PADL1)
		viewFrom.vz -= 64;

	if (PadStatus & PADRup)
		{
		viewFrom.vx *= 1.02;
		viewFrom.vy *= 1.02;
		viewFrom.vz *= 1.02;
		}

	if (PadStatus & PADRdown)
		{
		viewFrom.vx /= 1.02;
		viewFrom.vy /= 1.02;
		viewFrom.vz /= 1.02;
		}
}


void controlsScaling(void)
{
	FntPrint("Scaling %d %d %d\n",scale.vx, scale.vy, scale.vz);

	if (PadStatus & PADLright) 
		scale.vx += 4;

	if (PadStatus & PADLleft)
		scale.vx -= 4;

	if (PadStatus & PADLup)
		scale.vy += 4;

	if (PadStatus & PADLdown)
		scale.vy -= 4;

	if (PadStatus & PADL2)
		scale.vz += 4;

	if (PadStatus & PADL1)
		scale.vz -= 4;
}


// r,g and b long so routine works for ambient colours
void printColourDetails(long r, long g, long b, u_char highlight)
{
	FntPrint("Colour ");
	switch (highlight)
		{
		case 0:
			// highlight red
			FntPrint("~c0f0R %d ~cfffG %d B %d\n",r,g,b);
			break;
		case 1:
			// highlight green
			FntPrint("R %d ~c0f0G %d ~cfffB %d\n",r,g,b);
			break;
		case 2:
			// highlight blue
			FntPrint("R %d G %d ~c0f0B %d\n~cfff",r,g,b);
			break;
		default:
			// highlight none
			FntPrint("R %d G %d B %d\n",r,g,b);
		}
}


void controlsLighting(u_char light)
{
	static char rgb = 0;

	FntPrint("Direction X %d\n          Y %d\n          Z %d\n",lightSource[light].vx, lightSource[light].vy, lightSource[light].vz);

	printColourDetails(lightSource[light].r,lightSource[light].g,lightSource[light].b,rgb);

	if (PadStatus & PADLright) 
		lightSource[light].vx += 8;

	if (PadStatus & PADLleft)
		lightSource[light].vx -= 8;

	if (PadStatus & PADLup)
		lightSource[light].vy += 8;

	if (PadStatus & PADLdown)
		lightSource[light].vy -= 8;

	if (PadStatus & PADL2)
		lightSource[light].vz += 8;

	if (PadStatus & PADL1)
		lightSource[light].vz -= 8;

	if (PadStatus & PADRright)
		{
		if (++rgb > 2)
			rgb = 0;

		delay(10);
		}

	if (PadStatus & PADRleft)
		{
		if (--rgb < 0)
			rgb = 2;

		delay(10);
		}

	switch (rgb)
		{
		case 0:
			if (PadStatus & PADRup)
 				lightSource[light].r++;

			if (PadStatus & PADRdown)
				lightSource[light].r--;

			break;
		case 1:
			if (PadStatus & PADRup)
				lightSource[light].g++;

			if (PadStatus & PADRdown)
				lightSource[light].g--;

			break;
		case 2:
			if (PadStatus & PADRup)
				lightSource[light].b++;

			if (PadStatus & PADRdown)
				lightSource[light].b--;

			break;
		}

}


void controlsAmbient()
{
	static char rgb = 0;

	printColourDetails(ambientRed,ambientGreen,ambientBlue,rgb);

	if (PadStatus & PADRright)
		{
		if (++rgb > 2)
			rgb = 0;

		delay(10);
		}

	if (PadStatus & PADRleft)
		{
		if (--rgb < 0)
			rgb = 2;

		delay(10);
		}

	switch (rgb)
		{
		case 0:
			if (PadStatus & PADRup)
 				ambientRed += 8;

			if (PadStatus & PADRdown)
				ambientRed -= 8;

			break;
		case 1:
			if (PadStatus & PADRup)
				ambientGreen += 8;

			if (PadStatus & PADRdown)
				ambientGreen -= 8;

			break;
		case 2:
			if (PadStatus & PADRup)
				ambientBlue += 8;

			if (PadStatus & PADRdown)
				ambientBlue -= 8;

			break;
		}
}


void controlsFogging()
{
	static char option;

	switch (option)
		{
		case 0:
			if (foggingOnOff == 0)
				FntPrint("~c0f0Fogging Off\n");
			else
				FntPrint("~c0f0Fogging On\n");

			FntPrint("~cfffdqa %d dqb %d\n", fogging.dqa, fogging.dqb);

			printColourDetails(fogging.rfc,fogging.gfc,fogging.bfc,3);
			break;
		case 1:
			if (foggingOnOff == 0)
				FntPrint("Fogging Off\n");
			else
				FntPrint("Fogging On\n");

			FntPrint("~c0f0dqa %d ~cfffdqb %d\n",fogging.dqa,fogging.dqb);

			printColourDetails(fogging.rfc,fogging.gfc,fogging.bfc,3);
			break;

		case 2:
			if (foggingOnOff == 0)
				FntPrint("Fogging Off\n");
			else
				FntPrint("Fogging On\n");

			FntPrint("dqa %d ~c0f0dqb %d\n~cfff",fogging.dqa,fogging.dqb);

			printColourDetails(fogging.rfc,fogging.gfc,fogging.bfc,3);
			break;
		default:
			if (foggingOnOff == 0)
				FntPrint("Fogging Off\n");
			else
				FntPrint("Fogging On\n");

			FntPrint("dqa %d dqb %d\n", fogging.dqa, fogging.dqb);

			printColourDetails(fogging.rfc,fogging.gfc,fogging.bfc,option - 3);
		}


	if (PadStatus & PADRright)
		{
		if (++option > 5)
			option = 0;

		delay(10);
		}

	if (PadStatus & PADRleft)
		{
		if (--option < 0)
			option = 5;

		delay(10);
		}

	switch (option)
		{
		case 0:
			if ((PadStatus & PADRup) || (PadStatus & PADRdown))
				{
				foggingOnOff = !foggingOnOff;
				GsSetLightMode(foggingOnOff);
				delay(10);
				}

			break;
		case 1:
			if (PadStatus & PADRup)
				fogging.dqa += 64;

			if (PadStatus & PADRdown)
				fogging.dqa -= 64;

			break;
		case 2:
			if (PadStatus & PADRup)
				fogging.dqb += 4096;

			if (PadStatus & PADRdown)
				fogging.dqb -= 4096;

			break;
		case 3:
			if (PadStatus & PADRup)
 				fogging.rfc++;

			if (PadStatus & PADRdown)
				fogging.rfc--;

			break;
		case 4:
			if (PadStatus & PADRup)
				fogging.gfc++;

			if (PadStatus & PADRdown)
				fogging.gfc--;

			break;
		case 5:
			if (PadStatus & PADRup)
				fogging.bfc++;

			if (PadStatus & PADRdown)
				fogging.bfc--;

			break;
		}
}



void controlsExplosion(void)
{
	static char speed = 7;
	static char option = 0;

	if (bangCount < 314 && bang)
		{
		blowVertices(object[dispObj], speed, bangCount);
		bangCount ++;
		}
	else if (bang)
		{
		bangCount = 0;
		resetColors((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
		resetVertices((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
		}

	switch(option)
		{
		case 0:
			FntPrint("~c0f0Speed %d\n~cfff",speed);
			if (bang)
				FntPrint("Bang On\n");
			else
				FntPrint("Bang Off\n");
			break;
		case 1:
			FntPrint("Speed %d\n",speed);
			if (bang)
				FntPrint("~c0f0Bang On~cfff\n");
			else
				FntPrint("~c0f0Bang Off~cfff\n");
		}

	if ((PadStatus & PADRright) || (PadStatus & PADRleft))
		{
		option = !option;
		delay(10);
		}

	switch (option)
		{
		case 0:
			if (PadStatus & PADRup)
				{
				if (++speed	> 13)
					speed = 1;
				delay(10);
				}

			if (PadStatus & PADRdown)
				{
				if (--speed < 1)
					speed = 13;
				delay(10);
				}

			break;
		case 1:
			if ((PadStatus & PADRup) || (PadStatus & PADRdown))
				{
				bang = !bang;
				bangCount = 0;
				resetColors((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
				resetVertices((u_long *)tmdAddress, (u_long *)copyTmd,dispObj);
				delay(10);
				}

			break;
		}
}


