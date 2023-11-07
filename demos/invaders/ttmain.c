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
#define INVADER_SHOT_TIM_ADDRESS 0x80170000
#define PLAYER_SHOT_TIM_ADDRESS 0x80180000
#define UFO_TIM_ADDRESS 0x80190000


	// sprite sizes
#define INVADER_WIDTH 16
#define INVADER_HEIGHT 16
#define PLAYER_WIDTH 51
#define PLAYER_HEIGHT 18
#define PLAYER_SHOT_WIDTH 2
#define PLAYER_SHOT_HEIGHT 8
#define INVADER_SHOT_WIDTH 8
#define INVADER_SHOT_HEIGHT 8
#define UFO_WIDTH 61
#define UFO_HEIGHT 21


	// offsets into texture pages
#define PLAYER_U 9
#define PLAYER_V 3
#define INVADER_U 0
#define INVADER_V 0
#define PLAYER_SHOT_U 3
#define PLAYER_SHOT_V 0
#define INVADER_SHOT_U 0
#define INVADER_SHOT_V 0
#define UFO_U 2
#define UFO_V 7




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
#define MAX_INVADERS_SHOTS 250

	// objects: player, player's shot, ufo, invaders, invaders' shots
#define MAX_OBJECTS (MAX_INVADERS + MAX_INVADERS_SHOTS + 3)

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


MovementData PlayersShotsMovement;
GsSPRITE PlayersShotsSprite;
int PlayersShotIsAlive;


MovementData UfosMovement;
GsSPRITE UfosSprite;
int UfoIsAlive;


  
	
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
	int alive;
	GsSPRITE sprite;
	MovementData movement;
} Shot;




Invader InvadersArray[MAX_INVADERS];
int NumberOfInvaders;


Shot InvadersShotsArray[MAX_INVADERS_SHOTS];




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
int CreateNewShot (void);
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
void AdjustInvadersSpeed (void);
void HandleTheUfosAppearance(void);

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
		NumberOfInvaders = CountNumberOfInvaders();

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
			if (InvadersArray[i].alive)
				{
				GsSortSprite(&(InvadersArray[i].sprite),
						&WorldOrderingTable[activeBuffer], 0);
				}
			}
		
			// put shots into ordering table	
		for (i = 0; i < MAX_INVADERS_SHOTS; i++)
			{
			if (InvadersShotsArray[i].alive)
				{
				GsSortSprite(&(InvadersShotsArray[i].sprite),
						&WorldOrderingTable[activeBuffer], 0);
				}
			}
			
		if (PlayersShotIsAlive)
			{
			GsSortSprite(&PlayersShotsSprite,
					&WorldOrderingTable[activeBuffer], 0);
			}
		
		if (UfoIsAlive)
			{
			GsSortSprite(&UfosSprite,
					&WorldOrderingTable[activeBuffer], 0);
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
	
	PlayersShotIsAlive = FALSE;
	UfoIsAlive = FALSE;			 

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
	PlayersMovement.width = PLAYER_WIDTH;
	PlayersMovement.height = PLAYER_HEIGHT;
	PlayersMovement.dx = 0;
	PlayersMovement.dy = 0;
	InitGsSprite(&PlayersSprite);

	PlayersShotsMovement.x = 0;
	PlayersShotsMovement.y = 0;
	PlayersShotsMovement.width = PLAYER_SHOT_WIDTH;
	PlayersShotsMovement.height = PLAYER_SHOT_HEIGHT;
	PlayersShotsMovement.dx = 0;
	PlayersShotsMovement.dy = 0;
	InitGsSprite(&PlayersShotsSprite);
	PlayersShotIsAlive = FALSE;

	UfosMovement.x = 0;
	UfosMovement.y = 0;
	UfosMovement.width = UFO_WIDTH;
	UfosMovement.height = UFO_HEIGHT;
	UfosMovement.dx = 0;
	UfosMovement.dy = 0;
	InitGsSprite(&UfosSprite);
	UfoIsAlive = FALSE;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		InvadersArray[i].id = i;
		InvadersArray[i].alive = FALSE;
		InvadersArray[i].type = NONE;
		InvadersArray[i].movement.x = 0;
		InvadersArray[i].movement.y = 0;
		InvadersArray[i].movement.width = INVADER_WIDTH;
		InvadersArray[i].movement.height = INVADER_HEIGHT;
		InvadersArray[i].movement.dx = 0;
		InvadersArray[i].movement.dy = 0;
		InitGsSprite( &(InvadersArray[i].sprite) );
		}

	for (i = 0; i < MAX_INVADERS_SHOTS; i++)	 
		{
		InvadersShotsArray[i].id = i;
		InvadersShotsArray[i].movement.x = 0;
		InvadersShotsArray[i].movement.y = 0; 
		InvadersShotsArray[i].movement.width = INVADER_SHOT_WIDTH;
		InvadersShotsArray[i].movement.height = INVADER_SHOT_HEIGHT;
		InvadersShotsArray[i].movement.dx = 0;
		InvadersShotsArray[i].movement.dy = 0;
		InitGsSprite( &(InvadersShotsArray[i].sprite) );
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
		if (InvadersArray[i].alive)
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
		if (InvadersArray[i].alive)
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
		if (InvadersArray[i].alive)
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





int CreateNewShot (void)
{
	int id;
	int i; 
	int found = -1;

	for (i = 0; i < MAX_INVADERS_SHOTS; i++)
		{
		if (InvadersShotsArray[i].alive == FALSE)
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
		InvadersShotsArray[id].alive = TRUE;
		return id;
		}
}


void DestroyShot (int shotID)
{
	InvadersShotsArray[shotID].alive = FALSE;
}



void DestroyAllShots (void)
{
	int i;

	for (i = 0; i < MAX_INVADERS_SHOTS; i++)
		{
		if (InvadersShotsArray[i].alive)
			DestroyShot(i);
		}

	PlayersShotIsAlive = FALSE;
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
	GsIMAGE image; 
	RECT frameBufferPosition; 
	u_short texturePageID1, texturePageID2, texturePageID3;
	u_short texturePageID4, texturePageID5;
	u_long eightBitTextureAttributeMask = 1L << 24;
	u_long mask1 = 1L << 29;   		// semi-transparency
	u_long mask2 = 1L << 30;		// semi-transparency
	int i;
	
		// player's ship: 8-bit
	timPointer = (u_long *) SHIP_TIM_ADDRESS;
	timPointer++; 		// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image); 

		// where texture will go							
	frameBufferPosition.x = 320;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image.pw; 
	frameBufferPosition.h = image.ph;

#if DEBUG
	printf("px %d, py %d\n", image.px, image.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.pixel);
	DrawSync(0);		// probably not necessary
						// but LoadImage is non-blocking

		// load clut
	frameBufferPosition.x = image.cx;
	frameBufferPosition.y = image.cy;
	frameBufferPosition.w = image.cw;
	frameBufferPosition.h = image.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image.pmode);
		// this says if texture is 4-bit, 8-bit or 16-bit (or multi-mode)
	
	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.clut);
	DrawSync(0); 
	
   		// sort player's sprite
	texturePageID1 = GetTPage(1, 0, 320, 0);

#if DEBUG
	printf("tpage 1 is %hd\n", texturePageID1);
#endif

	PlayersSprite.w = image.pw * 2;	 // 8-bit clut: compressed in VRAM
	PlayersSprite.h = image.ph;
	PlayersSprite.tpage = texturePageID1;
	PlayersSprite.cx = image.cx;
	PlayersSprite.cy = image.cy;
	PlayersSprite.attribute |= eightBitTextureAttributeMask;
	PlayersSprite.attribute |= mask1;	// semi-transparency
	PlayersSprite.attribute |= mask2;
	PlayersSprite.u = PLAYER_U;
	PlayersSprite.v = PLAYER_V;
	
	
	
		// now: invader sprites: 8-bit
	timPointer = (u_long *) INVADER_TIM_ADDRESS;
	timPointer++;			// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image);

	frameBufferPosition.x = 320;
	frameBufferPosition.y = 256;
	frameBufferPosition.w = image.pw;	  // 8-bit CLUT
	frameBufferPosition.h = image.ph;
	
#if DEBUG
	printf("px %d, py %d\n", image.px, image.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.pixel);
	DrawSync(0);

	frameBufferPosition.x = image.cx;
	frameBufferPosition.y = image.cy;
	frameBufferPosition.w = image.cw;
	frameBufferPosition.h = image.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image.pmode);

	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.clut);
	DrawSync(0);

		// sort invaders' sprites
	texturePageID2 = GetTPage(1, 0, 320, 256);
#if DEBUG
	printf("tpage 2 is %hd\n", texturePageID2);
#endif

	for (i = 0; i < MAX_INVADERS; i++)
		{
		InvadersArray[i].sprite.w = image.pw * 2;		// 8-bit clut
		InvadersArray[i].sprite.h = image.ph;
		InvadersArray[i].sprite.tpage = texturePageID2;
		InvadersArray[i].sprite.cx = image.cx;
		InvadersArray[i].sprite.cy = image.cy;
		InvadersArray[i].sprite.attribute |= eightBitTextureAttributeMask;
		InvadersArray[i].sprite.attribute |= mask1;
		InvadersArray[i].sprite.attribute |= mask2;
		InvadersArray[i].sprite.scalex = ONE;
		InvadersArray[i].sprite.scaley = ONE;
		InvadersArray[i].sprite.u = INVADER_U;
		InvadersArray[i].sprite.v = INVADER_V;
		}
		


		// now: shot sprite: 4-bit
	timPointer = (u_long *) INVADER_SHOT_TIM_ADDRESS;
	timPointer++;			// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image);

	frameBufferPosition.x = 640;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image.pw;		// 4-bit CLUT
	frameBufferPosition.h = image.ph;

#if DEBUG
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif
	LoadImage(&frameBufferPosition, image.pixel);
	DrawSync(0);

	frameBufferPosition.x = image.cx;
	frameBufferPosition.y = image.cy;
	frameBufferPosition.w = image.cw;
	frameBufferPosition.h = image.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image.pmode);

	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.clut);
	DrawSync(0);

		// sort shots' sprites
	texturePageID3 = GetTPage(0, 0, 640, 0);
#if DEBUG
	printf("tpage 3 is %hd\n", texturePageID3);
#endif

	for (i = 0; i < MAX_INVADERS_SHOTS; i++)
		{
		InvadersShotsArray[i].sprite.w = image.pw * 4;
		InvadersShotsArray[i].sprite.h = image.ph;
		InvadersShotsArray[i].sprite.tpage = texturePageID3;
		InvadersShotsArray[i].sprite.cx = image.cx;
		InvadersShotsArray[i].sprite.cy = image.cy;
		InvadersShotsArray[i].sprite.scalex = ONE/4;
		InvadersShotsArray[i].sprite.scaley = ONE/4;
		InvadersShotsArray[i].sprite.u = INVADER_SHOT_U;
		InvadersShotsArray[i].sprite.v = INVADER_SHOT_V; 
		}



		// player's shot: 8-bit
	timPointer = (u_long *) PLAYER_SHOT_TIM_ADDRESS;
	timPointer++; 		// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image); 

		// where texture will go							
	frameBufferPosition.x = 640;
	frameBufferPosition.y = 256;
	frameBufferPosition.w = image.pw; 
	frameBufferPosition.h = image.ph;

#if DEBUG
	printf("px %d, py %d\n", image.px, image.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.pixel);
	DrawSync(0);		// probably not necessary
						// but LoadImage is non-blocking

		// load clut
	frameBufferPosition.x = image.cx;
	frameBufferPosition.y = image.cy;
	frameBufferPosition.w = image.cw;
	frameBufferPosition.h = image.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image.pmode);
		// this says if texture is 4-bit, 8-bit or 16-bit (or multi-mode)
	
	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.clut);
	DrawSync(0); 
	
   		// sort player's sprite
	texturePageID4 = GetTPage(1, 0, 640, 256);

#if DEBUG
	printf("tpage 4 is %hd\n", texturePageID4);
#endif

	PlayersShotsSprite.w = image.pw * 2;	 // 8-bit clut: compressed in VRAM
	PlayersShotsSprite.h = image.ph;
	PlayersShotsSprite.tpage = texturePageID4;
	PlayersShotsSprite.cx = image.cx;
	PlayersShotsSprite.cy = image.cy;
	PlayersShotsSprite.attribute |= eightBitTextureAttributeMask;
	PlayersShotsSprite.attribute |= mask1;	// semi-transparency
	PlayersShotsSprite.attribute |= mask2;
	PlayersShotsSprite.u = PLAYER_SHOT_U;
	PlayersShotsSprite.v = PLAYER_SHOT_V;



		// Ufo: 8-bit
	timPointer = (u_long *) UFO_TIM_ADDRESS;
	timPointer++; 		// skip TIM_HEADER

	GsGetTimInfo(timPointer, &image); 

		// where texture will go							
	frameBufferPosition.x = 960;
	frameBufferPosition.y = 0;
	frameBufferPosition.w = image.pw; 
	frameBufferPosition.h = image.ph;

#if DEBUG
	printf("px %d, py %d\n", image.px, image.py);
	
	printf("pixel rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.pixel);
	DrawSync(0);		// probably not necessary
						// but LoadImage is non-blocking

		// load clut
	frameBufferPosition.x = image.cx;
	frameBufferPosition.y = image.cy;
	frameBufferPosition.w = image.cw;
	frameBufferPosition.h = image.ch;

#if DEBUG
	printf("Pixel mode: %d\n", image.pmode);
		// this says if texture is 4-bit, 8-bit or 16-bit (or multi-mode)
	
	printf("clut rect: %d, %d, %d, %d\n",
		  frameBufferPosition.x,
		  frameBufferPosition.y,
		  frameBufferPosition.w,
		  frameBufferPosition.h);
#endif

	LoadImage(&frameBufferPosition, image.clut);
	DrawSync(0); 
	
   		// sort player's sprite
	texturePageID5 = GetTPage(1, 0, 960, 0);

#if DEBUG
	printf("tpage 5 is %hd\n", texturePageID5);
#endif

	UfosSprite.w = image.pw * 2;	 // 8-bit clut: compressed in VRAM
	UfosSprite.h = image.ph;
	UfosSprite.tpage = texturePageID5;
	UfosSprite.cx = image.cx;
	UfosSprite.cy = image.cy;
	UfosSprite.attribute |= eightBitTextureAttributeMask;
	UfosSprite.attribute |= mask1;	// semi-transparency
	UfosSprite.attribute |= mask2;
	UfosSprite.u = UFO_U;
	UfosSprite.v = UFO_V;
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
		if (PlayersShotIsAlive == FALSE)	  // one at a time
			{
			PlayersShotIsAlive = TRUE;

				// put shot above player, moving upwards
			PlayersShotsMovement.x = PlayersMovement.x + (PLAYER_WIDTH/2);
			PlayersShotsMovement.y = PlayersMovement.y - 2;
				// HERE we set the player's shot speed
			PlayersShotsMovement.dy = -2.5;
			}
		}
}





	// flip direction of all invaders
void SendAllInvadersTheOtherWay (void)
{
	int i;

	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive)
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
		if (InvadersArray[i].alive)
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
			if (InvadersArray[i].alive)
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
	RECT player, invader, playersShot, invadersShot, ufo;
	int i, j;
 

 		// sprites don't change size during game
		// hence always know width and height of collision rectangles
		// CHANGES HERE
	player.w = PlayersMovement.width;
	player.h = PlayersMovement.height;

	playersShot.w = PlayersShotsMovement.width;
	playersShot.h = PlayersShotsMovement.height;

	invader.w = InvadersArray[0].movement.width;	   
	invader.h = InvadersArray[0].movement.width;

	invadersShot.w = InvadersShotsArray[0].movement.width;
	invadersShot.h = InvadersShotsArray[0].movement.height;

	ufo.w = UfosMovement.width;
	ufo.h = UfosMovement.height;

		// move the player 
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

		// update sprite display position
	PlayersSprite.x = (short int) PlayersMovement.x;
	PlayersSprite.y = (short int) PlayersMovement.y;

		// sort out topleft of player's collision rectangle
	player.x = PlayersMovement.x;
	player.y = PlayersMovement.y;

		// deal with player's shot
	if (PlayersShotIsAlive)
		{
			// move player's shot
		PlayersShotsMovement.x += PlayersShotsMovement.dx;
		PlayersShotsMovement.y += PlayersShotsMovement.dy;

			// if goes off, destroy it
		if (PlayersShotsMovement.y < MIN_X)
			{
			PlayersShotIsAlive = FALSE;
			}

			// update sprite display position
		PlayersShotsSprite.x = PlayersShotsMovement.x;
		PlayersShotsSprite.y = PlayersShotsMovement.y;

		playersShot.x = PlayersShotsMovement.x;
		playersShot.y = PlayersShotsMovement.y;
		}

		// if any invader about to jump off screen,
		// send entire lot the other way
	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive)
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

		// deal with invaders
	for (i = 0; i < MAX_INVADERS; i++)
		{
		if (InvadersArray[i].alive)
			{
				// firstly move them
			InvadersArray[i].movement.x += InvadersArray[i].movement.dx;
			InvadersArray[i].movement.y += InvadersArray[i].movement.dy;

				// invaders win if get far down enough
			if (InvadersArray[i].movement.y > MAX_Y-16)
				{
				NumberOfLives = 0;
				break;
				}

				// update sprite display position
			InvadersArray[i].sprite.x = InvadersArray[i].movement.x;
			InvadersArray[i].sprite.y = InvadersArray[i].movement.y;

				// sort topleft of collision rectangle	
			invader.x = InvadersArray[i].movement.x;
			invader.y = InvadersArray[i].movement.y;

				// detect collisions of invaders with player's ship
			if (RectanglesOverlap(&player, &invader))	 // lose a life
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

				// detect collisions with player's shot
			if (PlayersShotIsAlive)
				{
				if (RectanglesOverlap(&invader, &playersShot))
					{
					PlayersShotIsAlive = FALSE;
					DestroyInvader(i);
					CurrentScore.points += (InvadersArray[i].type * 50);
					}
				}
			}
		}

		// deal with invaders' shots
	for (i = 0; i < MAX_INVADERS_SHOTS; i++)
		{
		if (InvadersShotsArray[i].alive)
			{	
				// move first
			InvadersShotsArray[i].movement.x 
								+= InvadersShotsArray[i].movement.dx;
			InvadersShotsArray[i].movement.y 
								+= InvadersShotsArray[i].movement.dy;
			
				// update sprite display position				
			InvadersShotsArray[i].sprite.x 
						= InvadersShotsArray[i].movement.x;
			InvadersShotsArray[i].sprite.y 
						= InvadersShotsArray[i].movement.y;
			
				// if it goes off screen, kill it			
			if ( (InvadersShotsArray[i].movement.y > 
					(MAX_Y-InvadersShotsArray[i].movement.height)) 
					|| (InvadersShotsArray[i].movement.y < MIN_Y) )
				{
				DestroyShot(i);
				}

				// sort topleft of collision rectangle 
			invadersShot.x = InvadersShotsArray[i].movement.x;
			invadersShot.y = InvadersShotsArray[i].movement.y;
			#if 0		// this section CRASHES GCC
				// does any shot collide with player's ship?
			if (RectanglesOverlap(&player, &invadersShot))
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
			#endif
			}
		}

		// deal with ufo
	UfosMovement.x += UfosMovement.dx;
	UfosMovement.y += UfosMovement.dy;

		// update sprite display position
	UfosSprite.x = UfosMovement.x;
	UfosSprite.y = UfosMovement.y;

		// sort topleft of Ufo's collision rectangle
	ufo.x = UfosMovement.x;
	ufo.y = UfosMovement.y;

		// is Ufo hit by player's shot?
	if (PlayersShotIsAlive)
		{
		if (RectanglesOverlap(&ufo, &playersShot))
			{
			UfoIsAlive = FALSE;
			PlayersShotIsAlive = FALSE;
			CurrentScore.points += (1000 * Level);
			}
		}

		// finally: do invaders fire any new shots?
	HandleInvadersShooting();

		// make them speed up if there are few left
	AdjustInvadersSpeed();

		// does ufo appear?
	HandleTheUfosAppearance();
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

		shotID = CreateNewShot();

			// put shot below invader, moving down
			// MAKE CHANGES HERE: graphics........
		InvadersShotsArray[shotID].movement.x 
 			= InvadersArray[invaderID].movement.x + (INVADER_WIDTH/2);
		InvadersShotsArray[shotID].movement.y 
			= InvadersArray[invaderID].movement.y + INVADER_HEIGHT + 2;
			// HERE we set the invaders' shot speed
		InvadersShotsArray[shotID].movement.dy = 1; 
		}			
}



	// make them speed up as their numbers decrease
void AdjustInvadersSpeed (void)
{	
	double newSpeed;
	int i;

	if (NumberOfInvaders < 10)
		{
		newSpeed = ((double)10) / ((double)NumberOfInvaders);
		for (i = 0; i < MAX_INVADERS; i++)
			{
			if (InvadersArray[i].alive)
				{
				if (InvadersArray[i].movement.dx >= 0)
					InvadersArray[i].movement.dx = newSpeed;
				else
					InvadersArray[i].movement.dx = -newSpeed;
				}
			}
		}
}	 





	// if nonexistent, does one appear?
void HandleTheUfosAppearance (void)
{
	if (UfoIsAlive == FALSE)
		{
		if (rand() % 240 == 0)		// HERE: sets Ufo frequency
			{
			UfoIsAlive = TRUE;
				// Ufo appears at topleft and moves left -> right
			UfosMovement.x = MIN_X;
			UfosMovement.y = MIN_Y;
				// HERE: sets Ufo's speed
			UfosMovement.dx = 2;
			}
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
	if (SoundSuccess)
		{
		SsSetMVol(MVOL, MVOL);			
		SsSeqSetVol(seq, SVOL, SVOL);		
		SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY); 
		}
}



void StopSound (void)
{
	if (SoundSuccess)
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


	

	
  	
