// 2D Tutorial main3.c
// 19th March 2001
// Max
// Re-use any/all code at your own discretion

#include <libps.h>		// PlayStation Specific Library

#include "pad.h"		// pad handling functions

#define SCREEN_WIDTH	320		// holds the screen width
#define SCREEN_HEIGHT	256		// holds the screen height

#define NUM_TILESX	20			// SCREEN_WIDTH/16 = 20
#define NUM_TILESY	16			// SCREEN_HEIGHT/16 = 16

#define OT_LENGTH	9			// ordering table length

#define NO_OF_SPRITES	(1+1)	// no of sprites plus one just for safety

#define BARNEY_ADDR	(0x80090000)	// location of our sprite in VRAM
									// this must be the same as in the auto2.sio file
#define TILES_ADDR (0x80093250)		// location of our backdrop image

int frameNo;			// holds the current display frame
int animFrameNo;		// hold the current animation frame
int animState;			// holds the type of animation to perform (walk left, right etc...)
int PLAYING = 1;		// 

int uIndices[8] = {0,32,64,96,128,96,64,32};	// holds the u, v coords of the animation frames
int vIndices[8] = {80,80,80,80,80,80,80,80};

GsOT WorldOT[2];						// ordering table headers
GsOT_TAG WorldTags[2][1<<OT_LENGTH];	// the ordering tables units
										
static PACKET packetArea[2][NO_OF_SPRITES*sizeof(GsSPRITE)]; // GPU PACKETS AREA

GsSPRITE sprite;	// this is basically a header file for the TIM

GsBG bgData;			// declare background handler
GsMAP bgMapData;		// declare a BG composition map
GsCELL bgCellData[16];	// declare cells for amount of 16x16 tiles

unsigned short mapLayout[16]; // holds the layout of the BG

u_long bgWorkArea[(((SCREEN_WIDTH/16+1)*(SCREEN_HEIGHT/16+1+1)*6+4)*2+2)]; // area of memory used for sorting BG


void Initialise(void)
{
	PadInit();	// without this you can kiss the pad adios
	SetVideoMode(MODE_PAL);	// this allows use of PAL resolutions
	// initialise the graphics engine with 320 x 240 resolution
	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsNONINTER|GsOFSGPU,1,0);
	GsDefDispBuff(0,0,0,SCREEN_HEIGHT);

	WorldOT[0].length = OT_LENGTH;	// set OT length
	WorldOT[0].org = WorldTags[0];	// link in the OT units
	WorldOT[1].length = OT_LENGTH;
	WorldOT[1].org = WorldTags[1];

	GsClearOt(0, 0, &WorldOT[0]);	// initialise the OTs
	GsClearOt(0, 0, &WorldOT[1]);

	FntLoad(960, 256);			// load the font
	FntOpen(10, 80, 256, 200, 0, 512);	// open it to screen
}

void InitSprite(u_long *addr)
{
	RECT rect;	// this is a rectangle in which the sprite will be stored
	
	GsIMAGE imageInfo;	// this holds the actual sprite data
	
	GsGetTimInfo((u_long *)(addr+1),&imageInfo); // read the TIM and store its data in imageInfo
															
	// set up the rect before we load in the data
	rect.x = imageInfo.px;		// image x position in frame buffer
	rect.y = imageInfo.py;		// image y position in frame buffer
	rect.w = imageInfo.pw;		// image width
	rect.h = imageInfo.ph;		// image height

	LoadImage(&rect, imageInfo.pixel);	// load in the data to the rect
	
	// check to see if the image has a clut i.e. < 16 bit
	if((imageInfo.pmode>>3)&0x01)
	{
		// CLUT exists so load it into the rect
		rect.x = imageInfo.cx;		// x position in Frame Buffer
		rect.y = imageInfo.cy;		// y position in Frame Buffer
		rect.w = imageInfo.cw;		// width of the CLUT
		rect.h = imageInfo.ch;		// height of the CLUT
		LoadImage(&rect, imageInfo.clut);	// Loads all of the above into the Frame Buffer
	}

	// check for bit mode
	switch(imageInfo.pmode)
	{
		case 0x08 /*4 bit*/ :	sprite.attribute = 0 << 24;		// this tells the yaroze what attributes the image has
								sprite.w = imageInfo.pw*4;		// this is the image data offset in the VRAM
								sprite.tpage = GetTPage(0,0,imageInfo.px,imageInfo.py);	// gets texture page address
								break;
		
		case 0x09 /*8 bit*/ :	sprite.attribute = 1 << 24;
								sprite.w = imageInfo.pw*2;
								sprite.tpage = GetTPage(1,0,imageInfo.px,imageInfo.py);
								break;
					
		default  /*16 bit*/ :	sprite.attribute = 1 << 25;
								sprite.w = imageInfo.pw;
								sprite.tpage = GetTPage(2,0,imageInfo.px,imageInfo.py);
								break;
	}
	
	sprite.h = imageInfo.ph;		// sets the height of the sprite
	
	sprite.x = (SCREEN_WIDTH / 2) - (sprite.w / 2);
	sprite.y = (SCREEN_HEIGHT / 2) - (sprite.h / 2);

	sprite.u = 0;					// sets the texture page offset of the sprite along the x axis
	sprite.v = 0;					// sets the texture page offset of the sprite along the y axis
	
	sprite.cx = imageInfo.cx;		// sets the x position of the CLUT, if any
	sprite.cy = imageInfo.cy;		// sets the y position of the CLUT, if any
	
	sprite.r = 128;					// used to apply brightness to the sprite, 128 is normal
	sprite.g = 128;					// used to apply brightness to the sprite, 128 is normal
	sprite.b = 128;					// used to apply brightness to the sprite, 128 is normal
	
	sprite.mx = sprite.w/2;			// sets the center point of the sprite for use with rotation etc
	sprite.my = sprite.h/2;			// Sets the center point of the sprite for use with rotation etc
	
	sprite.scalex = 4096;			// sets sprite scaling to default, x axis
	sprite.scaley = 4096;			// sets sprite scaling to default, y axis
	
	sprite.rotate = 0;				// sets rotation to off
	
	DrawSync(0);	// wait for LoadImage() function to finish
}

GsIMAGE *ReadTIM(u_long *addr)
{
	static GsIMAGE tim;
	RECT rect; 			
			
	// skip id and initialise image structure 
	GsGetTimInfo(++addr, &tim);	

	// prepare RECT to hold TIM image
	rect.x = tim.px;
	rect.y = tim.py;
	rect.w = tim.pw;
	rect.h = tim.ph;
	// load the image into the RECT on VRAM
	LoadImage(&rect, tim.pixel);

	DrawSync(0);	// wait for LoadImage function to finish executing

    // check if CLUT exists and transfer it to VRAM
	// prepare RECT to hold CLUT
 	if( (tim.pmode >> 3) & 0x01 ) 
 	{
 	  	rect.x = tim.cx;
	  	rect.y = tim.cy;
	  	rect.w = tim.cw;
	  	rect.h = tim.ch;
		// load the CLUT into the RECT in VRAM
	  	LoadImage(&rect, tim.clut);
	}	
	  
    DrawSync(0);	// wait for LoadImage function to finish executing

    return(&tim);	// return the initialised image data

}// end ReadTIM  

void InitBG (void)
{
	bgData.attribute = 1<<24;
	bgData.x = 0;
	bgData.y = 0;
	bgData.w = SCREEN_WIDTH * 2;
	bgData.h = SCREEN_HEIGHT * 2;
	bgData.scrollx = 0;
	bgData.scrolly = 0;
	bgData.r = 128;
	bgData.g = 128;
	bgData.b = 128;
	bgData.map = &bgMapData;
	bgData.mx = 0;
	bgData.my = 0;
	bgData.scalex = ONE;
	bgData.scaley = ONE;
	bgData.rotate = 0;
}

void InitMap (void)
{
	bgMapData.cellw  = 16;
	bgMapData.cellh  = 16;
	bgMapData.ncellw = 4;
	bgMapData.ncellh = 4;
	bgMapData.base   = &bgCellData[0];
	bgMapData.index  = &mapLayout[0];
}

void InitCELLs (void)
{
	GsIMAGE *timImage;
	u_short texPage;
	u_short CLUT;
	int count=0,
	    x, y;

	// get info
	timImage = ReadTIM((u_long *)TILES_ADDR);			// read in TIM
	texPage = GetTPage(1,0,timImage->px,timImage->py);	// get TPAGE
	CLUT = GetClut(timImage->cx,timImage->cy);			// get CLUT

	// rows in tpage
	for( y=0; y<=3; y++ )
	{
		// columns in tpage
		for( x=0; x<=3; x++ )
		{
			bgCellData[count].u     = x*16;	// offset x for every column
			bgCellData[count].v     = y*16;	// offset y for every row
			bgCellData[count].cba   = CLUT;	
			bgCellData[count].flag  = 0;
			bgCellData[count].tpage = texPage;
			count++;
		}// end for x
	}// end for y
}


void DealWithPad (void)
{
	long pad;

	pad = PadRead();

	if(pad)
	{
		if (pad & PADstart && pad & PADselect)	// quit
		{
			PLAYING = 0;
		}

		if (pad & PADstart)						// pause
		{
			while (pad & PADstart)
			{
			pad = PadRead();
			}
		}

		if (pad & PADLup)				// d-pad up
		{
			bgData.scrolly -= 2;
			animState = 1;
		}

		if (pad & PADLdown)				// d-pad down
		{
			bgData.scrolly += 2;
			animState = 1;
		}

		if (pad & PADLleft)				// d-pad left
		{
			bgData.scrollx -= 2;
			animState = 1;
		}

		if (pad & PADLright)			// d-pad right
		{
			bgData.scrollx += 2;
			animState = 1;
		}
	}
	else
	{
		animState = 0;
	}
}

int main(void)
{
	int vsync = 0,
	    side, i;

	int frameCount = 0;

	// layout set-up algorithm
	for(i = 0 ; i <= 16 ; i++)
	{
		mapLayout[i] = i;
	}
	
	Initialise();

	// initialise BG
	InitCELLs();
	InitMap();
	InitBG();
	
	InitSprite((u_long *)BARNEY_ADDR);

	// setup the initial position of the sprite
	// remember that the sprite is offset because the image is larger
	sprite.x = ((SCREEN_WIDTH / 2) - (32 / 2));
	sprite.y = ((SCREEN_HEIGHT / 2) - (80 / 2));

	
	// set up the height and width of one frame
	sprite.w /= 5;	// 160 / 5 = 32 pixels
	sprite.h /= 2;	// 160 / 2 = 80 pixels

	// set initial frame to display
	sprite.u = 0;
	sprite.v = 0;

	// setup timer for animation
	// StartRCnt(1);	// this is used to start the timer if you want to use it
	// ResetRCnt(1);	// this is called to zero the timer
	
	GsInitFixBg16(&bgData,bgWorkArea);

	side = GsGetActiveBuff();

	while(PLAYING)
	{
		FntPrint("frame: %d\n", frameNo);
		FntPrint("scroll: X: %d Y: %d\n", bgData.scrollx, bgData.scrolly);
		FntPrint("barney: X: %d Y: %d\n", sprite.x, sprite.y);

		frameNo++;		// increment frame count
		frameCount++;

		if(frameCount > 4)	// can also use if(GetRCnt(1) > 800)
		{
			animFrameNo++;	// increment animation frame counter
			frameCount = 0;
			// ResetRCnt(1);	// use this to reset the timer back to 0 for next time
		}

		DealWithPad();
		
		if(animState)
		{
			sprite.u = uIndices[animFrameNo];
			sprite.v = vIndices[animFrameNo];
		}
		else
		{
			sprite.u = 0;
			sprite.v = 0;
		}		
		
		GsSetWorkBase((PACKET *)packetArea[side]);
		
		GsClearOt(0,0,&WorldOT[side]);	//clear out the present OT ready for drawing

		GsSortFixBg16(&bgData,bgWorkArea,&WorldOT[side],1); // register the BG in the OT
		
		GsSortFastSprite(&sprite,&WorldOT[side],0);	// register the sprite in the ordering table
				
		DrawSync(0);
		vsync = VSync(1);
		VSync(0);		// get the vertical sync interval
								
		FntPrint("HSYNC: %d\n", vsync);

		GsSwapDispBuff();
		GsSortClear(0,0,0,&WorldOT[side]);
		GsDrawOt(&WorldOT[side]);	// draw the current off screen buffer to the screen

		if(animFrameNo >= 7)
			animFrameNo = 0;
		
		side = side^1;	// change side to 1 or 0
		FntFlush(-1);	// print all font calls
	}

	return ResetGraph(0);
}
