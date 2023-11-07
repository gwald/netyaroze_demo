/*
 *
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
*/


#include <libps.h>
#include "pad.h"


#define KANJI				
#define OT_LENGTH	1		
#define MAXOBJ		800		
#define MVOL		127	   
#define SVOL		127	   
#define DFILE		3	   


#define VH_ADDR		0x80090000
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000


#define FRAME_X		320	   
#define FRAME_Y		240		
#define WALL_X		(FRAME_X-16)   
#define WALL_Y		(FRAME_Y-16)   


#define limitRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))



GsOT		WorldOT[2];
GsOT_TAG	OTTags[2][1<<OT_LENGTH];


PACKET		GpuPacketArea[2][MAXOBJ*(20+4)];
GsSPRITE	sprt[MAXOBJ];


typedef struct {
	u_short x, y;			
	u_short dx, dy;		   
} POS;


volatile u_char *bb0, *bb1;


typedef struct {
	char	*fname;
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\SAMPLE1.SEQ;1", (void *)SEQ_ADDR, 0},
};


short vab, seq;


static void init_prim();	   
static void init_point(POS *pos);  
static long pad_read(long n);	   
static u_long PadRead(long id);		
static void datafile_search();	 
static void datafile_read();	   
static void init_sound();	 
static void play_sound();	  
static void stop_sound();	



long VideoMode;

main()
{
	int	nobj = 1;	
	GsOT	*ot;	   
	int	i, cnt, x, y;	
	int	activeBuff;
	GsSPRITE	*sp;
	POS	pos[MAXOBJ];
	POS	*pp;

	VideoMode = GetVideoMode();
	if (VideoMode == MODE_NTSC)
		printf("VideoMode at start is NTSC\n");
	else if (VideoMode == MODE_PAL)
		printf("VideoMode at start is PAL\n");
	else
		{
		printf("invalid video mode\n");
		exit(1);
		}


	GetPadBuf(&bb0, &bb1);	
	datafile_search();	
	datafile_read();	

	GsInitGraph(320, 240, 4, 0, 0);	
	GsDefDispBuff(0, 0, 0, 240);	

	
	for (i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}

	
#ifdef KANJI    
	KanjiFntOpen(160, 16, 256, 240, 704, 0, 768, 256, 0, 512);
#endif
	FntLoad(960, 256);	
	FntOpen(16, 16, 256, 200, 0, 512);	

	init_prim();		
	init_point(pos);   
	init_sound();		
	play_sound();	   

	while ((nobj = pad_read(nobj)) > 0) {
		
		activeBuff = GsGetActiveBuff();
		GsSetWorkBase((PACKET *)GpuPacketArea[activeBuff]);

		
		GsClearOt(0, 0, &WorldOT[activeBuff]);

		
		sp = sprt;	pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			
			sp->x = x;	sp->y = y;
			
			GsSortFastSprite(sp, &WorldOT[activeBuff], 0);
		}

		DrawSync(0);			
		cnt = VSync(0);			
		GsSwapDispBuff();		

		
		GsSortClear(60, 120, 120, &WorldOT[activeBuff]);

		
		GsDrawOt(&WorldOT[activeBuff]);

#ifdef KANJI
		KanjiFntPrint("ã ÇÃêîÅÅ%d\n", nobj);
		KanjiFntPrint("éûä‘=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n\n\n", cnt);
		FntPrint("UP    : INCREASE\n");
		FntPrint("DOWN  : DECREASE\n");
		FntPrint("L1    : PAUSE\n");
		FntPrint("SELECT: END\n");
		FntFlush(-1);
	}	

   
	stop_sound();	   
	return(0);		
}



#include "balltex.h"		


static void init_prim()
{
	GsSPRITE *sp;
	u_short	tpage;
	RECT	rect;
	int	i;
	
	rect.x = 640;	rect.y = 0;
	rect.w = 16/4;	rect.h = 16;
	LoadImage(&rect, ball16x16);
	tpage = GetTPage(0, 0, 640, 0);

	for (i = 0; i < 32; i++) {
		rect.x = 0;	rect.y = 480+i;
		rect.w = 256;	rect.h = 1;
		LoadImage(&rect, ballcolor[i]);
	}

	for (sp = sprt, i = 0; i < MAXOBJ; i++, sp++) {
		sp->attribute = 0;
		sp->x = 0;
		sp->y = 0;
		sp->w = 16;
		sp->h = 16;
		sp->tpage = tpage;
		sp->u = 0;
		sp->v = 0;
		sp->cx = 0;
		sp->cy = 480+(i%32);
		sp->r = sp->g = sp->b = 0x80;
		sp->mx = 0;
		sp->my = 0;
		sp->scalex = ONE;
		sp->scaley = ONE;
		sp->rotate = 0;
	}
}


static void init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		
		pos->y  = rand();		
		pos->dx = (rand() % 4) + 1;	
		pos->dy = (rand() % 4) + 1;	
		pos++;
	}
}

static long pad_read(long n)
{
	u_long padd = PadRead(1);		

	if (padd & PADLup)	n += 4;		
	if (padd & PADLdown)	n -= 4;	   

	if (padd & PADL1)
		while (PadRead(1)&PADL1);  

	if(padd & PADselect)
		return(-1);			

	limitRange(n, 1, MAXOBJ-1);	   
	return(n);
}


static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}



static void datafile_search()
{
	int i, j;

	for (i = 0; i < DFILE; i++) {		
		for (j = 0; j < 10; j++) {	
			if (CdSearchFile(&(dfile[i].finfo), dfile[i].fname) != 0)
				break;		
			else
				printf("%s not found.\n", dfile[i].fname);
		}
	}
}


static void datafile_read()
{
	int i, j;
	int cnt;

	for (i = 0; i < DFILE; i++) {		
		for (j = 0; j < 10; j++) { 
			CdReadFile(dfile[i].fname, dfile[i].addr,dfile[i].finfo.size);

			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	

			if (cnt == 0)
				break;	
		}
	}
}



static void init_sound()
{
	vab = SsVabTransfer( (u_char*)VH_ADDR, (u_char*)VB_ADDR, -1, 1 );
	if (vab < 0) {
		printf("SsVabTransfer failed (%d)\n", vab);
		return;
	}

	seq = SsSeqOpen((u_long *)SEQ_ADDR, vab);
	if (seq < 0)
		printf("SsSeqOpen failed (%d)\n", seq);
}


static void play_sound()
{
	SsSetMVol(MVOL, MVOL);			
	SsSeqSetVol(seq, SVOL, SVOL);	  
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);  
}


static void stop_sound()
{
	SsSeqStop(seq);				
	VSync(0);
	VSync(0);
	SsSeqClose(seq);			
	SsVabClose(vab);			
}


