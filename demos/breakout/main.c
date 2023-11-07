/***************************************************************
*															   *
* Copyright (C) 1995 by Sony Computer Entertainment			   *
*			All rights Reserved								   *
*															   *
*		 L.Evans 		 					 25/09/96		   *
*															   *
***************************************************************/



// 	Breakout/Pong demo program 

   



// NOTE: see the file <batch> for the necessary loading
// of main program and .TIM graphic files
// - they Must go into the exact memory destinations


// see makefile.mak for making			





#include <libps.h>

#include "pad.h"

#include "balltex.h"			// ball's texture pattern 

#include "myassert.h"


#if 0
#include <stdio.h>
#include <string.h>
#endif


#include <mwdebugio.h>






#define TRUE 1
#define FALSE 0



#define PLUS 1
#define MINUS -1

#define SIGN_OF(number)  (((number) >= 0) ? PLUS : MINUS)



#define BAT_TIM_ADDRESS 0x80160000
#define BRICK_TIM_ADDRESS 0x80170000






	// sound stuff ....
#define MVOL		127		
#define SVOL		127	   
#define DFILE		3				// number of files need to access

#define VH_ADDR		0x80090000		// where sound images go in memory
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000



typedef struct {
	char	fname[64];
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

	
	// OTHER CHOICES FOR MUSIC:
	// gogo.seq, musi.seq
static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\GOGO.SEQ;1", (void *)SEQ_ADDR, 0},
};


short vab, seq;		 // handlers

	











#define MAX_BRICKS 200

#define MAX_OBJECTS (MAX_BRICKS + 2)		
	// bricks + player's bat + ball

	// allow 24 bytes per primitive	for GPU work area
#define MAX_SIZEOF_PRIMITIVE 24



#define SCREEN_WIDTH 320 
#define SCREEN_HEIGHT 240

	// set up arbitrary walls
#define MIN_X 16
#define MAX_X 288
#define MIN_Y 30
#define MAX_Y 196

	// offset to make screen look right
#define SCREEN_X_OFFSET 0
#define SCREEN_Y_OFFSET 16


	// not exactly a three-D game ...
	// hence only 1 z-position in ordering table
	// ie no actual z-discrimination at all
#define OT_LENGTH 1

	// maximum speeds at which player's bat can move
#define MAX_PLAYER_X_SPEED 4
#define MAX_PLAYER_Y_SPEED	0

#define MAX_BALL_X_SPEED 5
#define MAX_BALL_Y_SPEED 5


	// refer to edges and corners of shapes, eg of rectangles
#define TOP 1
#define BOTTOM 2
#define LEFT 3
#define RIGHT 4
#define TOP_LEFT 5
#define TOP_RIGHT 6
#define BOTTOM_LEFT 7
#define BOTTOM_RIGHT 8
#define MIDDLE 9
#define OVERLAP 10




	  	 
typedef struct 
{
	int x, y;

	int width, height;

	int dx, dy;
} MovementData;







	
	// controller pad buffers: Don't access these directly
	// use PadRead to put pad status into an unsigned long variable
volatile u_char *bb0, *bb1;

	  

MovementData 	PlayersMovement;
GsSPRITE 		PlayersSprite;
int PlayerSpeed;


MovementData 	BallsMovement;
GsSPRITE		BallsSprite;


GsBOXF BackgroundRectangle;



GsOT		WorldOrderingTable[2];
	// 1 << OT_LENGTH: 2 to the power of OT_LENGTH
GsOT_TAG	OrderingTableTags[2][1<<OT_LENGTH];


PACKET		GpuPacketArea[2][MAX_OBJECTS * MAX_SIZEOF_PRIMITIVE];


	// controller pad buffer: easiest way to access is
	// bitwise-AND with constants in pad.h 
u_long PadStatus;


int QuitFlag = FALSE;


#define NONE 32766


#define FULL_BRICK 100
#define HALF_BRICK 101

typedef struct
{
	int id;
	int alive;
	int type;				// FULL_BRICK, HALF_BRICK
	int toughness;			// number of hits before destruction
	GsSPRITE sprite;
	MovementData movement;
} Brick;


Brick BricksArray[MAX_BRICKS];




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
void PauseUntilL1 (void);

void SetUpLevel1(void);
int DealWithPlayersDeath(void);
int DealWithPlayerFinishingTenLevels (void);

void ClearScores(void);
void DealWithNewScore (Score* last, Score* table, int tableLength,
											int* outputPosition);

void InitialiseAll(void);
void InitialiseObjects (void);
void InitGsSprite (GsSPRITE* sprite);

int CreateNewBrick (int type, int toughness);
void DestroyBrick (int brickID);
int CountNumberOfBricks (void);
void DestroyAllBricks (void);

void RestartTheBall (void);
void RestartPlayersBat (void);

void InitialiseTexturesAndSprites(void);
void ClearBackground(void);

void DealWithControllerPad(void);

void HandleAllObjects(void);

void MoveBallOutOfBat (void);

int RectanglesOverlap (RECT* first, RECT* second);
int XOverlap (RECT* first, RECT* second);
int YOverlap (RECT* first, RECT* second);
int RectanglesTouch (RECT* first, RECT* second);
int XTouch (RECT* first, RECT* second);
int YTouch (RECT* first, RECT* second);
int FindWhereRectanglesTouch (RECT* first, RECT* second);
 
void PadInit(void);
u_long PadRead(void);

void InitSound(void);		
void PlaySound(void);
void StopSound(void);
void DatafileSearch(void);
void DatafileRead(void);



	// amount that player's bat influences the ball
float Coefficient = 0.75;		  // USED TO BE type 'DOUBLE'


	// keep quantity within range
#define KeepWithinRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))


 	
	
	
int main (void)
{
	int playerWantsAnotherGame;
	int activeBuffer;
	int count;
	u_long frameNumber = 0;
	int i;
		
		// NOT FOR GNU ....
	MWbload("oden3.tim", (long*)BRICK_TIM_ADDRESS);
	MWbload("break3.tim", (long*)BAT_TIM_ADDRESS);
	
	
	InitialiseAll();

		   

	while (QuitFlag == FALSE)
		{
		DealWithControllerPad();

	   	HandleAllObjects();

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) GpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&WorldOrderingTable[activeBuffer], 0);

		GsSortSprite(&PlayersSprite,
							&WorldOrderingTable[activeBuffer], 0);

		GsSortSprite(&BallsSprite,
							&WorldOrderingTable[activeBuffer], 0);

		for (i = 0; i < MAX_BRICKS; i++)
			{
			if (BricksArray[i].alive == TRUE)
				{
				GsSortSprite(&(BricksArray[i].sprite),
						&WorldOrderingTable[activeBuffer], 0);
				}
			}
	 	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &WorldOrderingTable[activeBuffer]);

		GsDrawOt(&WorldOrderingTable[activeBuffer]);

	 	FntPrint("Breakout!\n");
		FntPrint("Frame number: %d\n", frameNumber);
		FntPrint("Lives: %d\n", NumberOfLives);
		FntPrint("Score: %d\n", CurrentScore.points);
		
		FntFlush(-1);

		if (CountNumberOfBricks() == 0)	  // time for next level
			{
			CrudestPause(30);
			Level++;
			if (Level > 10)
				{
				playerWantsAnotherGame 
					= DealWithPlayerFinishingTenLevels();
				if (playerWantsAnotherGame)
					{
					Level = 1;
					NumberOfLives = 5;
					DestroyAllBricks();
					CurrentScore.points = 0;
					SetUpLevel1();
					}
				else
					QuitFlag = TRUE;
				}
			else
				SetUpLevel1();	
			}							
		
		if (NumberOfLives <= 0)			// player has lost
			{
			CrudestPause(30);
			playerWantsAnotherGame = DealWithPlayersDeath();
			if (playerWantsAnotherGame)
				{
				Level = 1;
				NumberOfLives = 5;
				DestroyAllBricks();
				CurrentScore.points = 0;
				SetUpLevel1();
				}
			else
				QuitFlag = TRUE;
			}

		frameNumber++;
		}

		// cleanup
	StopSound();
	ResetGraph(3);
	return(0);
}






void CrudestPause (int n)
{
	int i;

	for (i = 0; i < n; i++)
		VSync(0);
}




void PauseUntilL1 (void)
{
	for (;;)
		{
		PadStatus = PadRead();

		VSync(0);

		if (PadStatus & PADL1)
			break;
		}
}
	   	  	
	 



  


	// all levels are identical at present
void SetUpLevel1 (void)
{
	int id;
	int i, j;				 

	RestartPlayersBat();

	RestartTheBall();
   
		// HERE: set up bricks
	for (i = 0; i < 3; i++)
		{
		for (j = 0; j < 15; j++)
			{
			id = CreateNewBrick(FULL_BRICK, 1);
			BricksArray[id].movement.x = MIN_X + (18 * j);
			BricksArray[id].movement.y = MIN_Y + 29 + (i * 18);
			}
		}
	for (i = 0; i < 2; i++)
		{
		for (j = 0; j < 14; j++)
			{
			id = CreateNewBrick(FULL_BRICK, 1);
			BricksArray[id].movement.x = MIN_X + 9 + (18 * j);
			BricksArray[id].movement.y = MIN_Y + 38 + (i * 18);
			}
		id = CreateNewBrick(HALF_BRICK, 1);
		BricksArray[id].movement.x = MIN_X;
		BricksArray[id].movement.y = MIN_Y + 38 + (i * 18);
		id = CreateNewBrick(HALF_BRICK, 1);
		BricksArray[id].movement.x = MIN_X + (29 * 9);
		BricksArray[id].movement.y = MIN_Y + 38 + (i * 18);
		}
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
		HighScoreTable[i].points = (5-i) * 1000;
		for (j = 0; j < 8; j++)
			{
			HighScoreTable[i].name[j] = 65 + i;
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

		// sound file initialisations
	DatafileSearch();
	DatafileRead();

		// first graphical initialisations
	GsInitGraph(320, 240, 4, 0, 0);	
	GsDefDispBuff(0, 0, 0, 240);
	
		// set the screen offset (varies with PAL/NTSC)
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

		// load .TIM files into memory, link with sprites
	InitialiseTexturesAndSprites();

	ClearBackground();

	Level = 1;
	NumberOfLives = 5;

	SetUpLevel1();

	ClearScores();

	InitSound();
	PlaySound();
}	  




void InitialiseObjects (void)
{
	int i;

		// sort out the player's bat
	PlayersMovement.x = 0;
	PlayersMovement.y = 0;
	PlayersMovement.width = 40;		
	PlayersMovement.height = 8;	
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;
	InitGsSprite(&PlayersSprite);
	PlayerSpeed = 3;

		// sort out the ball
	BallsMovement.x = 0;
	BallsMovement.y = 0;
	BallsMovement.width = 7;		   
	BallsMovement.height = 7;		   
	BallsMovement.dx = 0;
	BallsMovement.dy = 0;
	InitGsSprite(&BallsSprite);
	
		// sort out the bricks
	for (i = 0; i < MAX_BRICKS; i++)
		{
		BricksArray[i].id = i;
		BricksArray[i].alive = FALSE;
		BricksArray[i].type = NONE;
		BricksArray[i].toughness = NONE;
		BricksArray[i].movement.x = 0;
		BricksArray[i].movement.y = 0;
		BricksArray[i].movement.width = 0;	   
		BricksArray[i].movement.height = 0;   
		BricksArray[i].movement.dx = 0;
		BricksArray[i].movement.dy = 0;
		InitGsSprite( &(BricksArray[i].sprite) );
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
	


	// just using a global array with IDs to track objects
	// scope for improvement here
int CreateNewBrick (int type, int toughness)
{
	int id; 
	int found = -1;
	int i;

	for (i = 0; i < MAX_BRICKS; i++)
		{
		if (BricksArray[i].alive == FALSE)
			{
			found = 1;
			id = i;
			break;
			}
		}

	if (found == -1)
		return -1;
	else
		{
		BricksArray[id].alive = TRUE;
		BricksArray[id].type = type;
		BricksArray[id].toughness = toughness;

		if (type == FULL_BRICK)
			{
			BricksArray[id].movement.width = 16;
			BricksArray[id].movement.height = 8;
			}
		else		// HALF_BRICK
			{
			BricksArray[id].movement.width = 8;
			BricksArray[id].movement.height = 8;
			BricksArray[id].sprite.scalex = ONE/8;
			}

		return id;
		}
}


void DestroyBrick (int brickID)
{
	BricksArray[brickID].alive = FALSE;
	BricksArray[brickID].type = NONE;
	BricksArray[brickID].toughness = NONE;
}



int CountNumberOfBricks (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_BRICKS; i++)
		{
		if (BricksArray[i].alive == TRUE)
			{
			count++;
			}
		}

	return count;
}




void DestroyAllBricks (void)
{
	int i;

	for (i = 0; i < MAX_BRICKS; i++)
		{
		if (BricksArray[i].alive == TRUE)
			{
			BricksArray[i].id = i;
			BricksArray[i].alive = FALSE;
			BricksArray[i].type = NONE;
			BricksArray[i].toughness = NONE;
			BricksArray[i].movement.x = 0;
			BricksArray[i].movement.y = 0;
			BricksArray[i].movement.width = 0;	   
			BricksArray[i].movement.height = 0;   
			BricksArray[i].movement.dx = 0;
			BricksArray[i].movement.dy = 0;
			}
		}
}






void RestartTheBall (void)
{
		// ball starts at centre, moving down
	BallsMovement.x = SCREEN_WIDTH/2 - (BallsMovement.width/2);
	BallsMovement.y = SCREEN_HEIGHT/2 - (BallsMovement.height/2);
	BallsMovement.dx = 0;
	BallsMovement.dy = 1;
}

	
void RestartPlayersBat (void)
{
		// player starts stationary at bottom-middle of screen
	PlayersMovement.x = SCREEN_WIDTH/2 - (PlayersMovement.width/2);
	PlayersMovement.y = MAX_Y - PlayersMovement.height;
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;
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

	// ALSO: have used the textures provided in "balltex.h"
	// taken straight from sample code;
	// this means we get the texture data (pixels and CLUT)
	// directly from a C array;
	// easy to use, but not generally useful without the tools
	// that convert .TIM files into C-array files like balltex.h

void InitialiseTexturesAndSprites (void)
{
	u_long* timPointer;
	GsIMAGE image1, image2;
	RECT frameBufferPosition; 
	u_short texturePageID1, texturePageID2, texturePageID3;
	u_long eightBitTextureAttributeMask = 1L << 24;
	u_long sixteenBitTextureAttributeMask = 1L << 25;
	u_long mask1 = 1L << 29;
	u_long mask2 = 1L << 30;
	int i;
	
		// player's bat: 8-bit
	timPointer = (u_long *) BAT_TIM_ADDRESS;
	timPointer++; 		// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image1); 

		// load tpage							
	frameBufferPosition.x = 320;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image1.pw; 
	frameBufferPosition.h = image1.ph;

	LoadImage(&frameBufferPosition, image1.pixel);
	DrawSync(0);

		// load clut
	frameBufferPosition.x = image1.cx;
	frameBufferPosition.y = image1.cy;
	frameBufferPosition.w = image1.cw;
	frameBufferPosition.h = image1.ch;

	LoadImage(&frameBufferPosition, image1.clut);
	DrawSync(0); 
	
   		// sort player's sprite
	texturePageID1 = GetTPage(1, 0, 320, 0);

	PlayersSprite.w = image1.pw * 2;		// 8-bit clut
	PlayersSprite.h = image1.ph;
	PlayersSprite.tpage = texturePageID1;
	PlayersSprite.cx = image1.cx;
	PlayersSprite.cy = image1.cy;
	PlayersSprite.attribute |= eightBitTextureAttributeMask;
	PlayersSprite.attribute |= mask1;
	PlayersSprite.attribute |= mask2;

	 
	 
		// now: brick sprites: 16-bit
	timPointer = (u_long *) BRICK_TIM_ADDRESS;
	timPointer++;			// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image2);

	frameBufferPosition.x = 640;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image2.pw;	  
	frameBufferPosition.h = image2.ph;
	
	LoadImage(&frameBufferPosition, image2.pixel);
	DrawSync(0);
		
		// sort bricks' sprites
	texturePageID2 = GetTPage(2, 0, 640, 0);

	for (i = 0; i < MAX_BRICKS; i++)
		{
		BricksArray[i].sprite.w = image2.pw;	   
		BricksArray[i].sprite.h = image2.ph;
		BricksArray[i].sprite.tpage = texturePageID2;
		BricksArray[i].sprite.cx = image2.cx;
		BricksArray[i].sprite.cy = image2.cy;
		BricksArray[i].sprite.attribute |= sixteenBitTextureAttributeMask;
		BricksArray[i].sprite.attribute |= mask1;
		BricksArray[i].sprite.attribute |= mask2;
		BricksArray[i].sprite.scalex = ONE/4;	// squash to 1/4
		BricksArray[i].sprite.scaley = ONE/4;	// squash to 1/4
		}
 

		// now: ball sprite
	frameBufferPosition.x = 320;
	frameBufferPosition.y = 256;
	frameBufferPosition.w = 16/4;		// 4-bit CLUT
	frameBufferPosition.h = 16;

	LoadImage(&frameBufferPosition, ball16x16);
	DrawSync(0);

	frameBufferPosition.x = 0;
	frameBufferPosition.y = 481;
	frameBufferPosition.w = 16;	   
	frameBufferPosition.h = 1;

	LoadImage(&frameBufferPosition, ballcolor[4]);	// 1 is green
	DrawSync(0);

		// sort bricks' sprites
	texturePageID3 = GetTPage(0, 0, 320, 256);

	BallsSprite.w = 16;	   
	BallsSprite.h = 16;
	BallsSprite.tpage = texturePageID3;
	BallsSprite.cx = 0;
	BallsSprite.cy = 481;
	BallsSprite.scalex = ONE/2;		 // squash by 1/2
	BallsSprite.scaley = ONE/2;		 // squash by 1/2
}




	// set up background rectangle: main use is to stop sprites
	// having black-box edges
	// (sprites created very simply in PaintShopPro, 
	// converted via timutil) 

void ClearBackground (void)
{
	u_long mask1, mask2, mask3;

	BackgroundRectangle.attribute = 0;
	mask1 = 1L << 30;
	mask2 = 1L << 29;
	mask3 = 1L << 28;
		
		// turn semi-transparency on
	BackgroundRectangle.attribute
			= (BackgroundRectangle.attribute | mask1);

	BackgroundRectangle.x = 0;
	BackgroundRectangle.y = 0;
	BackgroundRectangle.w = SCREEN_WIDTH;
	BackgroundRectangle.h = SCREEN_HEIGHT;
	BackgroundRectangle.r = 0;
	BackgroundRectangle.g = 0;
	BackgroundRectangle.b = 0;
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
				
		// player moves the bat 
	if (PadStatus & PADLleft)
		{
		PlayersMovement.dx -= PlayerSpeed;
		KeepWithinRange(PlayersMovement.dx, 
			-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
		}
	if (PadStatus & PADLright)
		{
		PlayersMovement.dx += PlayerSpeed;
		KeepWithinRange(PlayersMovement.dx, 
			-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
		}

	if (PadStatus & PADR1)
		PlayerSpeed++;
	if (PadStatus & PADR2)
		PlayerSpeed--;
}





 

	// handle movements of player's bat, the ball;
	// handle collisions of ball with player's bat and with bricks
	// scope for optimisation here is Massive

	// this is NOT intended as an example of how to do collision-detection
	// and collision-handling: this was done quickly to make it work
	
void HandleAllObjects (void)
{
	RECT player, ball, brick;	// collision rectangles
	int edge;			// of brick rectangle: where did ball hit?
	int xInitialDistance, xLeftOver;
	int yInitialDistance, yLeftOver;
	int ballMoved = FALSE;	
	
	int i, j;

 	

		// sort bat's collision rectangle
	player.x = (short int) (PlayersMovement.x);
	player.y = (short int) (PlayersMovement.y);
	player.w = (short int) (PlayersMovement.width);
	player.h = (short int) (PlayersMovement.height);

		// sort ball's collision rectangle
	ball.x = (short int) (BallsMovement.x);
	ball.y = (short int) (BallsMovement.y);
	ball.w = (short int) (BallsMovement.width);
	ball.h = (short int) (BallsMovement.height);

		// handle ball hitting player's bat:
		// bat nudges ball along in x-direction only	
	if (RectanglesTouch(&player, &ball) 
				|| RectanglesOverlap(&player, &ball))
		{
		if (RectanglesOverlap(&player, &ball))
			{
			MoveBallOutOfBat();
			}
	
			// if below top-of-bat line, reflect off side of bat
		if (BallsMovement.y > 
				(MAX_Y-PlayersMovement.height-BallsMovement.height))
			{
			if (BallsMovement.dx == 0)
				BallsMovement.dx = PlayersMovement.dx;
			else if (SIGN_OF(BallsMovement.dx) 	// bat 'overtaking' ball
				== SIGN_OF(PlayersMovement.dx))
				{
				BallsMovement.dx += (Coefficient * PlayersMovement.dx);
				}
			else		// moving in opposite directions
				{
				BallsMovement.dx = -BallsMovement.dx 
									+ (Coefficient * PlayersMovement.dx);
				}
			}
		else
			{
				// first: reflect off top of bat
			BallsMovement.dy = -BallsMovement.dy;
		   		
				// effect of player's bat: nudge along x-wise
				// effect depends on bat's velocity
			#if 0		// OLD
			BallsMovement.dx 
				+= (int)(Coefficient * ((double)PlayersMovement.dx));
			#endif
			BallsMovement.dx += (Coefficient * PlayersMovement.dx);
			}

			// clip ball's velocity components
		KeepWithinRange(BallsMovement.dx, 
			-MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
		KeepWithinRange(BallsMovement.dy, 
			-MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
		BallsMovement.x += BallsMovement.dx;
		BallsMovement.y += BallsMovement.dy;
		ballMoved = TRUE;
		}
	else 			// see if ball hits a brick
   		{
			// sort ball's collision rectangle: where it's about to be
		ball.x = (short int) (BallsMovement.x + BallsMovement.dx);
		ball.y = (short int) (BallsMovement.y + BallsMovement.dy);
		ball.w = (short int) BallsMovement.width;
		ball.h = (short int) BallsMovement.height;
		for (i = 0; i < MAX_BRICKS; i++)
			{
			if (BricksArray[i].alive == TRUE)
				{		
					// update sprite display position
				BricksArray[i].sprite.x 
								= (short int) BricksArray[i].movement.x;
				BricksArray[i].sprite.y 	
								= (short int) BricksArray[i].movement.y;
					
					// get brick's collision rectangle
				brick.x = (short int) BricksArray[i].movement.x;
	   			brick.y = (short int) BricksArray[i].movement.y;
				brick.w = (short int) BricksArray[i].movement.width;
				brick.h = (short int) BricksArray[i].movement.height;
				
				if (RectanglesOverlap(&ball, &brick)
					|| RectanglesTouch(&ball, &brick))
					{
					edge = FindWhereRectanglesTouch(&ball, &brick);

						// main ploy: 
						// deal with reflection and move in one go
					switch(edge)
						{
						case LEFT:
							xInitialDistance = BallsMovement.x
								- (BricksArray[i].movement.x 
								+ BricksArray[i].movement.width);
							xLeftOver = -BallsMovement.dx 
											- xInitialDistance;
							BallsMovement.x 
								+= (BallsMovement.dx + (2 * xLeftOver));
							BallsMovement.dx = -BallsMovement.dx;
							BallsMovement.y += BallsMovement.dy;
							break;
						case RIGHT:
							xInitialDistance = BricksArray[i].movement.x 
								- (BallsMovement.x 
								+ BallsMovement.width);
							xLeftOver = BallsMovement.dx 
											- xInitialDistance;
							BallsMovement.x 
								+= (BallsMovement.dx - (2 * xLeftOver));
							BallsMovement.dx = -BallsMovement.dx;
							BallsMovement.y += BallsMovement.dy;
							break;
						case TOP:
						case TOP_LEFT:
						case TOP_RIGHT:
							yInitialDistance = BallsMovement.y
								- (BricksArray[i].movement.y 
								+ BricksArray[i].movement.height);
							yLeftOver = -BallsMovement.dy 
											- yInitialDistance;
							BallsMovement.y 
								+= (BallsMovement.dy + (2 * yLeftOver));
							BallsMovement.dy = -BallsMovement.dy;
							BallsMovement.x += BallsMovement.dx;
							break;
						case BOTTOM:
						case BOTTOM_RIGHT:
						case BOTTOM_LEFT:
							yInitialDistance = BricksArray[i].movement.y 
								- (BallsMovement.y 
								+ BallsMovement.height);
							yLeftOver = BallsMovement.dy 
											- yInitialDistance;
							BallsMovement.y 
								+= (BallsMovement.dy - (2 * yLeftOver));
							BallsMovement.dy = -BallsMovement.dy;
							BallsMovement.x += BallsMovement.dx;
							break;
						default:
							assert(FALSE);	// bad edge value 
						}

					ballMoved = TRUE;
					BricksArray[i].toughness--;
					if (BricksArray[i].toughness <= 0)
						{
						DestroyBrick(i);
						CurrentScore.points += 50;
						}
					break;		// ball only hits one brick at a time
					}
				}
			}
		}

		// keep ball's velocity components within sensible range
	KeepWithinRange(BallsMovement.dx, 
		-MAX_BALL_X_SPEED, MAX_BALL_X_SPEED);
	KeepWithinRange(BallsMovement.dy, 
		-MAX_BALL_Y_SPEED, MAX_BALL_Y_SPEED);
				

	if (ballMoved == FALSE)		    // ball hit nothing: move normally
		{  				
			// update balls position
		BallsMovement.x += BallsMovement.dx;
		BallsMovement.y += BallsMovement.dy;
		}

		// perfectly elastic collisions off walls and ceiling
	if (BallsMovement.x > MAX_X-BallsMovement.width)	 // right wall
		{
		BallsMovement.x -= (2 * 
				(BallsMovement.x - (MAX_X-BallsMovement.width)) );
		BallsMovement.dx = -BallsMovement.dx;
		}
	if (BallsMovement.x < MIN_X)			 // left wall
		{
		BallsMovement.x += (2 * (MIN_X - BallsMovement.x) );
		BallsMovement.dx = -BallsMovement.dx;
		}
	if (BallsMovement.y < MIN_Y)			// ceiling
		{
		BallsMovement.y += (2 * (MIN_Y - BallsMovement.y) );
		BallsMovement.dy = -BallsMovement.dy;
		}
	if (BallsMovement.y > MAX_Y-BallsMovement.height) 	 // hits floor
		{									// player loses a life
		NumberOfLives--;
		CrudestPause(30);		// half a second
		RestartTheBall();
		RestartPlayersBat();
		}

		// update sprite display position
	BallsSprite.x = (short int) BallsMovement.x-1;
	BallsSprite.y = (short int) BallsMovement.y-1;
	   

	   	 
	 	// last section: handle the player's bat

		// update player's bat movement
	PlayersMovement.x += PlayersMovement.dx;
	PlayersMovement.y += PlayersMovement.dy;

		// HERE: deny momentum to the player's bat
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;

		// bat stops at walls
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

		// update sprite display position
	PlayersSprite.x = (short int) PlayersMovement.x;
	PlayersSprite.y = (short int) PlayersMovement.y;
}




	// push ball upwards, out of bat area
void MoveBallOutOfBat (void)
{
	if (((int)BallsMovement.y) + ((int)BallsMovement.height)
			> ((int)PlayersMovement.y))
				{
				BallsMovement.y 
					= PlayersMovement.y - BallsMovement.height;
				}
}

	    
  


	// section below: basic rectangle intersections
	// massive scope for optimisation:
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


int RectanglesTouch (RECT* first, RECT* second)
{
	int boolean;

	boolean = (XTouch(first, second) && YOverlap(first, second))
				|| (YTouch(first, second) && XOverlap(first, second));

	return boolean;
}


int XTouch (RECT* first, RECT* second)
{
	if (first->x > second->x)
		{
		return (first->x == second->x + second->w);
		}
	else if (first->x == second->x)
		return FALSE;
	else
		{
		return (first->x + first->w == second->x);
		}
}



int YTouch (RECT* first, RECT* second)
{
	if (first->y > second->y)
		{
		return (first->y == second->y + second->h);
		}
	else if (first->y == second->y)
		return FALSE;
  	else
		{
		return (first->y + first->h == second->y);
		}
}



	// returns edge/corner constant (LEFT, BOTTOM_RIGHT), etc
	// or NONE if rectangles don't touch at all
	// NOTE: result is relative to the First rectangle's perspective

	// NOTE: this fucntion regards overlapping rectangles as
	// counting as touching
int FindWhereRectanglesTouch (RECT* first, RECT* second)
{
	int whereTheyTouch;		// TOP, BOTTOM, LEFT, RIGHT
							// TOP_LEFT, TOP_RIGHT, etc
	int xTouch;		// LEFT, MIDDLE, RIGHT, NONE
	int yTouch;		// TOP, MIDDLE, BOTTOM, NONE

	if (first->x > second->x)
		{
		if (first->x == second->x + second->w)
			xTouch = LEFT;
		else if (first->x < second->x + second->w)
			xTouch = MIDDLE;	
		else
			xTouch = NONE;
		}
	else if (first->x == second->x)
		xTouch = MIDDLE;
	else
		{
		if (first->x + first->w == second->x)
			xTouch = RIGHT;
		else if (first->x + first->w > second->x)
			xTouch = MIDDLE;
		else
			xTouch = NONE;
		}

	if (first->y > second->y)
		{
		if (first->y == second->y + second->h)
			yTouch = TOP;
		else if (first->y < second->y + second->h)
			yTouch = MIDDLE;	
		else
			yTouch = NONE;
		}
	else if (first->y == second->y)
		yTouch = MIDDLE;
	else
		{
		if (first->y + first->h == second->y)
			yTouch = BOTTOM;
		else if (first->y + first->h > second->y)
			yTouch = MIDDLE;
		else
			yTouch = NONE;
		}

	switch(xTouch)
		{
		case LEFT:
			switch(yTouch)
				{
				case TOP:
					whereTheyTouch = TOP_LEFT;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM_LEFT;
					break;
				case MIDDLE:
					whereTheyTouch = LEFT;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
				}
			break;
		case RIGHT:
			switch(yTouch)
				{
				case TOP:
					whereTheyTouch = TOP_RIGHT;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM_RIGHT;
					break;
				case MIDDLE:
					whereTheyTouch = RIGHT;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
				}
			break;
		case MIDDLE:
			switch(yTouch)
				{
				case TOP:
					whereTheyTouch = TOP;
					break;
				case BOTTOM:
					whereTheyTouch = BOTTOM;
					break;
				case MIDDLE:
					whereTheyTouch = OVERLAP;
					break;
				case NONE:
					whereTheyTouch = NONE;
					break;
				}
			break;	
		case NONE:
			whereTheyTouch = NONE;
			break;
		}
		 
	return whereTheyTouch;
}
	



	
	// set up low-level controller buffers
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





	// this flag ensures that de-initialisation done properly
	// ie only when initialisation succeeded
	// ==> don't have to keep resetting the black psx
int SoundSuccess;



	// NOTE: printf prints to the PC MS-DOS box siocons console
	// NOT to the TV screen

	// this sound code taken straight from other samples
void InitSound (void)
{
	vab = SsVabTransfer( (u_char*)VH_ADDR, (u_char*)VB_ADDR, -1, 1 );
	if (vab < 0) 
		{
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


	


	
  	
