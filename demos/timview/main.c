/***************************************************************
*															   *
* Copyright (C) 1995 by Sony Computer Entertainment			   *
*			All rights Reserved								   *
*															   *
*		 L.Evans 		 					 08/10/96		   *
*															   *
*			Tim file viewer									   *	
*															   *
***************************************************************/



#include <libps.h>

#include "pad.h"

#include "asssert.h"



#define TIM_ADDRESS 0x80190000




#define TRUE 1
#define FALSE 0


#define SCREEN_WIDTH 320 
#define SCREEN_HEIGHT 240

#define CENTRE_X 160
#define CENTRE_Y 120

   
#define SCREEN_X_OFFSET 0
#define SCREEN_Y_OFFSET 16




#define OT_LENGTH 1


#define MAX_SIZEOF_PRIMITIVE 24



GsSPRITE MainSprite;

	// background rectangle   	
GsBOXF BackgroundRectangle;



GsOT		WorldOrderingTable[2];
	// 1 << OT_LENGTH: 2 to the power of OT_LENGTH
GsOT_TAG	OrderingTableTags[2][1<<OT_LENGTH];


	// GPU work area
PACKET		GpuPacketArea[2][1000 * MAX_SIZEOF_PRIMITIVE];



	// controller pad buffer: easiest way to access is
	// bitwise-AND with constants in pad.h 
u_long PadStatus;

		
	// libgs globals: used for setting screen offsets 
extern DISPENV GsDISPENV;	/* DISPENV of Gs */







	// prototypes

int main (void);
void InitialiseAll (void);
void InitialiseTheOnlySprite (void);   
;void InitialiseGsSprite (GsSPRITE* sprite);
void ClearBackground (void);  
int DealWithControllerPad (void);




	// functions


int main (void)
{
	int activeBuffer;
	int i;
		


	InitialiseAll();

		   

	while (1)				// main loop
		{
		if (DealWithControllerPad() == 0)
			break;
	  
		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) GpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&WorldOrderingTable[activeBuffer], 0);

		GsSortSprite(&MainSprite, &WorldOrderingTable[activeBuffer], 0); 
			 	
	 		// wait for end of drawing  
		DrawSync(0);			
		
			// swap buffers							
		GsSwapDispBuff();		

			// register clear command
		GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);

			// start drawing
		GsDrawOt(&WorldOrderingTable[activeBuffer]);
		}

		// cleanup
	ResetGraph(3);
	return(0);
}





void InitialiseAll (void)
{
	int i;

		// set resolution (320, 240) and screen mode: non-interlace
	GsInitGraph(320, 240, GsOFSGPU|GsNONINTER, 0, 0);	
	GsDefDispBuff(0, 0, 0, 240);
	
		// set the screen offset (varies with PAL/NTSC)
	GsDISPENV.screen.x = SCREEN_X_OFFSET;
	GsDISPENV.screen.y = SCREEN_Y_OFFSET;	
	
		// sort out ordering tables   
   	for (i = 0; i < 2; i++) 
   		{
		WorldOrderingTable[i].length = OT_LENGTH;
		WorldOrderingTable[i].org = OrderingTableTags[i];
		}
	
		// set up the controller pad	
	PadInit();
	
		// set up fonts for printing onto screen
	FntLoad(960, 256);	
	FntOpen(16, 16, 256, 200, 0, 512);

		// sort out the only sprite
	InitialiseTheOnlySprite();

		// clear background rectangle
	ClearBackground();
}





	// set up the only sprite to show the TIM file

	// NOTE: look in 'flying\object.c' for more general-purpose
	// version of this function
void InitialiseTheOnlySprite (void)
{
	short int tPageType;		// arg for GetTPage
						// 0 for 4-bit, 1 for 8-bit, 2 for 16-bit
						// 3 for 24-bit, 4 for mixed modes
	short int pixelMode;
	int needToLoadClut;
	int widthCompression;
	RECT pixelDestination, clutDestination;
	GsIMAGE	timImage;		// structure holding info on TIM file
	int eightBitMask = 1 << 24;
	int sixteenBitMask = 1 << 25;
	int mask1 = 1 << 29;		// for semi-transparency
	int mask2 = 1 << 30;		// for semi-transparency

		// initialise sprite
	InitialiseGsSprite(&MainSprite);

		// get information about TIM file into a GsIMAGE structure
		// use (address+4) to skip the header of the TIM file 
	GsGetTimInfo( (u_long *)(TIM_ADDRESS+4), &timImage);

		// load TIM to specified area
	pixelDestination.x = 320;
	pixelDestination.y = 0;

		// width and height of image
	pixelDestination.w = timImage.pw;
	pixelDestination.h = timImage.ph;
	printf("Pixels: w: %d, h: %d\n", timImage.pw, timImage.ph);

		// transfer data to the frame buffer (DMA transfer request) 
	LoadImage(&pixelDestination, timImage.pixel);

		// wait for DMA transfer to finish
		// (LoadImage is a non-blocking function)
	DrawSync(0);

	pixelMode = timImage.pmode;
	printf("pixelMode: %d\n", pixelMode);

	switch(pixelMode)
		{
		case 8:			// 4-bit CLUT
			printf("4-bit texture\n");
			widthCompression = 4;
			needToLoadClut = TRUE;
			tPageType = 0;
			break;
		case 9:			// 8-bit CLUT
			printf("8-bit texture\n");
			widthCompression = 2;
			needToLoadClut = TRUE;
			MainSprite.attribute |= eightBitMask;
			tPageType = 1;
			break;
		case 2:			// 16-bit CLUT
			printf("16-bit texture\n");
			widthCompression = 1;
			needToLoadClut = FALSE;
			MainSprite.attribute |= sixteenBitMask;
			tPageType = 2;
			break;
		case 3:			// 24-bit CLUT
		case 4:			// Mixed mode
			{
			int i;
			
			printf("Cannot handle 24-bit or multi-mode TIM files\n");
			for (i = 0; i < 60; i++)
				{
				VSync(0);
				}
			exit(1);
			}
			break;
		}
	printf("tPageType: %d\n", tPageType);

		// if pixel mode requires a CLUT, load the CLUT
		// CLUTs are required for 8-bit and 4-bit textures
	if (needToLoadClut)
		{
		printf("Yes we do need a CLUT\n");

			// load CLUT below double buffers
		clutDestination.x = 960;
		clutDestination.y = 0;

			// width and height of CLUT
		clutDestination.w = timImage.cw;
		clutDestination.h = timImage.ch;	   // note: always one

		printf("CLUT: w: %d, h: %d\n", timImage.cw, timImage.ch);

			// Load CLUT data to frame buffer
		LoadImage(&clutDestination, timImage.clut);

			// wait for DMA transfer to finish
		DrawSync(0);
		}

		// sort out texture page ID
	MainSprite.tpage = GetTPage(tPageType, 0, 320, 0);
	printf("tpage value: %d\n", MainSprite.tpage); 

		// sort out correct size of sprite
	MainSprite.w = pixelDestination.w * widthCompression;
	MainSprite.h = pixelDestination.h;

		// position sprite around centre of screen
	MainSprite.x = CENTRE_X - (MainSprite.w/2);
	MainSprite.y = CENTRE_Y - (MainSprite.h/2);
	printf("sprite position: %d, %d\n", MainSprite.x, MainSprite.y);

		// sort out semi-transparency against background
	//MainSprite.attribute |= mask1;
	//MainSprite.attribute |= mask2;

		// sort out sprite's CLUT position
	MainSprite.cx = 960;
	MainSprite.cy = 0;
}

	




void InitialiseGsSprite (GsSPRITE* sprite)
{
		// initialise sprite to dummy
	sprite->attribute = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->w = 0;
	sprite->h = 0;
	sprite->tpage = 0;
	sprite->u = 0;
	sprite->v = 0;
	sprite->cx = 0;
	sprite->cy = 0;
	sprite->r = 0x80;
	sprite->g = 0x80;
	sprite->b = 0x80;
	sprite->mx = 0;
	sprite->my = 0;
	sprite->scalex = ONE;
	sprite->scaley = ONE;
	sprite->rotate = 0;
}





	// set up background rectangle: main use is to stop sprites
	// having black-box edges
	// (sprites created very simply in PaintShopPro, 
	// converted via timutil) 

void ClearBackground (void)
{
	u_long mask;

	BackgroundRectangle.attribute = 0;
	mask = 1L << 30;
		
		// turn semi-transparency on
	BackgroundRectangle.attribute
	  		= (BackgroundRectangle.attribute | mask);

	BackgroundRectangle.x = 0;
	BackgroundRectangle.y = 0;
	BackgroundRectangle.w = SCREEN_WIDTH;
	BackgroundRectangle.h = SCREEN_HEIGHT;
	BackgroundRectangle.r = 0;
	BackgroundRectangle.g = 0;
	BackgroundRectangle.b = 0;
}




	// START & SELECT to quit
   
int DealWithControllerPad (void)
{
	PadStatus = PadRead();

		// quit
	if ((PadStatus & PADselect) && (PadStatus & PADstart))
		{
		return 0;
		}

	return 1;
}
