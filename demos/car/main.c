/*
 * $PSLibId: Runtime Library Version 3.5$
 */
/*				60frame
 *
 *         Copyright (C) 1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Nov,11,1995	sachiko
 *
 *			60frame or 30frame
 */

#include <libps.h>
#include "pad.h"



#define OT_LENGTH	12		
#define	OT_SIZE		(1<<OT_LENGTH) 		 // 2 to the power OT_LENGTH
	// screen-to-viewpoint distance
#define	SCR_Z		1000   

#define OBJECTMAX 100		/* Max Objects */

#define PACKETMAX  10000		/* Max GPU packets */
#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte)
                                    	   paket size may be 24 byte(6 word) */
#define	FRAME_30	0
#define	FRAME_60	1

/* Top Address of texture data (TIM FORMAT) */
#define TEX_ADDR1	0x80120000
#define TEX_ADDR2	0x80130000
#define TEX_ADDR3	0x80140000
#define TEX_ADDR4	0x80150000
#define TEX_ADDR5	0x80160000
#define TEX_ADDR6	0x80170000
#define TEX_ADDR7	0x80180000

/* Top Address of modeling data (TMD FORMAT) */
#define MODEL_ADDR	0x80190000

typedef struct {
	GsOT		gsot;
	GsOT_TAG	ot[OT_SIZE];
	PACKET		packet[PACKETMAX2];
} DB;



	// low-level buffers for the controller pad
volatile u_char *bb0, *bb1;

static u_long PadRead(long id);


static void init_system(DB *db);
static void init_coordinate(GsCOORDINATE2 *objWorld,SVECTOR *rotate);
static void init_view();
static void init_light();
static void init_model(GsCOORDINATE2 *objWorld,GsDOBJ2 *object,int *objcnt);
static void init_texture();
static void load_texture(u_long addr);
static int  pad_read(GsCOORDINATE2 *objWorld,SVECTOR *rotate,int *frame);
static void move_object(GsCOORDINATE2 *objWorld,SVECTOR *rotate);
static void draw_object(DB *db,GsDOBJ2 *object,int objcnt,int frame);
static void opening(GsCOORDINATE2 *objWorld,SVECTOR *rotate,
					DB *db,GsDOBJ2 *object,int objcnt);

extern MATRIX	GsIDMATRIX;

main()
{
	GsCOORDINATE2	objWorld;	
	int		objcnt;
	DB		db[2];		/* packet double buffer: */
	int		idx;		/* current db index */
	SVECTOR		rotate;		
	GsDOBJ2		object[OBJECTMAX];	/* Array of Object Handler */
	static int	frame=FRAME_60;

	/* initialize */
	init_system(db);
	init_coordinate(&objWorld,&rotate);
	init_view();
	init_light();
	init_model(&objWorld,object,&objcnt);
	init_texture();

	FntLoad(960,256);

		// opening sequence as car approaches foreground
	opening(&objWorld,&rotate,db,object,objcnt);

	while ( pad_read(&objWorld,&rotate,&frame)==NULL ) {

		/* move objects */
		move_object(&objWorld,&rotate);

		/* Get double buffer index */
		idx = GsGetActiveBuff();

		/* draw objects */
		draw_object(&db[idx],object,objcnt,frame);

	}

		// clean-up
	ResetGraph(3);
	return 0;
}



static void init_system(DB *db)
{
	GetPadBuf(&bb0, &bb1);

	/* initialize GPU */
	GsInitGraph(640,240,GsINTER|GsOFSGPU,0,0);
	GsDefDispBuff(0,0,0,240);

	/* initialize OT */
	db[0].gsot.length = db[1].gsot.length = OT_LENGTH;
	db[0].gsot.point  = db[1].gsot.point  = 0;
	db[0].gsot.offset = db[1].gsot.offset = 0;
	db[0].gsot.org    = db[0].ot;
	db[1].gsot.org    = db[1].ot;

	/* initialize 3D system */
	GsInit3D();
}

static void init_coordinate(GsCOORDINATE2 *objWorld,SVECTOR *rotate)
{
		// clear memory with zeros
	memset(objWorld,0,sizeof(GsCOORDINATE2));
	memset(rotate,0,sizeof(SVECTOR));

	/* setting hierarchy of coordinate */
	GsInitCoordinate2(WORLD,objWorld);
}

static void init_view()
{
	GsRVIEW2	view;

		// screen-to-viewpoint distance
	GsSetProjection(SCR_Z);

	view.vpx = 0; view.vpy = 0; view.vpz = 1000;
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	view.rz  = 0;
	view.super = WORLD;

	GsSetRefView2(&view);
}

static void init_light()
{
	GsF_LIGHT	light;

		// set direction of parallel light source
	light.vx = 20;   light.vy = -100;  light.vz = -100;
		// set colour of light source
	light.r  = 0xd0; light.g  = 0xd0; light.b  = 0xd0;
		// register light in system
	GsSetFlatLight(0,&light);

		// set medium-level background lighting
	GsSetAmbient(ONE/2,ONE/2,ONE/2);

		// set standard lighting mode
	GsSetLightMode(0);
}




/* Read modeling data (TMD FORMAT) */
static void init_model(GsCOORDINATE2 *objWorld,GsDOBJ2 *object,int *objcnt)
{
	u_long	*dop;
	GsDOBJ2	*objp;		/* handler of object */
	int i;

	/* Top Address of MODELING DATA(TMD FORMAT) */
	dop=(u_long *)MODEL_ADDR;
	dop++;	/* hedder skip */

	/* Mapping real address */
	GsMapModelingData(dop);

	dop++;
	*objcnt = *dop;		/* Get number of Objects */

	dop++;			/* Adjusting for GsLinkObject4 */

	/* Link ObjectHandler and TMD FORMAT MODELING DATA */
	for(i=0;i<*objcnt;i++)
		GsLinkObject4((u_long)dop,&object[i],i);

	objp = object;
	i = *objcnt;
	while ( i-- ) {
		/* Set Coordinate of Object Handler */
		objp->coord2 =  objWorld;

		objp->attribute = 0;
		objp++;
	}
}



	// load all TIM files from main memory to frame buffer
static void init_texture()
{
	load_texture(TEX_ADDR1);
	load_texture(TEX_ADDR2);
	load_texture(TEX_ADDR3);
	load_texture(TEX_ADDR4);
	load_texture(TEX_ADDR5);
	load_texture(TEX_ADDR6);
	load_texture(TEX_ADDR7);
}



/* Load texture to VRAM	*/
static void load_texture(u_long addr)
{
	RECT	rect;		  // destination rectangle on frame buffer
	GsIMAGE	tim;		  // info on TIM file

	/* Get texture information of TIM FORMAT */  
	GsGetTimInfo((u_long *)(addr+4),&tim);

	/* XY point of image data on VRAM */
	rect.x = tim.px;
	rect.y = tim.py;

	/* Width and Height of image */
	rect.w = tim.pw;
	rect.h = tim.ph;

	/* Load texture to VRAM */
	LoadImage(&rect,tim.pixel);

		// if CLUT required (4 or 8 bit texture), load it to VRAM
	if((tim.pmode>>3)&0x01)
	{
		/* XY point of CLUT data on VRAM */
		rect.x = tim.cx;
		rect.y = tim.cy;

		/* Width and Height of CLUT */
		rect.w = tim.cw;
		rect.h = tim.ch;

		/* Load CLUT data to VRAM */
		LoadImage(&rect,tim.clut);
	}
}



	// handle the controller pad
static int pad_read(GsCOORDINATE2 *objWorld,SVECTOR *rotate,int *frame)
{
	u_long		padd;
	static u_long	padd_old;

		// rotation speed
	static int	rot_speed[2]={4*ONE/360,2*ONE/360};
		// translation i.e. simple movement speed
	static int	tra_speed[2]={20,10};

	padd = PadRead(0);

	/* end */
	if ( padd & PADselect )
		return(-1);

	/* change speed */
	if ( padd & PADstart ) {
		if ( padd != padd_old )
			*frame ^= 1;
	}

	/* rotate Y */
	if ( padd & PADRleft  ) rotate->vy -= rot_speed[*frame];
	if ( padd & PADRright ) rotate->vy += rot_speed[*frame];

	/* rotate X */
	if ( padd & PADRup    ) rotate->vx += rot_speed[*frame];
	if ( padd & PADRdown  ) rotate->vx -= rot_speed[*frame];

	/* translate Z */
	if ( padd & PADL1 ) {
		if ( objWorld->coord.t[2] < 500 )
			objWorld->coord.t[2] += tra_speed[*frame];
	}
	if ( padd & PADL2 ) objWorld->coord.t[2] -= tra_speed[*frame];

	/* translate Y */
	if ( padd & PADLdown ) objWorld->coord.t[1] += tra_speed[*frame];
	if ( padd & PADLup   ) objWorld->coord.t[1] -= tra_speed[*frame];

	/* translate X */
	if ( padd & PADLleft  ) objWorld->coord.t[0] += tra_speed[*frame];
	if ( padd & PADLright ) objWorld->coord.t[0] -= tra_speed[*frame];

	rotate->vy += rot_speed[*frame]/2;

	padd_old = padd;

	return(0);
}



	// opening sequence: car moves from the distance to the foreground
static void opening(GsCOORDINATE2 *objWorld,SVECTOR *rotate,DB *db,
			GsDOBJ2 *object,int objcnt)
{
	int	idx;
	int	i;

	objWorld->coord.t[0] = -930;
	objWorld->coord.t[1] = 60;
	objWorld->coord.t[2] = -6050;
	rotate->vy = +110;

	for ( i=0; i<470; i++ ) {

		objWorld->coord.t[0] += 2;
		objWorld->coord.t[2] += (1+i*.05);

		move_object(objWorld,rotate);
		idx = GsGetActiveBuff();
		draw_object(&db[idx],object,objcnt,FRAME_60);

	}
}


static void move_object(GsCOORDINATE2 *objWorld,SVECTOR *rotate)
{
	MATRIX		tmpmat;
	SVECTOR		tmpvec;

	/* start from unit matrix */
	tmpmat = GsIDMATRIX;

	/* Set Translate */
	tmpmat.t[0] = objWorld->coord.t[0];
	tmpmat.t[1] = objWorld->coord.t[1];
	tmpmat.t[2] = objWorld->coord.t[2];

	/* set rotate */
	tmpvec = *rotate;

	/* rotate matrix */
	RotMatrix(&tmpvec,&tmpmat);

	/* set Matrix to coordinate */
	objWorld->coord = tmpmat;

	/* clear flag because of changing parameter */
	objWorld->flg = 0;
}

static void draw_object(DB *db,GsDOBJ2 *object,int objcnt,int frame)
{
	GsDOBJ2		*op;
	MATRIX		tmpmat;

	/* Set top address of Packet Area for output of GPU PACKETS */
	GsSetWorkBase(db->packet);

	/* Clear OT for using buffer */
	GsClearOt(0,0,&db->gsot);

	op = object;
	while ( objcnt-- ) {

		/* Perspective Translate Object and Set OT */
			// find local world matrix, set light matrix by that,
			// get local screen matrix, set local screen matrix
		GsGetLw(op->coord2,&tmpmat);
		GsSetLightMatrix(&tmpmat);
		GsGetLs(op->coord2,&tmpmat);
		GsSetLsMatrix(&tmpmat);
		GsSortObject4(op,&db->gsot,14-OT_LENGTH,getScratchAddr(0));

		op++;
	}

	DrawSync(0);		// wait for end of drawing

		// deal with different frame rates
	switch (frame) {
	case FRAME_30:		/* 30 frame */
		VSync(2);
		break;
	case FRAME_60:		/* 60 frame */
		VSync(0);
		break;
	}

	FntPrint("speed = %d frame/sec\n",30*(frame+1));

	FntFlush(-1);

	/* Swap double buffer */
	GsSwapDispBuff();

	/* Set SCREEN CLESR PACKET to top of OT */
	GsSortClear(0x0,0x0,0x0,&db->gsot);

	/* Drawing OT */
	GsDrawOt(&db->gsot);
}



	// controller pad:
	// read status into unsigned long for easier and clearer access
static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

