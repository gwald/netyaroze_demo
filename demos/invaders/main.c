/***************************************************************
*															   *
* Copyright (C) 1995 by Sony Computer Entertainment			   *
*			All rights Reserved								   *
*															   *
*		 L.Evans 		 					 23/09/96		   *
*															   *
***************************************************************/



//	Space invaders demo program 



/********		

				Lots of extensions to be made to this
				basic Space Invaders demo :-

				explosions when collisions occur
				different types of invaders
				different levels progressing in difficulty
				UFO going across the top
				destructible bases
				more limited firing (accuracy w.r.t. arcade)
				invaders' firing accurately
				finer detail graphics

				etc etc ......

**********/



// NOTE: see the file <batch1> for the necessary loading
// of main program and .TIM graphic files
// - they Must go into the exact memory destinations


// see makefile.mak for making


#include <libps.h>

#include "pad.h"




#define TRUE 1
#define FALSE 0


   // where the .TIM textures get loaded into main memory
#define INVADER_TIM_ADDRESS 0x80150000
#define SHIP_TIM_ADDRESS 0x80160000
#define SHOT_TIM_ADDRESS 0x80170000





	// sound business: directly nicked from other sample code
#define MVOL		127		
#define SVOL		127	   
#define DFILE		3		

#define VH_ADDR		0x80090000
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000



typedef struct {
	char	fname[64];
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

	
	// OTHER CHOICES FOR MUSIC:
	// gogo.seq, musi.seq (files on blackpsx startup disk)
static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\GOGO.SEQ;1", (void *)SEQ_ADDR, 0},
};


short vab, seq;

	










#define MAX_INVADERS 100
#define MAX_SHOTS 250

#define MAX_OBJECTS (MAX_INVADERS + MAX_SHOTS + 1)

	// allow 24 bytes per primitive for GPU work area
#define MAX_SIZEOF_PRIMITIVE 24



#define SCREEN_WIDTH 320 
#define SCREEN_HEIGHT 240

	// set up arbitrary walls
#define MIN_X 16
#define MAX_X 286
#define MIN_Y 30
#define MAX_Y 196

	// offset to make screen look right
#define SCREEN_X_OFFSET 0
#define SCREEN_Y_OFFSET 16

	// not exactly a three-D game 
	// hence only 1 z-position in ordering table
	// ie no actual z-discrimination at all
#define OT_LENGTH 1


#define MAX_X_SPEED 2
#define MAX_Y_SPEED	2



	// double-fine movement: 
	// not really seen to full advantage here ...   
typedef struct 
{
	double x, y;

	double width, height;

	double dx, dy;
} MovementData;







	
	// controller pad buffers: Don't access these directly
	// use PadRead to put pad status into an unsigned long variable
volatile u_char *bb0, *bb1;

	  

MovementData PlayersMovement;
GsSPRITE 	PlayersSprite;
int TimeSincePlayerLastFired = 0;

	
GsBOXF BackgroundRectangle;



GsOT		WorldOrderingTable[2];
	// 1 << OT_LENGTH: 2 to the power of OT_LENGTH
GsOT_TAG	OrderingTableTags[2][1<<OT_LENGTH];

	// work area: do not touch
PACKET		GpuPacketArea[2][MAX_OBJECTS * MAX_SIZEOF_PRIMITIVE];


	// controller pad buffer
u_long PadStatus;

int QuitFlag = FALSE;

	// NONE: used for empty <type> fields
#define NONE 32766

	// types of space invader: only the first type used as yet
#define INVADER_1 1
#define INVADER_2 2
#define INVADER_3 3	
#define INVADER_4 4

	// whose shot is it?
#define _PLAYER 1
#define _INVADER 2 


typedef struct
{
	int id;
	int alive;
	int type;			// INVADER_1, INVADER_2, etc
	GsSPRITE sprite;
	MovementData movement;
} Invader;


typedef struct 
{
	int id;
	int type;	 // _PLAYER, _INVADER  
	GsSPRITE sprite;
	MovementData movement;
} Shot;




Invader InvadersArray[MAX_INVADERS];
Shot ShotsArray[MAX_SHOTS];




int Level; 
int NumberOfLives;



typedef struct 
{
	char name[16];
	u_long points;
} Score;

Score HighScoreTable[5];
Score CurrentScore;



#define DEBUG 0


	// libgs globals: used for setting screen offsets 
extern DRAWENV GsDRAWENV;	/* DRAWENV of Gs */
extern DISPENV GsDISPENV;	/* DISPENV of Gs */




	// prototypes
	
int main(void);

void CrudestPause (int n);

void SetUpLevel1(void);
void RenewLevel (void);
int DealWithPlayersDeath(void);
int DealWithPlayerFinishingTenLevels (void);

void ClearScores(void);
void DealWithNewScore (Score* last, Score* table, int tableLength,
											int* outputPosition);

void InitialiseAll(void);
void InitialiseObjects (void);
void InitGsSprite (GsSPRITE* sprite);

int CreateNewInvader (int invaderType);
void DestroyInvader (int invaderID);
void DestroyAllInvaders (void);
int CountNumberOfInvaders (void);
int FindIDOfNthInvader (int n);
int CreateNewShot (int shotType);
void DestroyShot (int shotID);
void DestroyAllShots (void);

void InitialiseTexturesAndSprites(void);
void ClearBackground(void);

void DealWithControllerPad(void);

void SendAllInvadersTheOtherWay (void);
void RepositionAllInvadersOnTheLeft (void);
void HandleAllObjects(void);

int RectanglesOverlap (RECT* first, RECT* second);
int XOverlap (RECT* first, RECT* second);
int YOverlap (RECT* first, RECT* second);

void HandleInvadersShooting(void);

void PadInit(void);
u_long PadRead(void);

void InitSound(void);		
void PlaySound(void);
void StopSound(void);
void DatafileSearch(void);
void DatafileRead(void);





	// keep quantity within range
#define KeepWithinRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))
	


   

int main (void)
{
	int playerWantsAnotherGame;
	int activeBuffer;
	int count;
	u_long frameNumber = 0;
	int i;
		


	InitialiseAll();

		   

	while (QuitFlag == FALSE)
		{
		DealWithControllerPad();

	   	HandleAllObjects();		// movement, collision-detection, etc

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) GpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);

			// put background rectangle into ordering table
		GsSortBoxFill(&BackgroundRectangle,	
						&WorldOrderingTable[activeBuffer], 0);

			// put player sprite into ordering table
		GsSortSprite(&PlayersSprite,
							&WorldOrderingTable[activeBuffer], 0);

			// put invaders into ordering table
		for (i = 0; i < MAX_INVADERS; i++)
			{
			if (InvadersArray[i].alive == TRUE)
				{
				GsSortSprite(&(InvadersArray[i].sprite),
						&WorldOrderingTable[activeBuffer], 0);
				}
			}
		
			// put shots into ordering table	
		for (i = 0; i < MAX_SHOTS; i++)
			{
			if (ShotsArray[i].type != NONE)
				{
				GsSortSprite(&(ShotsArray[i].sprite),
						&WorldOrderingTable[activeBuffer], 0);
				}
			}				
	  
		DrawSync(0);		// wait for end of drawing							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);

		GsDrawOt(&WorldOrderingTable[activeBuffer]);

	 	FntPrint("Space Invaders!\n");
		FntPrint("Frame number: %d\n", frameNumber);
		FntPrint("Lives: %d\n", NumberOfLives);
		FntPrint("Score: %d\n", CurrentScore.points);
		
			// flush text onto screen itself
		FntFlush(-1);

		if (CountNumberOfInvaders() == 0)	  // end of level
			{
			Level++;
			if (Level > 10)
				{
				playerWantsAnotherGame 
					= DealWithPlayerFinishingTenLevels();
				if (playerWantsAnotherGame)
					{
					Level = 1;
					NumberOfLives = 5;
					CurrentScore.points = 0;
					SetUpLevel1();
					}
				else
					QuitFlag = TRUE;
				}
			else
				RenewLevel();	 // "next level" 
			}							
		
		if (NumberOfLives <= 0)
			{
			playerWantsAnotherGame = DealWithPlayersDeath();
			if (playerWantsAnotherGame)
				{
				Level = 1;
				NumberOfLives = 5;
				CurrentScore.points = 0;
				SetUpLevel1();
				}
			else
				QuitFlag = TRUE;
			}

		frameNumber++;
		TimeSincePlayerLastFired++;
		}

	StopSound();	   // vital if blackpsx is not to need resetting
	ResetGraph(3);
	return(0);
}




void CrudestPause (int n)
{
	int i;

	for (i = 0; i < n; i++)
		VSync(0);
}


   // NOTE: this function misnamed, all levels are identical at present
void SetUpLevel1 (void)
{
	int id;
	int i, j;
	
		// clear all invaders before starting again
	DestroyAllInvaders();				 

		// player starts stationary, at middle-bottom of screen
	PlayersMovement.x = MIN_X + (SCREEN_WIDTH/2);
	PlayersMovement.y = MAX_Y - PlayersSprite.h;
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;

#if DEBUG
	printf("player: sprite w %d, h %d\n",
			PlayersSprite.w, PlayersSprite.h);
	printf("tpage: %d\n", PlayersSprite.tpage);
	printf("x %d, y %d, cx %d, cy %d\n",
		PlayersSprite.x, PlayersSprite.y,
		PlayersSprite.cx, PlayersSprite.cy);
#endif
   
		// set up invaders
	for (i = 0; i < 4; i++)
		{
		for (j = 0; j < 8; j++)
			{
			id = CreateNewInvader(INVADER_1 + i);
			InvadersArray[id].movement.x = MIN_X + 20 + (20 * j);
			InvadersArray[id].movement.y = MIN_Y + 20 + (20 * i);
			InvadersArray[id].movement.dx = 1;
			}
		}
}




void RenewLevel (void)
{
	int i;

	DestroyAllShots();
			
	SetUpLevel1();		
}




	// simpler GUI loop: ask for new game or quit
int DealWithPlayersDeath (void)
{
	int newGame;
	int position;
	int count, activeBuffer;
		// these below: decent pause between choice and action
	int frameNow = 0, whenToLeaveLoop = -1, exitSet = FALSE;
	int i;

		// find position (if any) in high score table
		// update table
	DealWithNewScore( &CurrentScore, HighScoreTable, 5, &position);

	for (;;)
		{
		PadStatus = PadRead();

		if ( (PadStatus & PADL1) && (PadStatus & PADR1) )
			{
			if (exitSet == FALSE)
				{
				newGame = FALSE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;		
				}
			}  
		if (PadStatus & PADRdown)
			{
			if (exitSet == FALSE)
				{
				newGame = TRUE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;
				}
			}

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) GpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);

			// only background rectangle gets put into ordering table
		GsSortBoxFill(&BackgroundRectangle,	
						&WorldOrderingTable[activeBuffer], 0);	
	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);

		GsDrawOt(&WorldOrderingTable[activeBuffer]);

		FntPrint("You have died\n\n");
		FntPrint("Your final score was: %d\n\n", CurrentScore.points);
		if (position != -1)
			FntPrint("New high score! position %d\n\n", position+1);
		FntPrint("High Score Table\n");
		for (i = 0; i < 5; i++)
			{
			FntPrint("%s\t%d\n", 
				HighScoreTable[i].name,
				HighScoreTable[i].points);
			}
		FntPrint("\npress <R-down> for new game\n");
		FntPrint("press L1 and R1 to quit\n");	
		FntFlush(-1);
		frameNow++;
		if (whenToLeaveLoop == frameNow)
			break;
		}

	return newGame; 
}



	// simpler GUI loop: ask for new game or quit
	// this function Very similar to one above ...
int DealWithPlayerFinishingTenLevels (void)
{
	int newGame;
	int position;
	int count, activeBuffer;
		// these below: decent pause between choice and action
	int frameNow = 0, whenToLeaveLoop = -1, exitSet = FALSE;
	int i;

	DealWithNewScore( &CurrentScore, HighScoreTable, 5, &position);

	for (;;)
		{
		PadStatus = PadRead();

		if ( (PadStatus & PADL1) && (PadStatus & PADR1) )
			{
			if (exitSet == FALSE)
				{
				newGame = FALSE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;		
				}
			}  
		if (PadStatus & PADRdown)
			{
			if (exitSet == FALSE)
				{
				newGame = TRUE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;
				}
			}

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) GpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&WorldOrderingTable[activeBuffer], 0);	
	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);

		GsDrawOt(&WorldOrderingTable[activeBuffer]);

		FntPrint("You have finished ten levels\n\n");
		FntPrint("Your final score was: %d\n\n", CurrentScore.points);
		if (position != -1)
			FntPrint("New high score! position %d\n\n", position+1);
		FntPrint("High Score Table\n");
		for (i = 0; i < 5; i++)
			{
			FntPrint("%s\t%d\n", 
				HighScoreTable[i].name,
				HighScoreTable[i].points);
			}
		FntPrint("\npress <R-down> for new game\n");
		FntPrint("press L1 and R1 to quit\n");		
		FntFlush(-1);
		frameNow++;
		if (whenToLeaveLoop == frameNow)
			break;
		}

	return newGame; 
}





void ClearScores (void)
{
	int i, j;

	CurrentScore.points = 0;
	strcpy(CurrentScore.name, "Player One");

	for (i = 0; i < 5; i++)
		{
		HighScoreTable[i].points = (5-i) * 10000;
		for (j = 0; j < 8; j++)
			{
			HighScoreTable[i].name[j] = 65 + i;			// 'AAAAAAAA' etc
			}
		HighScoreTable[i].name[8] = '\0';
		}
}



	// output is -1 if not in table, else is position
	// assume table is ordered: 0 is Highest, tableLength-1 is lowest

void DealWithNewScore (Score* last, Score* table, int tableLength,
											int* outputPosition)
{
	int highestPosition = tableLength;
	int i;

	for (i = tableLength-1; i >= 0; i--)
		{
		if (last->points > table[i].points)
			{
			highestPosition = i;
			}
		}

	if (highestPosition == tableLength)			// not in table
		{
		*outputPosition = -1;
		}
	else	  // update table: shuffling down		
		{
		*outputPosition = highestPosition;

		for (i = *outputPosition; i <= tableLength-2; i++)
			{
			strcpy(table[i+1].name, table[i].name);
			table[i+1].points = table[i].points;
			}
		strcpy(table[*outputPosition].name, last->name);
		table[*outputPosition].points = last->points;
		}
}
	




void InitialiseAll (void)
{
	int i;

		// sorting out the sound data
	DatafileSearch();
	DatafileRead();

		// first graphical initialisations
	GsInitGraph(320, 240, 4, 0, 0);	
	GsDefDispBuff(0, 0, 0, 240);
	
		// set the screen offset (can change if want PAL mode)
	GsDISPENV.screen.x = SCREEN_X_OFFSET;
	GsDISPENV.screen.y = SCREEN_Y_OFFSET;
	// GsDISPENV.screen.w = SCREEN_WIDTH;
	// GsDISPENV.screen.h = SCREEN_HEIGHT;	
	   
   	for (i = 0; i < 2; i++) 
   		{
		WorldOrderingTable[i].length = OT_LENGTH;
		WorldOrderingTable[i].org = OrderingTableTags[i];
		}
		
	PadInit();
	
	FntLoad(960, 256);	
	FntOpen(16, 16, 256, 200, 0, 512);

	InitialiseObjects();

		// load .TIMs into memory, link with sprites
	InitialiseTexturesAndSprites();

	ClearBackground();

	Level = 1;
	NumberOfLives = 5;

	SetUpLevel1();

	ClearScores();

		// start playing sound
	InitSound();
	PlaySound();
}	  




void InitialiseObjects (void)
{
	int i;

	PlayersMovement.x = 0;
	PlayersMovement.y = 0;
	PlayersMovement.width = 44;
	PlayersMovement.height = 24;
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;
	InitGsSprite(&PlayersSprite);

	for (i = 0; i < MAX_INVADERS; i++)
		{
		InvadersArray[i].id = i;
		InvadersArray[i].alive = FALSE;
		InvadersArray[i].type = NONE;
		InvadersArray[i].movement.x = 0;
		InvadersArray[i].movement.y = 0;
		InvadersArray[i].movement.width = 16;
		InvadersArray[i].movement.height = 16;
		InvadersArray[i].movement.dx = 0;
		InvadersArray[i].movement.dy = 0;
		InitGsSprite( &(InvadersArray[i].sprite) );
		}

	for (i = 0; i < MAX_SHOTS; i++)	 
		{
		ShotsArray[i].id = i;
		ShotsArray[i].type = NONE;
		ShotsArray[i].movement.x = 0;
		ShotsArray[i].movement.y = 0; 
		ShotsArray[i].movement.width = 8;
		ShotsArray[i].movement.height = 8;
		ShotsArray[i].movement.dx = 0;
		ShotsArray[i].movement.dy = 0;
		InitGsSprite( &(ShotsArray[i].sprite) );
		}
}



void InitGsSprite (GsSPRITE* sprite)
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
	


	// the whole mechanism of invaders and shots
	// could easily be optimised
int CreateNewInvader (int invaderType)
{
	int i; 
	int found = -1;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == FALSE)
			{
			found = 1;
			break;
			}
		}

	if (found == -1)
		return -1;
	else
		{
		InvadersArray[i].alive = TRUE;
		InvadersArray[i].type = invaderType;
		return i;
		}
}


void DestroyInvader (int invaderID)
{
	InvadersArray[invaderID].alive = FALSE;
	InvadersArray[invaderID].type = NONE;
}



void DestroyAllInvaders (void)
{
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
			// this first bit is really redundant
		if (InvadersArray[i].alive == TRUE)
			{
			DestroyInvader(i);
			}
		InvadersArray[i].id = i;
		InvadersArray[i].alive = FALSE;
		InvadersArray[i].type = NONE;
		InvadersArray[i].movement.x = 0;
		InvadersArray[i].movement.y = 0;
		InvadersArray[i].movement.width = 16;
		InvadersArray[i].movement.height = 16;
		InvadersArray[i].movement.dx = 0;
		InvadersArray[i].movement.dy = 0;
		}
}




int CountNumberOfInvaders (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			count++;
			}
		}

	return count;
}


int FindIDOfNthInvader (int n)
{
	int id, count = 0;
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			count++;
			if (count == n)
				{
				id = i;
				break;
				}
			}
		}

	return id;
}





int CreateNewShot (int shotType)
{
	int i; 
	int found = -1;

	for (i = 0; i < MAX_SHOTS; i++)
		{
		if (ShotsArray[i].type == NONE)
			{
			found = 1;
			break;
			}
		}

	if (found == -1)
		return -1;
	else
		{
		ShotsArray[i].type = shotType;
		return i;
		}
}


void DestroyShot (int shotID)
{
	ShotsArray[shotID].type = NONE;
}


void DestroyAllShots (void)
{
	int i;

	for (i = 0; i < MAX_SHOTS; i++)
		{
		if (ShotsArray[i].type != NONE)
			DestroyShot(i);
		}
}




	


	// main business:
	// get pointer to TIM in memory; skip header
	// get tim info, into a GsIMAGE structure
	// load texture and CLUT into frame buffer
	// get texture page ID
	// assign relevant variables to GsSPRITE fields
	
	// NOTE: look at .TIM files using the tool <timutil>,
	// .TIM files store where they expect their pixels and CLUT data
	// to be stored in VRAM; want to ensure proper match
	// between where you load .TIMs and where they expext to go

void InitialiseTexturesAndSprites (void)
{
	u_long* timPointer;
	GsIMAGE image1, image2, image3;		// probably don't need three 
	RECT frameBufferPosition; 
	u_short texturePageID1, texturePageID2, texturePageID3;
	u_long eightBitTextureAttributeMask = 1L << 24;
	u_long mask1 = 1L << 29;   		// semi-transparency
	u_long mask2 = 1L << 30;		// semi-transparency
	int i;
	
		// player's ship: 8-bit
	timPointer = (u_long *) SHIP_TIM_ADDRESS;
	timPointer++; 		// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image1); 

		// where texture will go							
	frameBufferPosition.x = 320;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image1.pw; 
	frameBufferPosition.h = image1.ph;

#if DEBUG
	printf("px %d, py %d\n", image1.px, image1.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image1.pixel);
	DrawSync(0);		// probably not necessary
						// but LoadImage is non-blocking

		// load clut
	frameBufferPosition.x = image1.cx;
	frameBufferPosition.y = image1.cy;
	frameBufferPosition.w = image1.cw;
	frameBufferPosition.h = image1.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image1.pmode);
		// this says if texture is 4-bit, 8-bit or 16-bit (or multi-mode)
	
	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image1.clut);
	DrawSync(0); 
	
   		// sort player's sprite
	texturePageID1 = GetTPage(1, 0, 320, 0);
#if DEBUG
	printf("tpage 1 is %hd\n", texturePageID1);
#endif

	PlayersSprite.w = image1.pw * 2;	 // 8-bit clut: compressed in VRAM
	PlayersSprite.h = image1.ph;
	PlayersSprite.tpage = texturePageID1;
	PlayersSprite.cx = image1.cx;
	PlayersSprite.cy = image1.cy;
	PlayersSprite.attribute |= eightBitTextureAttributeMask;
	PlayersSprite.attribute |= mask1;	// semi-transparency
	PlayersSprite.attribute |= mask2;





		// now: invader sprites: 8-bit
	timPointer = (u_long *) INVADER_TIM_ADDRESS;
	timPointer++;			// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image2);

	frameBufferPosition.x = 320;
	frameBufferPosition.y = 256;
	frameBufferPosition.w = image2.pw;	  // 8-bit CLUT
	frameBufferPosition.h = image2.ph;
	
#if DEBUG
	printf("px %d, py %d\n", image2.px, image2.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image2.pixel);
	DrawSync(0);

	frameBufferPosition.x = image2.cx;
	frameBufferPosition.y = image2.cy;
	frameBufferPosition.w = image2.cw;
	frameBufferPosition.h = image2.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image2.pmode);

	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image2.clut);
	DrawSync(0);

		// sort invaders' sprites
	texturePageID2 = GetTPage(1, 0, 320, 256);
#if DEBUG
	printf("tpage 2 is %hd\n", texturePageID2);
#endif

	for (i = 0; i < MAX_INVADERS; i++)
		{
		InvadersArray[i].sprite.w = image2.pw * 2;		// 8-bit clut
		InvadersArray[i].sprite.h = image2.ph;
		InvadersArray[i].sprite.tpage = texturePageID2;
		InvadersArray[i].sprite.cx = image2.cx;
		InvadersArray[i].sprite.cy = image2.cy;
		InvadersArray[i].sprite.attribute |= eightBitTextureAttributeMask;
		InvadersArray[i].sprite.attribute |= mask1;
		InvadersArray[i].sprite.attribute |= mask2;
		InvadersArray[i].sprite.scalex = ONE/2;
		InvadersArray[i].sprite.scaley = ONE/2;
		}
 

		// now: shot sprite: 4-bit
	timPointer = (u_long *) SHOT_TIM_ADDRESS;
	timPointer++;			// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image3);

	frameBufferPosition.x = 640;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image3.pw;		// 4-bit CLUT
	frameBufferPosition.h = image3.ph;

#if DEBUG
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif
	LoadImage(&frameBufferPosition, image3.pixel);
	DrawSync(0);

	frameBufferPosition.x = image3.cx;
	frameBufferPosition.y = image3.cy;
	frameBufferPosition.w = image3.cw;
	frameBufferPosition.h = image3.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image3.pmode);

	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image3.clut);
	DrawSync(0);

		// sort shots' sprites
	texturePageID3 = GetTPage(0, 0, 640, 0);
#if DEBUG
	printf("tpage 3 is %hd\n", texturePageID3);
#endif

	for (i = 0; i < MAX_SHOTS; i++)
		{
		ShotsArray[i].sprite.w = image3.pw * 4;
		ShotsArray[i].sprite.h = image3.ph;
		ShotsArray[i].sprite.tpage = texturePageID3;
		ShotsArray[i].sprite.cx = image3.cx;
		ShotsArray[i].sprite.cy = image3.cy;
		ShotsArray[i].sprite.scalex = ONE/4;
		ShotsArray[i].sprite.scaley = ONE/4; 
		}
}


	// set up background rectangle: main use is to stop sprites
	// having black-box edges
	// (sprites created very simply in PaintShopPro, 
	// converted via timutil) 
void ClearBackground (void)
{
	u_long mask;

	BackgroundRectangle.attribute = 0;
	mask = 1L << 30;			// semi-transparency
   					// used to stop sprites having black-box edges
		
		// turn semi-transparency on
	BackgroundRectangle.attribute
			= (BackgroundRectangle.attribute | mask);	

	BackgroundRectangle.x = 0;
	BackgroundRectangle.y = 0;
	BackgroundRectangle.w = SCREEN_WIDTH;
	BackgroundRectangle.h = SCREEN_HEIGHT;
	BackgroundRectangle.r = 0;	   // black; actual screen looks dark blue 
	BackgroundRectangle.g = 0;	   // seem to get mix of black with the
	BackgroundRectangle.b = 0;	   // default of light blue
}	


	
   
void DealWithControllerPad (void)
{
		// controller pad buffer status
		// bitwise-AND with the macros defined in pad.h
		// to access all the standard controller pad buttons
	PadStatus = PadRead();

		// quit
	if ((PadStatus & PADselect) && (PadStatus & PADstart))
		{
		QuitFlag = TRUE;
		return;
		}

		// pause
	if (PadStatus & PADL1)
		{
		for (;;)
			{
			PadStatus = PadRead();
			if ((PadStatus & PADL1) == FALSE)
				break;
			}
		}
				
		// player moving 
	if (PadStatus & PADLleft)
		{
		PlayersMovement.dx -= 1;
		KeepWithinRange(PlayersMovement.dx, 
			-MAX_X_SPEED, MAX_X_SPEED);
		}
	if (PadStatus & PADLright)
		{
		PlayersMovement.dx += 1;
		KeepWithinRange(PlayersMovement.dx, 
			-MAX_X_SPEED, MAX_X_SPEED);
		}

		// firing a shot
	if (PadStatus & PADRup)
		{
		if (TimeSincePlayerLastFired > 10)	  // limits on firing
			{
			int shotID = CreateNewShot(_PLAYER);
				// put shot above player, moving upwards
			ShotsArray[shotID].movement.x = PlayersMovement.x + 24;
			ShotsArray[shotID].movement.y = PlayersMovement.y - 2;
			ShotsArray[shotID].movement.dx = 0;
				// HERE we set the player's shot speed
			ShotsArray[shotID].movement.dy = -2.5;
			TimeSincePlayerLastFired = 0;
			}
		}
}





	// flip direction of all invaders
void SendAllInvadersTheOtherWay (void)
{
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			InvadersArray[i].movement.dx = -(InvadersArray[i].movement.dx);
			InvadersArray[i].movement.y++;
			}
		}
}




void RepositionAllInvadersOnTheLeft (void)
{
	double leastX = 1500;
	double amountMove;
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			if (InvadersArray[i].movement.x < leastX)
				leastX = InvadersArray[i].movement.x;
			}
		}

	if (leastX != 1500)		// more than one invader
		{
		amountMove = leastX - MIN_X;
		for (i = 0; i < MAX_INVADERS; i++)	
			{
			if (InvadersArray[i].alive == TRUE)
				{
				InvadersArray[i].movement.x -= amountMove;
				}
			}
		}
}



	// deal with movements and collisions
	// scope for optimisation here is Massive

void HandleAllObjects (void)
{
	RECT player, invader, shot;
	int i, j;

	PlayersMovement.x += PlayersMovement.dx;
	PlayersMovement.y += PlayersMovement.dy;

		// player stops at edge of screen
	if (PlayersMovement.x > MAX_X-PlayersMovement.width)
		{
		PlayersMovement.x = MAX_X-PlayersMovement.width;
		PlayersMovement.dx = 0;
		}
	else if (PlayersMovement.x < MIN_X)
		{
		PlayersMovement.x = MIN_X;
		PlayersMovement.dx = 0;
		}

	PlayersSprite.x = (short int) PlayersMovement.x;
	PlayersSprite.y = (short int) PlayersMovement.y;

	player.x = (short int) PlayersMovement.x;
	player.y = (short int) PlayersMovement.y;
	player.w = (short int) PlayersMovement.width;
	player.h = (short int) PlayersMovement.height;

	invader.w = 16;	   
	invader.h = 16;

	shot.w = 8;
	shot.h = 8;

		// if any invader about to jump off screen,
		// send entire lot the other way
	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			if (InvadersArray[i].movement.x + InvadersArray[i].movement.dx
				> MAX_X-16)
					{
					SendAllInvadersTheOtherWay();
					break;
					}
			if (InvadersArray[i].movement.x + InvadersArray[i].movement.dx
				< MIN_X)
					{
					SendAllInvadersTheOtherWay();
					break;
					}
			}
		}

		// now move invaders
	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive == TRUE)
			{
			InvadersArray[i].movement.x += InvadersArray[i].movement.dx;
			InvadersArray[i].movement.y += InvadersArray[i].movement.dy;

			if (InvadersArray[i].movement.y > MAX_Y-16)
				{
				QuitFlag = TRUE;
				return;
				}

			InvadersArray[i].sprite.x 
				= (short int) InvadersArray[i].movement.x;
			InvadersArray[i].sprite.y 
				= (short int) InvadersArray[i].movement.y;

				// detect collisions of invaders with player's ship
			invader.x = (short int) InvadersArray[i].movement.x;
			invader.y = (short int) InvadersArray[i].movement.y;
			if (RectanglesOverlap(&player, &invader))
				{
				CrudestPause(30);
				DestroyInvader(i);
				NumberOfLives--;
				DestroyAllShots();
				PlayersMovement.x = MIN_X + (SCREEN_WIDTH/2);
				PlayersMovement.y = MAX_Y - PlayersSprite.h;
				PlayersMovement.dx = 0;
				PlayersMovement.dy = 0;
				RepositionAllInvadersOnTheLeft();
				}

			}
		}	
	
	HandleInvadersShooting();

		// move shots
	for (i = 0; i < MAX_SHOTS; i++)
		{
		if (ShotsArray[i].type != NONE)
			{
			ShotsArray[i].movement.x += ShotsArray[i].movement.dx;
			ShotsArray[i].movement.y += ShotsArray[i].movement.dy;
							
			ShotsArray[i].sprite.x 
				= (short int) ShotsArray[i].movement.x;
			ShotsArray[i].sprite.y 
				= (short int) ShotsArray[i].movement.y;
						
			if ( (ShotsArray[i].movement.y > 
					(MAX_Y-ShotsArray[i].movement.height)) 
					|| (ShotsArray[i].movement.y < MIN_Y) )
				{
				DestroyShot(i);
				}

			shot.x = (short int) ShotsArray[i].movement.x;
			shot.y = (short int) ShotsArray[i].movement.y;

				// invaders' shots can only collide with player's ship
			if (ShotsArray[i].type == _INVADER)
				{
				if (RectanglesOverlap(&player, &shot))
					{
					CrudestPause(30);
					DestroyShot(i);
					NumberOfLives--;
					DestroyAllShots();
					PlayersMovement.x = MIN_X + (SCREEN_WIDTH/2);
					PlayersMovement.y = MAX_Y - PlayersSprite.h;
					PlayersMovement.dx = 0;
					PlayersMovement.dy = 0;
					RepositionAllInvadersOnTheLeft();
					}
				}
			else	// player's shots can only collide with invaders
				{
				for (j = 0; j < MAX_INVADERS; j++)
					{
					if (InvadersArray[j].alive == TRUE)
						{
						invader.x = (short int) InvadersArray[j].movement.x;
						invader.y = (short int) InvadersArray[j].movement.y;
						if (RectanglesOverlap(&shot, &invader))
							{
							DestroyShot(i);
							CurrentScore.points 
								+= (50 * InvadersArray[j].type);
							DestroyInvader(j);
							break;
							}
						}
					}
				}
			}
		}
}




	// this done very simply: no notion of aiming at player at all
	// but very easy to add
void HandleInvadersShooting (void)
{
	int numberOfInvaders, whichOne, invaderID;
	int shotID;

	if (rand() % 60 == 0)		// average 4 times per frame
		{
		numberOfInvaders = CountNumberOfInvaders();
		whichOne = (rand() % numberOfInvaders) + 1;
		invaderID = FindIDOfNthInvader(whichOne);

#if DEBUG	
		printf("whichOne: %d, invaderID: %d\n",
				whichOne, invaderID);
#endif

		shotID = CreateNewShot(_INVADER);

			// put shot below invader, moving down
		ShotsArray[shotID].movement.x 
			= InvadersArray[invaderID].movement.x + 8;
		ShotsArray[shotID].movement.y 
			= InvadersArray[invaderID].movement.y + 17;
			// HERE we set the invaders' shot speed
		ShotsArray[shotID].movement.dy = 1; 
		}			
}




	// scope for optimisation again;
	// macrofication at the very least
int RectanglesOverlap (RECT* first, RECT* second)
{
   int boolean;

	boolean = XOverlap(first,second) && YOverlap(first, second);

	return boolean;
}


int XOverlap (RECT* first, RECT* second)
{
	if (first->x >= second->x)
		{
		return (first->x < second->x + second->w);
		}
  	else
		{
		return (first->x + first->w > second->x);
		}
}


int YOverlap (RECT* first, RECT* second)
{
	if (first->y >= second->y)
		{
		return (first->y < second->y + second->h);
		}
	else
		{
		return (first->y + first->h > second->y);
		}
}

   


  
	// set up buffers
void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}




	// nice and comprehensible ...
	// see the documentation for details
	// BUT this makes pad-handling functions very easy,
	// see DealWithControllerPad above
u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}





	// this ensures that de-initialisation done properly
	// ie only when initialisation succeeded
	// ==> don't have to keep resetting the black psx
int SoundSuccess;


	// NOTE: printf prints to the PC MS-DOS box siocons console
	// NOT to the black playstation screen

	// this sound code taken straight from other samples
void InitSound (void)
{
	vab = SsVabTransfer( (u_char*)VH_ADDR, (u_char*)VB_ADDR, -1, 1 );
	if (vab < 0) {
		printf("SsVabTransfer failed (%d)\n", vab);
		SoundSuccess = FALSE;
		return;
	}

   
   	seq = SsSeqOpen((u_long *)SEQ_ADDR, vab);
	if (seq < 0)
		{
		printf("SsSeqOpen failed (%d)\n", seq);
		SoundSuccess = FALSE;
		}
	else
		SoundSuccess = TRUE;
}





void PlaySound (void)
{
	if (SoundSuccess == TRUE)
		{
		SsSetMVol(MVOL, MVOL);			
		SsSeqSetVol(seq, SVOL, SVOL);		
		SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY); 
		}
}



void StopSound (void)
{
	if (SoundSuccess == TRUE)
		{
		SsSeqStop(seq);			   
		VSync(0);
		VSync(0);
		SsSeqClose(seq);			
		SsVabClose(vab);
		}			
}


	// this CD-file-access code taken straight from other samples
void DatafileSearch (void)
{
	int i, j;

	for (i = 0; i < DFILE; i++) 
		{	   
		for (j = 0; j < 10; j++) 
			{	
			if (CdSearchFile(&(dfile[i].finfo), dfile[i].fname) != 0)
				break;		
			else
				printf("%s not found.\n", dfile[i].fname);
			}
		}
}




void DatafileRead (void)
{
	int i, j;
	int cnt;

	for (i = 0; i < DFILE; i++) 
		{		
		for (j = 0; j < 10; j++) 
			{	
			CdReadFile(dfile[i].fname, dfile[i].addr,dfile[i].finfo.size);

			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	

			if (cnt == 0)
				break;	
			}
		}
}


	


	
  	
