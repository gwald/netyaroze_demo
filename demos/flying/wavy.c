//
//	"wave3d" 
//	main.c
//
//
// Copyright (c) 1996 by Sony Computer Entertainment Inc.
// All Rights Reserved.
//
#include <libps.h>
#include "pad.h"
#include "sincos.h"

#define PACKETMAX		(10000)
#define OBJECTMAX		(100)
#define PACKETBUFMAX	(PACKETMAX*24)

#define PACKETMAX2		(256)
#define PACKETBUFMAX2	(PACKETMAX2*24)

#define TEX_ADDR		(0x80010000)
#define MODEL_ADDR		(0x800c0000)

#define OT_LENGTH		(10)
#define OT_LENGTH2		(1)

GsOT		Wot[2];							
GsOT_TAG	zsorttable[2][1<<OT_LENGTH];	
PACKET		out_packet[2][PACKETBUFMAX];	
GsOT		Wot2[2];					  
GsOT_TAG	zsorttable2[2][1<<OT_LENGTH2];	
PACKET		out_packet2[2][PACKETBUFMAX2]; 

GsDOBJ2		object[OBJECTMAX];				
u_long		Objnum;							

GsBOXF		tex_clear;						
GsSPRITE	back[240];					   
int			back_wave_job;				   
int			back_wave_amp;
int			back_wave_ph;
int			back_wave_timer;

GsCOORDINATE2	DCube;
SVECTOR			PWorld, PCube;

GsRVIEW2	view;					   
GsF_LIGHT	pslt[3];						

volatile u_char	*bb0, *bb1;					
u_long	padd;								

int		outbuf_idx;


u_long	PadRead(long id);
int		obj_interactive(void);
void	init_all(void);
void	view_init(void);
void	light_init(void);
void	coord_init(void);
void	set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor);
void	texture_init(u_long addr);
void	model_init(void);
void	init_back_wave(void);
void	sort_back_wave(void);
void	back_wave(int *job);



int main(void)
{
	int		i;
	GsDOBJ2	*op;
	MATRIX	tmpls;
	int		cpu_load, gpu_load;
	RECT	tmp_clip;
	short	tmp_ofs[2];

	init_all();

	FntLoad(960, 256);	
	FntOpen(-96, -96, 192, 192, 0, 512);	

	GetPadBuf(&bb0, &bb1);	

	while (1) {
		if (obj_interactive() == 0) {
			return (0);
		}

		GsSetRefView2(&view);	

		outbuf_idx=GsGetActiveBuff();  
		GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	

		for (i = 0, op = object; i < 1/*Objnum*/; i++) {
			
			GsGetLs(op->coord2,&tmpls);
			
			GsSetLightMatrix(&tmpls);
			
			GsSetLsMatrix(&tmpls);
			
			GsSortObject4(op, &Wot[outbuf_idx], 14-OT_LENGTH,
					(u_long *)getScratchAddr(0));
			op++;
		}

		padd = PadRead(0);		
		cpu_load = VSync(1);	
		DrawSync(0);		   
		gpu_load = VSync(1);	
		VSync(0);			   
		GsSwapDispBuff();		
		tmp_clip = GsDRAWENV.clip; 
		tmp_ofs[0] = GsDRAWENV.ofs[0];	
		tmp_ofs[1] = GsDRAWENV.ofs[1];
		setRECT(&GsDRAWENV.clip, 256, 0, 256, 240);
		GsDRAWENV.ofs[0] = 256+128;
		GsDRAWENV.ofs[1] = 0+120;
		PutDrawEnv(&GsDRAWENV);		
		
		GsSortBoxFill(&tex_clear, &Wot[outbuf_idx], 1<<OT_LENGTH -1);
		
		GsDrawOt(&Wot[outbuf_idx]);

		GsSetWorkBase((PACKET*)out_packet2[outbuf_idx]);
		GsClearOt(0, 0, &Wot2[outbuf_idx]);	
		
		back_wave(&back_wave_job);
		GsDRAWENV.clip = tmp_clip;	
		GsDRAWENV.ofs[0] = tmp_ofs[0];	
		GsDRAWENV.ofs[1] = tmp_ofs[1];
		PutDrawEnv(&GsDRAWENV);		
		
		GsSortClear(0x0,0x0,0x0,&Wot2[outbuf_idx]);
		DrawSync(0);		   
		
		GsDrawOt(&Wot2[outbuf_idx]);

		FntPrint("CPU LOAD : %d\n", cpu_load);
		FntPrint("GPU LOAD : %d\n", gpu_load);
		FntFlush(-1);
    }
}



void init_back_wave(void)
{
	int		i;
	GsSPRITE	*sp = &back[0];

	for (i = 0; i < 240; i++, sp++) {
		sp->attribute = (1<<25);
		sp->y = -120+i;
		sp->w = 256;
		sp->h = 1;
		sp->tpage = GetTPage(2, 0, 256, 0);
		sp->u = 0;
		sp->v = i;
		sp->r = sp->g = sp->b = 0x80;
		sp->mx = 0;
		sp->my = 0;
		sp->scalex = ONE;
		sp->scaley = ONE;
		sp->rotate = 0;
	}

}



void sort_back_wave(void)
{
	int		i;
	GsSPRITE	*sprtp;

	sprtp = &back[0];
	for (i = 0; i < 240; i++, sprtp++) {
		sprtp->x = -128 + back_wave_amp * rsin(back_wave_ph+i*128) / ONE,
		GsSortFastSprite(&back[i], &Wot2[outbuf_idx], 1<<OT_LENGTH2 -1);
	}
	back_wave_ph += 64;
}




int rsin_sub(int ang)
{
	if(ang <= 2048) {
		if (ang <= 1024) {
			return (sincos_tbl[ang]);
		}
		return (sincos_tbl[2048-ang]);
	} else {
		if (ang <= 3072) {
			return (-sincos_tbl[ang-2048]);
		}
		return (-sincos_tbl[4096-ang]);
	}
}

int rsin(int ang)
{
	if (ang < 0) {
		return (-rsin_sub(-ang & 0x0fff));
	}
	else {
		return (rsin_sub(ang & 0x0fff));
	}
}

#if	0
int rcos(int ang)
{
    if (ang < 0) {
    	ang = -ang;
    }
    ang &= 0x0fff;

	if (ang <= 2048){
		if (ang <= 1024) {
			return (sincos_tbl[1024-ang]);
		}
		return (-sincos_tbl[ang-1024]);
	} else {
		if (ang <= 3072) {
			return (-sincos_tbl[3072-ang]);
		}
		return (sincos_tbl[ang-3072]);
	}
}
#endif




void back_wave(int *job)
{
	switch (*job) {
		case	0 :
			init_back_wave();
			*job += 1;
			break;
		case	1 :
			back_wave_amp = 0;
			back_wave_ph = 0;
			back_wave_timer = 0;
			*job += 1;
		case	2 :
			sort_back_wave();
			if (++back_wave_timer > 60*3) {
				back_wave_timer = 0;
				*job += 1;
			}
			break;
		case	3 :
			sort_back_wave();
			if (++back_wave_timer > 3) {
				back_wave_timer = 0;
				if (++back_wave_amp > 80) {
					*job += 1;
				}
			}
			break;
		case	4 :
			sort_back_wave();
			if (++back_wave_timer > 3) {
				back_wave_timer = 0;
				if (--back_wave_amp < 0) {
					*job = 1;
				}
			}
			break;
		default :
			break;
	}
}



int obj_interactive(void)
{
	if (padd & PADRleft) {
		PCube.vy += 5*ONE/360;
	}
	
	if (padd & PADRright) {
		PCube.vy -= 5*ONE/360;
	}
	
	if (padd & PADRup) {
		PCube.vx += 5*ONE/360;
	}
	
	if (padd & PADRdown) {
		PCube.vx -= 5*ONE/360;
	}

	
	if (padd & PADR1) {
		DCube.coord.t[2] += 100;
	}
	
	if (padd & PADR2) {
		DCube.coord.t[2] -= 100;
	}

	if (padd & PADLleft) {
		DCube.coord.t[0] -=10;
	}
	
	if (padd & PADLright) {
		DCube.coord.t[0] +=10;
	}

	
	if (padd & PADLdown) {
		DCube.coord.t[1]+=10;
	}
	
	if (padd & PADLup) {
		DCube.coord.t[1]-=10;
	}
	
		// quit
	if (padd & PADselect) {
		ResetGraph(3);
		return (0);
	}

	set_coordinate(&PCube, &DCube);
	return (1);
}



void init_all(void)
{
	GsBOXF		*bp = &tex_clear;

	ResetGraph(0);					
	padd=0;							

	GsInitGraph(256 ,240, 4, 1, 0);	
									
	GsDefDispBuff(0, 0, 0, 256);	
	GsInit3D();						

	Wot[0].length = OT_LENGTH;		
	Wot[1].length = OT_LENGTH;		
	Wot[0].org = zsorttable[0];		
	Wot[1].org = zsorttable[1];	   
	Wot2[0].length = OT_LENGTH2;	
	Wot2[1].length = OT_LENGTH2;	
	Wot2[0].org = zsorttable2[0];	
	Wot2[1].org = zsorttable2[1];	

	coord_init();					
	model_init();					
	view_init();				   
	light_init();				   

	back_wave_job = 0;
	back_wave(&back_wave_job);	   

	bp->attribute = 0;
	bp->x = -128;
	bp->y = -120;
	bp->w = 256;
	bp->h = 240;
	bp->r = bp->g = bp->b = 0;
}



void view_init(void)
{
	GsSetProjection(1000);	

	view.vpx = 0; view.vpy = 0; view.vpz = -2000;
   
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	
	view.rz=0;
	
	view.super = WORLD;

	GsSetRefView2(&view);
}




void light_init(void)
{
	pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
	
	pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
	
	GsSetFlatLight(0, &pslt[0]);

	pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
	pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
	GsSetFlatLight(1, &pslt[1]);
  
	pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
	pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
	GsSetFlatLight(2, &pslt[2]);

	GsSetAmbient(0, 0, 0);

	GsSetLightMode(0);
}





void coord_init(void)
{
	GsInitCoordinate2(WORLD, &DCube);

	PWorld.vx = PWorld.vy = PWorld.vz = 0;
	PCube = PWorld;
	
	DCube.coord.t[2] = 4000;
}




void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
	MATRIX	tmp1;

	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	RotMatrix(pos, &tmp1);

	coor->coord = tmp1;

	coor->flg = 0;
}




void texture_init(u_long addr)
{
	RECT	rect1;
	GsIMAGE	tim1;

	GsGetTimInfo((unsigned long *)(addr+4), &tim1);
	rect1.x = tim1.px;		
	rect1.y = tim1.py;		
	rect1.w = tim1.pw;		
	rect1.h = tim1.ph;		

	LoadImage(&rect1, tim1.pixel);
	DrawSync(0);

	if ((tim1.pmode>>3) & 0x01) {
		rect1.x = tim1.cx;	
		rect1.y = tim1.cy; 
		rect1.w = tim1.cw;	
		rect1.h = tim1.ch;	

		LoadImage(&rect1, tim1.clut);
		DrawSync(0);
	}
}


void model_init(void)
{
	u_long	*dop;
	GsDOBJ2	*objp;
	int		i;

	dop = (u_long *)MODEL_ADDR;	
	dop++;

	GsMapModelingData(dop);		
							   
	dop++;
	Objnum = *dop;				
	dop++;					   
								
	for (i = 0; i < Objnum; i++) {
		GsLinkObject4((u_long)dop, &object[i], i);
	}

	for (i = 0, objp = object; i < Objnum; i++) {
		
		objp->coord2 =  &DCube;
		
		objp->attribute = GsDOFF;
		objp++;
	}

	object[0].attribute &= ~GsDOFF;	 
	object[0].coord2 = &DCube;	
}




u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}

