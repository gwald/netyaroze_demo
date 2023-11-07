/*
 * $PSLibId: Runtime Library Version 3.5$
 */
/*
 *	Oden: light animation program
 *
 *	"oden.c" ******** Demonstration of Lighting Caliculation
 *
 *	Version 1.**	Oct,  22, 1993 (YUTAKA)
 *      Version 2.00    Dec,  27, 1994 (YUTAKA)
 *	
 *	Adapted to black psx 7/10/96	LPGE
 *
 *	Copyright (C) 1993,1994,1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */

#include <libps.h>
#include "pad.h"




#define PACKETMAX 2000
#define OBJECTMAX 10
#define PACKETMAX2 (PACKETMAX*64)
#define MODEL_ADDR 0x80140000
#define TEX_ADDR   0x80150000
#define OT_LENGTH  8
u_long padd;

GsOT            Wot[2];			 /* WORLD Ordering table TAG */
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* World ordering table */
GsDOBJ2		object[OBJECTMAX];	 /* object */
u_long          Objnum;
GsCOORDINATE2   DWorld,DCube,DCone,DBall;
GsFOGPARAM      FOG1;
int             Am;

/*============================*/
GsRVIEW2  view;
GsF_LIGHT pslt[3];
RECT  psdisp;

PACKET		out_packet[2][PACKETMAX2];  /* GTE output */
SVECTOR         PWorld; /* work short vector for making Coordinate parmeter */
                   

extern MATRIX GsIDMATRIX,GsWSMATRIX;
VECTOR ModelScale[OBJECTMAX];



	// buffers for the controller pad
volatile u_char *bb0, *bb1;



void main(void);
void draw_all(u_long on);
int obj_interactive(void);
void light_set(u_long padd, int n);
void ambient_set(u_long padd);
void modeling_data_read(void);
void init_all(void);
void view_init(void);
void light_init(void);
void texture_init(u_long addr);
void set_coordinate(SVECTOR* pos, GsCOORDINATE2* coor);
static u_long PadRead(long id);

/*============================*/
void main(void)
{
  int    fn;
  
  init_all();
  modeling_data_read();
  
  for(fn=0;;fn++)
    {
      if(obj_interactive())	
      	break;
      draw_all(Objnum);
    }

  ResetGraph(3);
}


void draw_all(u_long on)	/* draw all objects */
{
  int     i;
  GsDOBJ2  *op;
  int    outbuf_idx;
  MATRIX tmpm;
  VECTOR v;
  
  outbuf_idx=GsGetActiveBuff();
  /* rewind out packet pointer */
  GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
  GsClearOt(0,0,&Wot[outbuf_idx]);
  
  for(i=0,op=object;i<on;i++,op++)
    {
      GsGetLw(op->coord2,&tmpm);	/* local-world matrix */
      
      v = ModelScale[i];	/* light matrix */
      ScaleMatrix(&tmpm,&v);	
      GsSetLightMatrix(&tmpm);
      
      GsGetLs(op->coord2,&tmpm); /* local-screen matrix */
      GsSetLsMatrix(&tmpm);
      
      GsSortObject4(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
    }
  
  VSync(0);						/* wait for the next V-BLANK */
  padd = PadRead(1);
  ResetGraph(1);		   // clear drawing
  GsSwapDispBuff();		   // swap buffers
  GsSortClear(0, 0, 0, &Wot[outbuf_idx]);	// register clearing command
  GsDrawOt(&Wot[outbuf_idx]);
}


int obj_interactive(void)
{
  u_long i,mode_f;
  GsDOBJ2   *objp;
  
  objp=object;
  mode_f = 0;
  
  if(padd & PADn)	   
    {
      light_set(padd,0);
      mode_f = 1;
    }
  if(padd & PADo)	  
    {
      light_set(padd,1);
      mode_f = 1;
    }
  if(padd & PADm)		
    {
      light_set(padd,2);
      mode_f = 1;
    }
  if(padd & PADl)		
    {
      ambient_set(padd);
      mode_f = 1;
    }
  
  if(mode_f == 1)
    return 0;

  if((padd & PADLleft)>0)
    {				/* ball */
      object[0].attribute = GsDOFF;
      object[3].attribute = 0;
    }
  else if((padd & PADLright)>0)
    {				/* cube */
      object[2].attribute = GsDOFF|GsDIV2;
      object[5].attribute = 0|GsDIV2;
    }
  else if((padd & PADLup)>0)
    {				/* cone */
      object[1].attribute = GsDOFF;
      object[4].attribute = 0;
    }
  else if((padd & PADLdown)>0)
    {
      object[0].attribute = 0;
      object[3].attribute = GsDOFF;
      object[2].attribute = 0|GsDIV2;
      object[5].attribute = GsDOFF|GsDIV2;
      object[1].attribute = 0;
      object[4].attribute = GsDOFF;
    }
  else
    {
      for(i=0;i<1;i++)
	{
	  if((padd & PADRleft)>0) PWorld.vy -=5*ONE/360;
	  if((padd & PADRright)>0) PWorld.vy +=5*ONE/360;
	  if((padd & PADRup)>0) PWorld.vx+=5*ONE/360;
	  if((padd & PADRdown)>0) PWorld.vx-=5*ONE/360;
	  
	  if((padd & PADh)>0 && (DWorld.coord.t[2] > -2800))
	    {
	      DWorld.coord.t[2] -= 80;
	    }
	  if((padd & PADk)>0)
	    {
	      DWorld.coord.t[2] += 80;
	    }
	  
	  if((padd & PADk)>0 && (padd & PADh)>0) return -1;
	  objp++;
	}
      set_coordinate(&PWorld,&DWorld);
    }
    
    return 0;
}




void light_set(u_long padd, int n)
{
  if((padd & PADRup) && (pslt[n].r < 0xfe))
    pslt[n].r+=2;
  if((padd & PADRdown) && (pslt[n].r > 0))
    pslt[n].r-=2;
  
  if((padd & PADRright) && (pslt[n].g < 0xfe))
    pslt[n].g+=2;
  if((padd & PADRleft) && (pslt[n].g > 0))
    pslt[n].g-=2;
  
  if((padd & PADk) && (pslt[n].b < 0xfe))
    pslt[n].b+=2;
  if((padd & PADh) && (pslt[n].b > 0))
    pslt[n].b-=2;
  
  if((padd & PADLup) && (pslt[n].vy < 300))
    pslt[n].vy+=10;
  if((padd & PADLdown) && (pslt[n].vy > -300))
    pslt[n].vy-=10;
  
  if((padd & PADLleft) && (pslt[n].vx < 300))
    pslt[n].vx+=10;
  if((padd & PADLright) && (pslt[n].vx > -300))
    pslt[n].vx-=10;
  
  GsSetFlatLight(n,&pslt[n]);
}


void ambient_set(u_long padd)
{
  if((padd & PADRup) && (Am<2*ONE))
    Am+=20;
  if((padd & PADRdown) && (Am>20))
    Am-=20;
  GsSetAmbient(Am,Am,Am);
}


void modeling_data_read(void)
{
  u_long size,*dop;
  GsDOBJ2 *objp;
  VECTOR  v;
  int i;
  
  dop=(u_long *)MODEL_ADDR;
  dop++;			/* file header increment */
  
  GsMapModelingData(dop);
  dop++;
  Objnum = *dop;		/* header read */
  dop++;			/* point top */
  
  for(i=0;i<Objnum;i++)
    GsLinkObject4((u_long)dop,&object[i],i);
  
  GsInitCoordinate2(WORLD,&DWorld);
  DWorld.coord.t[2] = 1000;
  GsInitCoordinate2(&DWorld,&DCube);
  GsInitCoordinate2(&DWorld,&DCone);
  GsInitCoordinate2(&DWorld,&DBall);
  for(i=0,objp=object;i<Objnum;i++)
    {
      objp->coord2 = &DWorld;
      objp++;
    }
  
  /* Init work vector */
  PWorld.vx=PWorld.vy=PWorld.vz=0;

  object[0].attribute = 0x00000000;
  object[0].coord2 = &DBall; /* ball */
  object[0].coord2->coord.t[0] = -400*2;
  object[0].coord2->coord.t[1] = -140*2;
  v.vx = 4*2*ONE/5;
  v.vy = 4*2*ONE/5;
  v.vz = 4*2*ONE/5;
  
  ScaleMatrix(&DBall.coord,&v);
  ModelScale[0].vx = ModelScale[0].vy = ModelScale[0].vz = 5*ONE/4/2;

  
  object[1].attribute = 0x00000000;
  object[1].coord2 = &DCone; /* cone */
  object[1].coord2->coord.t[0] = 0;
  v.vx = 2*ONE*4/3;
  v.vy = 2*ONE;
  v.vz = 2*ONE*4/3;
  ScaleMatrix(&DCone.coord,&v);
  ModelScale[1].vx = ModelScale[1].vz = 3*ONE/4/2;
  ModelScale[1].vy = ONE/2;

  object[2].attribute = GsDIV2;
  object[2].coord2 = &DCube; /* cube */
  DCube.coord.t[0] = 300*2;
  DCube.coord.t[2] = -160*2;
  v.vx = 2*ONE*5/2;
  v.vy = 2*ONE*5/2;
  v.vz = 2*ONE*5/2;
  ScaleMatrix(&DCube.coord,&v);
  ModelScale[2].vx = ModelScale[2].vy = ModelScale[2].vz = 2*ONE/5/2;
  
  object[3].attribute = GsDOFF;
  object[3].coord2 = &DBall;
  ModelScale[3].vx = ModelScale[3].vy = ModelScale[3].vz = 5*ONE/4/2;
  object[4].attribute = GsDOFF;
  object[4].coord2 = &DCone;
  ModelScale[4].vx = ModelScale[4].vz = 3*ONE/4/2;
  ModelScale[4].vy = ONE/2;
  object[5].attribute = GsDOFF|GsDIV2;
  object[5].coord2 = &DCube;
  ModelScale[5].vx = ModelScale[5].vy = ModelScale[5].vz = 2*ONE/5/2;
}

void init_all(void)
{
	// set up controller pad buffers
  GetPadBuf(&bb0, &bb1);	
  padd = 0;
  
	// set screen resolution and Interlace mode
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  GsDefDispBuff(0,0,0,0);
  
  Wot[0].length=OT_LENGTH;
  Wot[0].tag=Wot[0].org=zsorttable[0];	/* World Ordering table */
  Wot[1].length=OT_LENGTH;
  Wot[1].tag=Wot[1].org=zsorttable[1];
  
  GsInit3D();
  view_init();
  light_init();
  
  texture_init(TEX_ADDR);
  
  GsSetLightMode(0);
  
  Am = 0;
  GsSetAmbient(Am,Am,Am);
}


void view_init(void)
{
  /*---- Set projection,view ----*/
  
  GsSetProjection(1000);	/* set projection <h> */
  view.vpx = 0; view.vpy = -75; view.vpz = -5000;
  view.vrx = 0; view.vry = -75; view.vrz = 0;
  view.rz=0;
  
  view.super = WORLD;
  GsSetRefView2(&view);	/* set view */
}


void light_init(void)
{
  /*---- Set light ----*/
  pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= 100;
  pslt[0].r=0x80; pslt[0].g=0x80; pslt[0].b=0x80;
  GsSetFlatLight(0,&pslt[0]);
  
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
  pslt[2].r=0x80; pslt[2].g=0x80; pslt[2].b=0x80;
  GsSetFlatLight(2,&pslt[2]);
  
  /*---- Set ambient ---*/
  
  GsSetAmbient(Am,Am,Am);
  
  /*---- Set Fog parm --*/
  
  FOG1.dqa = -10000*ONE/64/1000;
  FOG1.dqb = 5/4*ONE*ONE;
  FOG1.rfc = FOG1.gfc = FOG1.bfc = 0x00;
  GsSetFogParam(&FOG1);
}


	// load texture into frame buffer
void texture_init(u_long addr)
{
  RECT rect1;		   // destination on frame buffer
  GsIMAGE tim1;		   // info on TIM picture
  
  GsGetTimInfo((u_long *)(addr+4),&tim1);
  rect1.x=tim1.px;
  rect1.y=tim1.py;
  rect1.w=tim1.pw;
  rect1.h=tim1.ph;
  LoadImage(&rect1,tim1.pixel);
  if((tim1.pmode>>3)&0x01)	/* if clut exists, load it up */
    {
      rect1.x=tim1.cx;
      rect1.y=tim1.cy;
      rect1.w=tim1.cw;
      rect1.h=tim1.ch;
      LoadImage(&rect1,tim1.clut);
    }
}


/* Set coordinte parameter from work vector	*/
void set_coordinate(SVECTOR* pos, GsCOORDINATE2* coor)
{
  MATRIX tmp1;
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;		/* start from unit matrix */
    
  /* Set translation */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  v1 = *pos;
  
  /* Rotate Matrix */
  RotMatrix(&v1,&tmp1);
  
  /* Set Matrix to Coordinate */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter */
  coor->flg = 0;
}



static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

