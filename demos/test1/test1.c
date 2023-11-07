
#include <libps.h>


#include "pad.h"

#include "2d1.h"

#include "rand.h"

#include "text_str.h"

#include "address.h"

#include "sound.h"

#include "storescr.h"





GsIMAGE FireTextureInfo;




	// main prototypes
void main (void);

void InitialiseAll (void);

void CleanupAndExit (void);

void DealWithControllerPad (void);









int SCREEN_WIDTH = 320;		 
int SCREEN_HEIGHT = 240;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

   

#define OT_LENGTH	9		



static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH];


int QuitFlag = FALSE;

int frameNumber = 0;


GsSPRITE TheSprite;

int StringX = -10, StringY = -30;



void main (void)
{
	int	hsync = 0;
	int bufferIndex;			  // buffer index
	
	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{		
		FntPrint("frame: %d\n", frameNumber);
		RegisterTextStringForDisplay ("display test", StringX, StringY);				

		frameNumber++;

		DealWithControllerPad();

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		GsSortSprite( &TheSprite, &Wot[bufferIndex], 0);

		DisplayTextStrings (&Wot[bufferIndex]);

		hsync = VSync(0);		 
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[bufferIndex]);
		GsDrawOt(&Wot[bufferIndex]);
		bufferIndex = GsGetActiveBuff();
		FntFlush(-1);
		}

	CleanupAndExit();
}






void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,SCREEN_HEIGHT);

	GsInit3D();		   

		// sort our only ordering table
	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

		// sort our single texture and single sprite
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	LinkSpriteToImageInfo ( &TheSprite, &FireTextureInfo);
		 
		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( 50, 100, 256, 200, 0, 512);
}







void CleanupAndExit (void)
{
	//StoreScreen2 ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}





void DealWithControllerPad (void)
{
	long pad;

	pad = PadRead();

	if (pad & PADselect && pad & PADstart)
		QuitFlag = TRUE;

	if (pad & PADstart)
		{
		for (;;)
			{
			pad = PadRead();
			if ( (pad & PADstart) == 0)
				break;
			}
		}

	if (pad & PADLleft)
		TheSprite.x--;
	if (pad & PADLright)
		TheSprite.x++;
	if (pad & PADLup)
		TheSprite.y--;
	if (pad & PADLdown)
		TheSprite.y++;

	if (pad & PADRleft)
		StringX--;
	if (pad & PADRright)
		StringX++;
	if (pad & PADRup)
		StringY--;
	if (pad & PADRdown)
		StringY++;

	if (pad & PADR1)
		TurnOnSound (1, 0, 60, 1, 10, 24, 24);
	if (pad & PADR2)
		TurnOnSound (4, 0, 60, 1, 20, 48, 48);
}