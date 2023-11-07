/*

	Plasma Effect using Yaroze hardware...

	CJH @SCEE.
	Based on 'Blur' demo code by Jim Pitts

	May 24th '97

	This was prompted by Jims code, I decided to try and speed it up
	making any short cuts I could to achieve 60fps. ( I want to use it
	as a backdrop for the next version of my Sound to Light )
	My initial C routine with no optimisation took 7000 scan lines to
	process 256by240 - This version takes around 240 scan lines ( but only
	processes 160by120 ( about 1/3 ) which is effectively 10 times quicker

	I think I can squeeze more speed by converting back to assembly code,
	but it's quick enough at the moment.

	May 25th '97

	I changed the screen res and my processing time went up to 280 scanlines
	which was a bit odd, as the extra time involves the GPU only...
	It turned out that FntFlush blocks ( I think )
	Moving it from straight after the Draw to just before the VSync saved
	nearly 60 scan lines...

	I've now got it running on a 640x480 screen taking around 199 scanlines
	for the C version. ( I know that it still is only 160x120, but when I
	add some graphics on top they will be high res )

	....

	I've converted the C back to assembly and it now takes 165 scanlines,
	Now to add some other stuff...
	
	I added my Snd2lt stuff and things now take 277 scanlines. I'll have to
	optimise some other stuff
	....
	The main routine (now called main2) now uses the scratchpad as a stack.
	This will speed up the calls to the LibPS stuff that I have no control
	over. Things now take 238 lines, so 60fps is still as possibility.
	NB: When you change stack, do it around a routine call, or your program
	will become incredibly confused as local variables are accessed via SP
	if any optimisations are used....

*/

#include <libps.h>

//	Extra function for reading CD data

extern signed short *ReadCDDA();


#define OTSIZE 1

// Three OTs because I dont need to double buffer the sprite
GsOT ot[3];		
GsOT_TAG tags[3][1<<OTSIZE];

PACKET work[3][24*1000];	// Maximum of 1000 items per frame

volatile u_short *pad0,*pad1;

#define PADsquare	(1<<15)
#define PADcross	(1<<14)
#define PADcircle	(1<<13)
#define PADtriangle	(1<<12)

#define PADR1		(1<<11)
#define PADL1       (1<<10)
#define PADR2		(1<< 9)
#define PADL2       (1<< 8)

#define PADleft		(1<< 7)
#define PADdown		(1<< 6)
#define PADright	(1<< 5)
#define PADup		(1<< 4)

#define PADstart	(1<< 3)
#define PADselect	(1<< 0)


long *cossine = (short *)0x80090000;

char buffer[160*240];
RECT screen = { 640,0,80,120 };
RECT clear = { 0,0,640,512 };
RECT clut = { 640,255,16,1 };
short colour[] = { 5<<10,17<<10,15<<10,27<<10 };

GsSPRITE draw = {
	1<<24,
	-320,-240,
	160,120,
	10,
	0,0,
	640,255,
	128,128,128,
	0,0,
	ONE*4,ONE*4,
	0
	};


/*
	Plasma generating routine...
	I may have broken Jims original algorithm, but this C version is quite
	optimal ( I was only able to gain 20% by hand coding )
*/


extern plasma_asm( int theta );

void plasma_c( int theta )
{
	register int x,y;
	register unsigned long *p=buffer;
	register long c1,c2;
	register long *cs;
	register long csv;
	register unsigned long four;

	for (y=240;y-=2;)
	{
		c2 = cossine[y];
		cs = &cossine[theta];
		csv = cs[0];
		c1 = (2*theta)+(c2>>16);
		c2 = (c2<<16)>>16;
			
		x = 40;
		do
		{
			four = (((((csv<<16)>>16)-c1)&256)>>7)+((((csv>>16)+c2)&256)>>8);
			csv = cs[2];
			four |= (((((csv<<16)>>16)-c1-1)&256)<<1)+((((csv>>16)+c2+1)&256));
			csv = cs[4];
			four |= (((((csv<<16)>>16)-c1-2)&256)<<9)+((((csv>>16)+c2+2)&256)<<8);
			csv = cs[6];
			four |= (((((csv<<16)>>16)-c1-3)&256)<<17)+((((csv>>16)+c2+3)&256)<<16);
			csv = cs[8];
	
			*p++ = four;
			cs+=8;
			c1+=4;
			c2+=4;
		} while (--x);
	}
}

int tracklist[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 };



void main2()
{
	int theta=0;
	int idx;
	int i;
	signed short *data;
	short *scv;
	int n;

// Red to Yellow line

	GsGLINE line={ 0,0,0,0,0,255,0,0,255,255,0 };


	GsInitGraph(  640,480,GsOFSGPU,1,0 );
	GsDefDispBuff( 0,0, 0,0 );
	GsInit3D();

	GetPadBuf( (volatile unsigned char **)&pad0,(volatile unsigned char **)&pad1 );

	ot[0].length = ot[1].length = ot[2].length =OTSIZE;
	ot[0].org = tags[0];
	ot[1].org = tags[1];
	ot[2].org = tags[2];

//	Start CD playing audio

	SsSetSerialVol( SS_CD,127,127 );
	SsSetSerialAttr( SS_CD,SS_REV,SS_SOFF );
	SsSetSerialAttr( SS_CD,SS_MIX,SS_SON );
	CdPlay( 2,tracklist,0 );

//	Set Pallette

	LoadImage( &clut,colour );
	DrawSync( 0 );

	GsSwapDispBuff();		// Just so screen is active...
	idx = GsGetActiveBuff();

//	Set up sprite with it's own OT...

	GsSetWorkBase ( work[2] );
	GsClearOt( 0,0,&ot[2] );
	GsSortSprite( &draw,&ot[2],1 );	


	while ( pad0[1]&PADselect )
	{

		GsSetWorkBase ( work[idx] );
		GsClearOt( 0,0,&ot[idx] );
		GsSortSprite( &draw,&ot[idx],1 );	

//	Very lazy plasma effect...Thanks to Jim...

//		plasma_c( theta );
		plasma_asm( theta );
		data = ReadCDDA();

		theta = (theta+1)&0x3ff;

		scv = cossine;
		for (i=0;i<256;i++)
		{
			n = data[0]+data[512];
			data++;
			if (n<0) n=-n;
			line.x1 = ((scv[0]-0x100)*n)>>15;
			line.y1 = ((scv[1]-0x100)*n)>>15;
			scv += 8;
			GsSortGLine( &line,&ot[idx],0 );
		}


		LoadImage( &screen,buffer );
		GsDrawOt( &ot[2] );		// Draw sprite
		GsDrawOt( &ot[idx] );
		idx ^= 1;
	}

	ResetGraph( 0 );

}


long savesp;

int main()
{

// Change SP to scratchpad area...

	__asm__ volatile ("sw $29,(savesp)");
	__asm__ volatile ("la $29,0x1f8003f0" );

// Run program...

	main2();

// Restore SP....

	__asm__ volatile ("lw $29,(savesp)");

}