/* $PSLibId: Runtime Library Version 3.5$ */
/* tmdview4: GsDOBJ2 object viewing rotine 
 * 
 * "tuto5.c" ******** Multi Screen Coordinate Use libgs VER 3.2 LATER 
 * 
 * Version 1.00	Jun,  15, 1995 
 * 
 * Copyright (C) 1995 by Sony Computer Entertainment All rights Reserved 
 */

#include <libps.h>
#include "pad.h"



#define OBJECTMAX 100		/* Max Objects */

#define TEX_ADDR   0x80120000	/* Top Address of texture data1 (TIM FORMAT) */

#define TEX_ADDR1   0x80130000	/* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80140000	/* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80150000	/* Top Address of modeling data (TMD FORMAT) */

#define OT_LENGTH  12		/* bit length of OT */


GsOT    Wot[2];			/* OT handler for double buffers */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* substance of OT */

GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler */

u_long  Objnum;			/* valibable of number of Objects */


GsCOORDINATE2 DWorld;		/* Coordinate for GsDOBJ2 */

SVECTOR PWorld;			/* work short vector for making Coordinate parameter */

GsRVIEW2 view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long  padd;			/* Controler data */

/* work area of PACKET DATA used double size for packet double buffer */
u_long  out_packet[2][0x10000];

// buffers for the controller pad
volatile u_char *bb0, *bb1;


static u_long PadRead(long id);


/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* the pointer of Object handler */
	int     outbuf_idx;
	MATRIX  tmpls, tmplw;

	// OLD		ResetCallback();
	init_all();

	FntLoad(960, 256);
	// OLD		SetDumpFnt(FntOpen(0, -64, 256, 200, 0, 512));

	while (1) {
		GsSetRefView2(&view);	
		if (obj_interactive() == 0)
			return 0;	/* interactive parameter get */
		outbuf_idx = GsGetActiveBuff();	/* get the double buffer index */
		GsSetWorkBase((PACKET *)out_packet[outbuf_idx]);
		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* clear ordering table */

		for (i = 0, op = object; i < Objnum; i++) {
			/* Calculate Local-World Matrix & Local-Screen Matrix */
			GsGetLws(op->coord2, &tmplw, &tmpls);
			/* Set LWMATRIX to GTE Lighting Registers */
			GsSetLightMatrix(&tmplw);
			/* Set LSAMTRIX to GTE Registers */
			GsSetLsMatrix(&tmpls);
			/* Perspective Translate Object and Set OT */
			GsSortObject4(op, &Wot[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		padd = PadRead(1);	/* Readint Control Pad data */
		VSync(0);	/* Wait VSYNC */
		ResetGraph(1);	/* Reset GPU */
		GsSwapDispBuff();	/* Swap double buffer */
		SetDispMask(1);

		/* Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* Start Drawing */
		GsDrawOt(&Wot[outbuf_idx]);
		FntFlush(-1);
	}
}

obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;
	static int level = 0;
	static SVECTOR scale = {ONE, ONE, ONE};

	/* Rotate Y */
	if ((padd & PADRleft) > 0)
		PWorld.vy -= 5 * ONE / 360;
	/* Rotate Y */
	if ((padd & PADRright) > 0)
		PWorld.vy += 5 * ONE / 360;
	/* Rotate X */
	if ((padd & PADRup) > 0)
		PWorld.vx += 5 * ONE / 360;
	/* Rotate X */
	if ((padd & PADRdown) > 0)
		PWorld.vx -= 5 * ONE / 360;

	/* Transfer Z */
	if ((padd & PADm) > 0)
		DWorld.coord.t[2] -= 1000;
	/* Transfer Z */
	if ((padd & PADl) > 0)
		DWorld.coord.t[2] += 1000;
	/* Transfer X */
	if ((padd & PADLleft) > 0)
		DWorld.coord.t[0] += 10;
	/* Transfer X */
	if ((padd & PADLright) > 0)
		DWorld.coord.t[0] -= 10;
	/* Transfer Y */
	if ((padd & PADLdown) > 0)
		DWorld.coord.t[1] += 10;
	/* Transfer Y */
	if ((padd & PADLup) > 0)
		DWorld.coord.t[1] -= 10;
	/* Exit Program */
	if ((padd & PADk) > 0) {
		ResetGraph(3);
		return 0;
	}
	/* Calculate Matrix from Object Parameter and Set Coordinate */
	set_coordinate(&PWorld, &DWorld);

	FntPrint("Z = %d\n", DWorld.coord.t[2]);

	if ((padd & PADn) > 0 && level == 0) {
		level = 1;
		scale.vx = ONE / 8;
		scale.vy = ONE / 8;
		scale.vz = ONE / 8;
	}
	if ((padd & PADo) > 0 && level == 1) {
		level = 0;
		scale.vx = ONE;
		scale.vy = ONE;
		scale.vz = ONE;
	}
	if (level == 0)
		FntPrint("SCREEN SCALE is NORMAL");
	else
		FntPrint("SCREEN SCALE is 1/8");

	GsScaleScreen(&scale);	/* reset normal screen coordinate scale */
	return 1;
}


init_all()
{				/* initialize rotines */
	ResetGraph(0);		/* reset GPU */
	// OLD		PadInit(0);		/* init controler */
	GetPadBuf(&bb0, &bb1);	
	padd = 0;		/* init contorler value */


	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (interrace mode) */

	GsDefDispBuff(0, 0, 0, 0);	/* set the double buffers */

#if 0
	GsInitGraph(640, 240, GsNONINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (on interrace mode) */
	GsDefDispBuff(0, 0, 0, 240);	/* set the double buffers */
#endif

	GsInit3D();		/* init 3d part of libgs */

	Wot[0].length = OT_LENGTH;	/* set the length of OT1 */
	Wot[0].org = zsorttable[0];	/* set the top address of OT1 tags */

	/* set anoter OT for double buffer */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	coord_init();		/* initialize the coordinate system` */
	model_init();		/* set up the modeling data */
	view_init();		/* set the viewpoint */
	light_init();		/* set the flat light */

	texture_init(TEX_ADDR);	/* 16bit texture load */
	texture_init(TEX_ADDR1);/* 8bit  texture load */
	texture_init(TEX_ADDR2);/* 4bit  texture load */
}


view_init()
{				/* set the viewpoint */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* set the projection */

	/* set the viewpoint parameter */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;
	/* set the refarence point parameter */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = -4000;
	/* set the roll pameter of viewpoint */
	view.rz = 0;
	view.super = WORLD;	/* set the view coordinate */

	/* set the view point from parameters (libgs caliculate World-Screen
	   Matrix) */
	GsSetRefView2(&view);
}

light_init()
{				/* init Flat light */
	/* Setting Light ID 0 */
	/* Setting direction vector of Light0 */
	pslt[0].vx = 20;
	pslt[0].vy = -100;
	pslt[0].vz = -100;

	/* Setting color of Light0 */
	pslt[0].r = 0xd0;
	pslt[0].g = 0xd0;
	pslt[0].b = 0xd0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);

	/* Setting Light ID 1 */
	pslt[1].vx = 20;
	pslt[1].vy = -50;
	pslt[1].vz = 100;
	pslt[1].r = 0x80;
	pslt[1].g = 0x80;
	pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);

	/* Setting Light ID 2 */
	pslt[2].vx = -20;
	pslt[2].vy = 20;
	pslt[2].vz = -100;
	pslt[2].r = 0x60;
	pslt[2].g = 0x60;
	pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);

	/* Setting Ambient */
	GsSetAmbient(0, 0, 0);

	/* Setting default light mode */
	GsSetLightMode(0);
}

coord_init()
{				/* Setting coordinate */
	/* Setting hierarchy of Coordinate */
	GsInitCoordinate2(WORLD, &DWorld);

	/* Init work vector */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	/* the org point of DWold is set to Z = -4000 */
	DWorld.coord.t[2] = -4000;
}


/* Set coordinte parameter from work vector */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* work vector */
	GsCOORDINATE2 *coor;	/* Coordinate */
{
	MATRIX  tmp1;

	/* Set translation */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* Rotate Matrix */
	RotMatrix(pos, &tmp1);

	/* Set Matrix to Coordinate */
	coor->coord = tmp1;

	/* Clear flag becase of changing parameter */
	coor->flg = 0;
}

/* Load texture to VRAM */
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/* Get texture information of TIM FORMAT */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* X point of image data on VRAM */
	rect1.y = tim1.py;	/* Y point of image data on VRAM */
	rect1.w = tim1.pw;	/* Width of image */
	rect1.h = tim1.ph;	/* Height of image */

	/* Load texture to VRAM */
	LoadImage(&rect1, tim1.pixel);

	/* Exist Color Lookup Table */
	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx;	/* X point of CLUT data on VRAM */
		rect1.y = tim1.cy;	/* Y point of CLUT data on VRAM */
		rect1.w = tim1.cw;	/* Width of CLUT */
		rect1.h = tim1.ch;	/* Height of CLUT */

		/* Load CULT data to VRAM */
		LoadImage(&rect1, tim1.clut);
	}
}


model_init()
{				/* set up the modeling data */
	u_long *dop;
	GsDOBJ2 *objp;		/* the handler or modeling data */
	int     i;

	dop = (u_long *) MODEL_ADDR;	/* the top address of modeling data */
	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* map the modeling data to real address */
	dop++;
	Objnum = *dop;		/* get the number of objects */
	dop++;			/* inc the address to link to the handler */

	/* Link TMD data and Object Handler */
	for (i = 0; i < Objnum; i++)	
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {	
	/* default object coordinate */
		objp->coord2 = &DWorld;

		objp->attribute = 0;	/* attribute init */

		objp++;
	}
}



static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

