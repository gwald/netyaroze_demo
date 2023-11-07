/*

Simple test program for Yaroze..

CJH @SCEE.

Draw lines on Screen ( to follow music )

V1.0 May 6th 1997

	First Yaroze code; Get lines drawing on screen ( sin wave pattern )
	Implement CD data transfer
	Set play list
	Get First scope on screen
	
V1.1 May 7th 1997

	FFT routine and display mode
	Polar waveform ( using sin table from FFT routine )
	Add CD volume control & track change 
	On screen display
	
V1.2 May 7th 1997 ( after dinner.... )

	Wierd colour cycling ( may be bugged )
	Strobe style sample and hold
	Switchable OSD		

V1.3 May 8th 1997 (early morning, before going to work)

	Added timer function to check speed
	Added persistence feature (cool....)

----I think I'll leave this version as is for now..
*/

#include <libps.h>

#include "fft.c"

//	Extra function for reading CD data

extern signed short *ReadCDDA();



//	OT variables...

#define OTSIZE 8
#define OTSHIFT 14-OTSIZE

GsOT ot[2];
GsOT_TAG tags[2][1<<OTSIZE];
PACKET work[2][24*2500];		// Room for GPU packets

// Red to Yellow line

GsGLINE line={ 0,0,0,0,0,255,0,0,255,255,0 };

// Screen size sprite for persistence effect

GsSPRITE oldscr =
{
	(2<<24),				// 16bit Sprite with RGB adjust
	-128,-120,
	256,240,
	0,
	0,0,
	0,0,
	0x70,0x70,0x70,
	0,0,
	4096,4096,
	0
};

// Colour presets

#define ON 255<<16
int target[]=
{
	ON,0,0,		// Red
	ON,ON,0,		// Yellow
	0,0,ON,		// Blue
	ON,ON,ON,	// White
	ON,0,ON,		// Cyan
	0,ON,0,		// Green
	0,ON,ON,		// Magenta
	ON,0,0			// Red again	
};

#define U 255<<10
#define D -255<<10

int delta[]=
{
	U,D,D,	
	0,U,0,
	D,D,U,
	U,U,0,
	0,D,0,
	D,U,D,
	0,0,U,
	U,D,D
};

int acc[]=
{
	ON,0,0,		// Red at start
	ON,ON,0	// and Yellow
};
//	Joypad variables and constants

volatile u_short *pad0,*pad1;			// Pad buffers

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


// CD tracklist....

int tracklist[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 };

int main()
{
	int idx;
	int i,n,f;
	signed short *data;
	int track;
	int timer;
	int display=0;
	int volume=127;
	int bounce=0;
	int frame=0;
	int sample=0;
	int base=0;
	int osd=1;
	int persist=0;

//	Initialise system

	ResetGraph( 0 );
	GsInitGraph( 256,240,GsOFSGPU,1,0 );
	GsDefDispBuff( 0,0, 0,256 );
	GsInit3D();

	GetPadBuf( &pad0,&pad1 );

//	Initialise OTs

	ot[0].length = ot[1].length = OTSIZE;
	ot[0].org = tags[0];
	ot[1].org = tags[1];

	
//	Initialise Viewpoints	


	GsSetProjection( 1000 );


//	Start CD playing audio

	SsSetSerialVol( SS_CD,127,127 );
	SsSetSerialAttr( SS_CD,SS_REV,SS_SOFF );
	SsSetSerialAttr( SS_CD,SS_MIX,SS_SON );
	CdPlay( 2,tracklist,0 );


//	Initialise Font system

	FntLoad(960, 256);					 // fonts
	f = FntOpen(-112, -90, 224, 16, 0, 56); 


//	Main loop

	while (pad0[1]&PADselect)
	{

		timer = VSync( 1 );

//		Setup OT and drawing stuff..

		idx = GsGetActiveBuff();
		GsSetWorkBase( work[idx] );
		GsClearOt( 0,0,&ot[idx] );
									
//		Fetch data from CD...dependant on sample count

		if ( (frame--)<1 )
		{
			data = ReadCDDA();
			frame = sample>>3;
		}

		if ( ((pad0[1]&PADL1)==0) && (sample>0) ) sample--;
		if ( ((pad0[1]&PADL2)==0) && (sample<240) ) sample++;
			
			
//		Change CD track & volume if needed			

		if ( ((pad0[1]&PADdown)==0) && (volume>0) )
		{
			volume--;
			SsSetSerialVol( SS_CD,volume,volume );
		}
		if ( ((pad0[1]&PADup)==0) && (volume<127) )
		{
			volume++;
			SsSetSerialVol( SS_CD,volume,volume );
		}
			
		track = CdPlay( 3,tracklist,0 );

//		Debounced controls..
		 					
		if (bounce==0)
		{
			if (((pad0[1]&PADleft)==0)&&(track>0))
			{
				track--;
				CdPlay( 2,tracklist,track );
				bounce=10;
			}
			if (((pad0[1]&PADright)==0)&&(tracklist[track+1]!=0))
			{
				track++;
				CdPlay( 2,tracklist,track );
				bounce=10;
			}
			if ((pad0[1]&PADstart)==0)
			{
				osd ^= 1;
				bounce = 10;
			}
			if ((pad0[1]&PADR1)==0)
			{
				persist ^= 1;
				bounce = 10;
			}
		}
		else bounce--;

			 					
//		Handle colour fading if needed

		n=0;
		for (i=0;i<6;i++)
			if (target[i+base]!=acc[i])
			{
				acc[i]+=delta[i+base];
				n++;
			}
		
		line.r0 = acc[0]>>16;
		line.g0 = acc[1]>>16;
		line.b0 = acc[2]>>16;
		line.r1 = acc[3]>>16;
		line.g1 = acc[4]>>16;
		line.b1 = acc[5]>>16;		 									 

		if ((n==0)&&((pad0[1]&PADtriangle)==0))
		{
			base+=3;
			if (base==21) base=0;
		}


//		Add drawing stuff

		if ((pad0[1]&PADcross)==0) display = 0;
		if ((pad0[1]&PADsquare)==0) display = 1;
		if ((pad0[1]&PADcircle)==0) display = 2;

		switch ( display )
		{

		case 0:		// Mirror filled mono waveform


			line.x0 = line.x1 = -128;
			line.y0 = 0;

			for (i=0;i<256;i++)
			{
				line.y1 = ((data[i]+data[512+i])*volume)>>15;
				GsSortGLine( &line,&ot[idx],0 );
				line.y1 = -line.y1;
				GsSortGLine( &line,&ot[idx],0 );
				line.x0++;
				line.x1++;
			}
			break;

		case 1:		// Polar filled mono waveform

			line.x0 = line.y0 = 0;

			for (i=0;i<256;i++)
			{
				n = ((data[i]+data[512+i])*volume)>>15;
				line.x1 = (s_table[i]*n)>>7;
				line.y1 = (s_table[(i+64)&255]*n)>>7;
				GsSortGLine( &line,&ot[idx],0 );
			}
			break;

		case 2:		// Mono FFT 128 point

			// Bit reversal for mono with simple window

			for (i=0;i<128;i++)
			{
				real[ b_table[i] ] = ((data[i]+data[512+i])*i*volume)>>15;
				imag[ i ] = 0;
			}
			for (;i<256;i++)
			{
				real[ b_table[i] ] = ((data[i]+data[512+i])*(256-i)*volume)>>15;
				imag[ i ] = 0;
			}


			fft();			// Perform FFT

			line.x0 = line.x1 = -120;
			line.y0 = 100;

			for ( i=0;i<120;i++ )
			{
				line.y1 = 100-((abs(real[i])+abs(imag[i]))*(i+4))/64;
				GsSortGLine( &line,&ot[idx],0 );
				line.x0 += 2;
				line.x1 += 2;
			}


		}

//		Add persistance if needed

		if (persist)
		{
			oldscr.tpage = idx?0:16;
			GsSortSprite( &oldscr,&ot[idx],0 );
		}

//		Switch buffers and draw primitives

		if ((pad0[1]&PADR2)==0)		// Timer info
		{
			timer = VSync(1)-timer;
			FntPrint( f,"Calc time: %3d",timer );
		}

		DrawSync(0);
		VSync(0);
		timer = VSync(1);
		GsSwapDispBuff();
		if (persist==0) GsSortClear( 0,0,0,&ot[idx] );
		GsDrawOt( &ot[idx] );

		if ((pad0[1]&PADR2)==0)		// Timer info
		{
			DrawSync(0);
			timer = VSync(1)-timer;
			FntPrint( f,"Draw time: %3d\n",timer );
		}

		if (osd)
		{
			FntPrint( f,"Track %d Volume %d Hold %d",tracklist[track],volume,sample>>3 );
			FntFlush(-1);		// Show text
		}
	}

}