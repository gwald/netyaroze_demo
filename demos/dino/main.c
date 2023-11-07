/*
 *		"main.c"
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 */


#include <libps.h>
#include "pad.h"
#include "addr.h"
#include "mime.h"  
#include "control.h"  

extern int mimepr[MIMEMAX];

GsIMAGE TexInfo;
RECT BgPos;


#define OBJECTMAX	100	

#define	TIM_HEADER	0x00000010	
#define OT_LENGTH	10		
#define PACKETMAX	3000


GsOT            Wot[2];		
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; 

GsDOBJ2		object[OBJECTMAX]; 

u_long          Objnum;		

GsFOGPARAM      FOG1;
GsCOORDINATE2   DWorld;  
SVECTOR         PWorld; 
extern MATRIX GsIDMATRIX;  

GsRVIEW2  view;			
GsF_LIGHT pslt[3];		


PACKET *PacketArea = (PACKET *)STD0_VB_ADDR;

/*=== for Transfer Function  ===*/
extern int cntrlarry[CTLMAX][CTLTIME]; 
extern CTLFUNC ctlfc[CTLMAX];

/* Transfer Function Wave */
static int cnv0[16] = {
	  39,  79, 118, 197, 315, 394, 433, 473,
	 473, 433, 394, 315, 197, 118,  79,  39
};

static int cnv1[8] = {
	 365, 728, 910, 819, 637, 364, 182,  91
};

static int cnv2[12] = {
	 400, 900,1000, 900, 600, 400, 200, 110,
	-100,-200, -80, -20
};

static int cnv3[24] = {
	 200, 325, 450, 475, 500, 475, 450, 375,
	 300, 250, 200, 150, 100,  80,  55,   0,
	 -50, -75,-100, -70, -40, -25, -10,  -5
};

static int cnv4[32] = {
	  20,  30,  40,  50,  59,  79,  98, 128,
	 157, 177, 197, 206, 216, 226, 236, 238,
	 236, 226, 216, 206, 197, 177, 157, 128,
	  98,  79,  59,  50,  40,  30,  20,  10
};

static int cnv5[4] = {
	 512, 1536, 1536, 512, 
};

static int blinktable[120] = {
	   0,1024,2048,3072,3072,3072,3072,2048,2048,2048,
	1024,1024,1024, 768, 768, 512, 256, 128,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,2048,4096,4096,4096,4096,4096,
	3072,2048,1024,   0,   0,   0,2048,4096,2048,   0,
	   0,   0,   0,   0, 512,4096,3072,2048,1024, 512,
	   0,   0,   0, 512,4096,3072,2048,1024, 512,   0,   
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0, 512,1024,2048,2048,2048,2048,2048,
	1024, 768, 512, 256,   0,   0,   0,   0,   0,   0
};

static int lf[16] = { 1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0};
static int rf[16] = { 0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0};
static int ln[16] = { 0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0};
static int rn[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0};

int alpha;
int modeflag;
int modeflagex;
u_long fn;

/******************************/


u_long padd;			
volatile u_char *bb0, *bb1;


#define SFILE	3

typedef struct{
	char	*fname;
	void	*addr;
	CdlFILE finfo;
} FILE_INFO;

static FILE_INFO sfile[SFILE] = {
	{ "\\DATA\\SOUND\\SAMPLE1.SEQ;1",(void *)SAMPLE1_SEQ_ADDR,0 },
	{ "\\DATA\\SOUND\\STD0.VH;1",(void *)STD0_VH_ADDR,0 },
	{ "\\DATA\\SOUND\\STD0.VB;1",(void *)STD0_VB_ADDR,0 }
};


static void init_all();			
static void init_all2();	   
static void view_init();	   
static void light_init();	   
static void coord_init();	   
static void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor);
					
static int pick(u_long x);	  
static void initTexture1();		
static void initTexture2();	   
static void model_init();		
static int obj_interactive();	   
static u_long PadRead(long id);		
static void datafile_search();		
static void datafile_read();		


extern void snd_set(u_long *seq_a, u_char *vab_b, u_char *vab_h);
					
extern void snd_play();			
extern void snd_stop();			
extern void set_tmd_data();            
extern void set_vertex_mime_data();    
extern void vertex_mime();            
extern void init_cntrlarry(int *cnvwave, int number);
                        
extern void set_cntrl(u_long frm);
                        


main()
{
  	int     i;
  	GsDOBJ2 *op;		
   	int     outbuf_idx;
  	MATRIX  tmpls;

  	long		ph1,ph2,dmy;

	GetPadBuf(&bb0, &bb1);	

  	datafile_search();			    
  	datafile_read();

  	snd_set(sfile[0].addr, sfile[1].addr, sfile[2].addr);
					
	modeflag = 1;
	modeflagex = 0;
	alpha = 32;

  	init_all();

  	initTexture1();
  	initTexture2();

  	init_all2();

	/*== MIME Function Initialize =====*/
  	set_tmd_data();
  	set_vertex_mime_data();
	for(i = 0 ; i < MIMEMAX ; i++) mimepr[i] = 0;

	/*== Control Transfer Function Initialize ====*/
	init_cntrlarry(cnv0,16);

	ctlfc[4].num=24;
	ctlfc[4].cnv=cnv3;
	ctlfc[5].num=24;
	ctlfc[5].cnv=cnv3;
	ctlfc[6].num=24;
	ctlfc[6].cnv=cnv3;
	ctlfc[7].num=24;
	ctlfc[7].cnv=cnv3;
	ctlfc[8].num=24;
	ctlfc[8].cnv=cnv3;
	ctlfc[9].num=12;
	ctlfc[9].cnv=cnv2;
	ctlfc[10].num=24;
	ctlfc[10].cnv=cnv3;
	ctlfc[11].num=24;
	ctlfc[11].cnv=cnv3;
	ctlfc[12].num=24;
	ctlfc[12].cnv=cnv3;

	snd_play(sfile[0].addr,sfile[1].addr,sfile[2].addr);

    SetVideoMode(MODE_PAL);
    GsDISPENV.screen.y += 27;

  	for (fn=0;;fn++) {
        	if (obj_interactive() < 0) break;

        	set_cntrl(fn);
		PWorld.vy += ctlfc[4].out;
		PWorld.vx += ctlfc[5].out;
		DWorld.coord.t[2] += ctlfc[8].out;
		DWorld.coord.t[0] += ctlfc[6].out;		
		DWorld.coord.t[1] += ctlfc[7].out;		
        	if(modeflag != 0){
			ph2 = fn & 0xffff;
			ph1 = ((ph2*alpha)>>6)%16;
			ctlfc[0].in = lf[ph1]*4096;
			ctlfc[1].in = rf[ph1]*4096;
			ctlfc[2].in = ln[ph1]*6000;
			ctlfc[3].in = rn[ph1]*6000;
		}
	 	mimepr[0] = ctlfc[0].out;
		mimepr[1] = ctlfc[1].out;
		mimepr[2] = ctlfc[2].out;
		mimepr[3] = ctlfc[3].out;
		mimepr[4] = ctlfc[9].out;
		mimepr[5] = ctlfc[10].out;
		mimepr[6] = ctlfc[11].out;
		mimepr[7] = pick(fn);
		mimepr[8] = ctlfc[12].out;
		vertex_mime();
	
	      	GsSetRefView2(&view);	

	      	outbuf_idx=GsGetActiveBuff();
						
	        GsSetWorkBase((PACKET *)(PacketArea+outbuf_idx*PACKETMAX*24));
			 	   
	      	GsClearOt(0,0,&Wot[outbuf_idx]);
	
	      	for(i=0,op=object;i<Objnum;i++) {
		  	
		  	GsGetLs(op->coord2,&tmpls);
	                
		  	GsSetLightMatrix(&tmpls);
	          	
		  	GsSetLsMatrix(&tmpls);
	          	
		  	GsSortObject4(op,&Wot[outbuf_idx],14-OT_LENGTH,(u_long *)0x1f800000);
	          	op++;
		}
	      	padd=PadRead(1);		      
	     	VSync(0);			
	
/*		DrawSync(0);		*/	
		ResetGraph(1);			
	      GsSwapDispBuff();			
	      	
	     
	 	BgPos.y = (1-outbuf_idx)*240;    			
	  	LoadImage(&BgPos, TexInfo.pixel);    		   
		DrawSync(0);
	
	      GsDrawOt(&Wot[outbuf_idx]);
	}	

	snd_stop();		
	return(0);	   
}


static int obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	if((padd & PADRleft)>0) 	 ctlfc[4].in =  6*ONE/360;
	else {  if((padd & PADRright)>0) ctlfc[4].in = -6*ONE/360; 
		else 			 ctlfc[4].in =      0; 
	}

	if((padd & PADRup)>0) 		 ctlfc[5].in =  6*ONE/360;
	else { 	if((padd & PADRdown)>0)  ctlfc[5].in = -6*ONE/360;
		else			 ctlfc[5].in =      0; 
	}

	if((padd & PADh)>0)		 ctlfc[8].in = -300;
	else {  if((padd & PADk)>0)	 ctlfc[8].in =  300; 
		else			 ctlfc[8].in =   0; 
	}

	if((padd & PADLleft)>0) 	 ctlfc[6].in =  200;
	else {  if((padd & PADLright)>0) ctlfc[6].in = -200; 
		else			 ctlfc[6].in =   0; 
	}

	if((padd & PADLup)>0)		 ctlfc[7].in = -200;
	else {  if((padd & PADLdown)>0)	 ctlfc[7].in =  200;
		else			 ctlfc[7].in =   0; 
	}
  
	if( ((padd & PADl)>0)&&((padd & PADn)>0)&&((padd & PADk)>0) ){
		modeflag = 0;
		ctlfc[0].in = 0;
		ctlfc[1].in = 0;
		ctlfc[2].in = 0;
		ctlfc[3].in = 0;
	}

	if( ((padd & PADl)>0)&&((padd & PADn)>0)&&((padd & PADh)>0) ){
		modeflag = 1;
		ctlfc[9].in = 0;
		ctlfc[10].in = 0;
		ctlfc[11].in = 0;
		ctlfc[12].in = 0;  
	}

	if( ((padd & PADl)>0)&&((padd & PADn)>0)&&((padd & PADLup)>0) ){
		modeflagex = 0;
		ctlfc[0].in = 0;
		ctlfc[1].in = 0;
		ctlfc[2].in = 0;
		ctlfc[3].in = 0;
	}

	if( ((padd & PADl)>0)&&((padd & PADn)>0)&&((padd & PADLdown)>0) ){
		modeflagex = 1;
		ctlfc[9].in = 0;
		ctlfc[10].in = 0;
		ctlfc[11].in = 0;
		ctlfc[12].in = 0;  
	}

	if(modeflag == 0){
		if(modeflagex == 1){		
			if((padd & PADl)>0) 	ctlfc[0].in = ONE; 
			else			ctlfc[0].in = 0;
		  	if((padd & PADm)>0) 	ctlfc[1].in = ONE;
			else			ctlfc[1].in = 0;
		  	if((padd & PADn)>0) 	ctlfc[2].in = ONE;
			else			ctlfc[2].in = 0;
		  	if((padd & PADo)>0) 	ctlfc[3].in = ONE;
			else			ctlfc[3].in = 0;
		} else{
		  	if((padd & PADm)>0) 	ctlfc[9].in = ONE;
			else			ctlfc[9].in = 0;
			if((padd & PADo)>0) 	ctlfc[10].in = ONE;
			else			ctlfc[10].in = 0;
			if((padd & PADl)>0) 	ctlfc[11].in = 6000;
			else			ctlfc[11].in = 0;
			if((padd & PADn)>0) 	ctlfc[0].in = ONE;
			else			{ ctlfc[0].in = 0;
						ctlfc[12].in = 0; }  
		}
	} else {
		if(modeflagex == 1){		
			if((padd & PADl)>0) alpha++;
			if((padd & PADm)>0) alpha--;
			if((padd & PADn)>0) alpha = 32;
		} else {
			if((padd & PADm)>0) 	ctlfc[9].in = 6000;
			else			ctlfc[9].in = 0;
			if((padd & PADo)>0) 	ctlfc[10].in = ONE;
			else			ctlfc[10].in = 0;
			if((padd & PADl)>0) 	ctlfc[11].in = 6000;
			else			ctlfc[11].in = 0;
			if((padd & PADn)>0) 	ctlfc[12].in = 6000;
			else			ctlfc[12].in = 0;  
		}
	}

  	if( ((padd & PADh)>0) && ((padd & PADk)>0) ){
                return(-1);
	}
 
  	set_coordinate(&PWorld,&DWorld);
   
	return(0);
}


static void init_all()
{
	short i;

	ResetGraph(0);		  
	GsInitGraph(640,240,0,1,0);	
	GsDefDispBuff(0,0,0,240);  
	GsInit3D();		 
	padd=0;			  
	
	for (i = 0; i < 2; i++) {
		Wot[i].length=OT_LENGTH;
		Wot[i].org=zsorttable[i];
	}
}

static void init_all2()
{
	coord_init();			
	model_init();			  
	view_init();		  
	light_init();		   
}			


static void view_init()
{
	GsSetProjection(1000);	
  
  	view.vpx = 0; view.vpy = 0; view.vpz = 2000;

  	view.vrx = 0; view.vry = 0; view.vrz = 0;

  	view.rz=0;
  	view.super = WORLD;	   
  
   	GsSetRefView2(&view);
}


static void light_init()
{
	pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= 100;

    pslt[0].r=0xf0; pslt[0].g=0xf0; pslt[0].b=0xf0;

	GsSetFlatLight(0,&pslt[0]);

	pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
	pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
	GsSetFlatLight(1,&pslt[1]);
	
	pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
	pslt[2].r=0x80; pslt[2].g=0x80; pslt[2].b=0x80;
	GsSetFlatLight(2,&pslt[2]);
	
	GsSetAmbient(ONE/4,ONE/4,ONE/4);

	GsSetLightMode(1);

	FOG1.dqa = -5*12000*64/600;
	FOG1.dqb = 5/4*ONE*ONE;
	FOG1.rfc = FOG1.gfc = FOG1.bfc = 0x00;
	GsSetFogParam(&FOG1);
}


static void coord_init()
{
   	GsInitCoordinate2(WORLD, &DWorld);

	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	DWorld.coord.t[2] = -30000;
}


static void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
	MATRIX tmp1;
	SVECTOR v1;
	
	tmp1	 = GsIDMATRIX;		

	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	v1 = *pos;

	RotMatrix(&v1,&tmp1);

	coor->coord = tmp1;

	coor->flg = 0;
}    

static int pick(u_long x)
{
	return(blinktable[x % 120]);
}


static void initTexture1()
{
	u_long	*timP;
	GsIMAGE tinfo;
	
	timP = (u_long *)DINONEW_TIM_ADDR;
	while (1) {
		if (*timP != TIM_HEADER) {
			break;
		}
		timP++;

		GsGetTimInfo(timP, &tinfo);
					
		printf("PIX: %d %d	", tinfo.px, tinfo.py);
		printf("MODE: %d	", tinfo.pmode);

		timP += tinfo.pw * tinfo.ph/2+3+1;
		LoadImage((RECT *)&tinfo.px, tinfo.pixel);

	  	if((tinfo.pmode>>3)&0x01) {
			printf("CLUT: %d %d", tinfo.cx, tinfo.cy);
			LoadImage((RECT *)&tinfo.cx, tinfo.clut);
						
			timP += tinfo.cw * tinfo.ch/2+3;
		}
		printf("\n");
	}
}


static void initTexture2()
{
	GsIMAGE *tinfo;
	
	tinfo = &TexInfo;
	GsGetTimInfo((u_long *)CASTLE_TIM_ADDR+1, tinfo);
				
	printf("PIX: %d %d	", tinfo->px, tinfo->py);
	printf("MODE: %d	", tinfo->pmode);

	LoadImage((RECT *)&tinfo->px, tinfo->pixel);
				
	if((tinfo->pmode>>3)&0x01) {
		printf("CLUT: %d %d", tinfo->cx, tinfo->cy);
		LoadImage((RECT *)&tinfo->cx, tinfo->clut);
	}
	printf("\n");

	BgPos.x = 0;
	BgPos.y = 0;
	BgPos.w = TexInfo.pw;
	BgPos.h = 240;
}


static void model_init()
{			
	u_long *dop;
	GsDOBJ2 *objp;		
	int i;
	
	dop=(u_long *)MD0_TMD_ADDR;	
	dop++;			/* header skip */
	
	GsMapModelingData(dop);	
					
	dop++;
	Objnum = *dop;		
	dop++;			 
	for(i=0;i<Objnum;i++)		
		GsLinkObject4((u_long)dop,&object[i],i);
	
	for(i=0, objp=object; i<Objnum; i++) {
		objp->coord2 =	&DWorld;
				
			
		objp->attribute = 0;
/*		objp->attribute = GsLOFF;	 */
			
		objp++;
	}
}


static void datafile_search()
{
	int i, j;

	for (i = 0; i < SFILE; i++) {	   
		for (j = 0; j < 10; j++) {	
			if (CdSearchFile(&(sfile[i].finfo), sfile[i].fname) != 0)
				break;		
			else
				printf("%s not found.\n", sfile[i].fname);
		}
	}
}


static void datafile_read()
{
	int i, j;
	int cnt;

	for (i = 0; i < SFILE; i++) {		
		for (j = 0; j < 10; j++) {	
			CdReadFile(sfile[i].fname, sfile[i].addr,sfile[i].finfo.size);

		  
			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	

			if (cnt == 0)
				break;	
		}
	}
}


static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}


