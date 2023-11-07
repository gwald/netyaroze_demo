/***************************************************************
*															   *
* Copyright (C) 1995 by Sony Computer Entertainment			   *
*			All rights Reserved								   *
*															   *
*		 L.Evans 		 					 25/09/96		   *
*															   *
***************************************************************/

			





#include <libps.h>

#include <stdio.h>

#include <rand.h>

#include "pad.h"

#include "balltex.h"			// ball's texture pattern 

#include "asssert.h"

#include "breakout.h"

#include "2d1.h"

#include "address.h"

#include "main.h"




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




		   

	  	  

MovementData 	PlayerOnesMovement;
GsSPRITE 		PlayerOnesSprite;


MovementData 	PlayerTwosMovement;
GsSPRITE 		PlayerTwosSprite;



MovementData 	BallsMovement;
GsSPRITE		BallsSprite;
int BallsMovementSpeed;


GsBOXF BackgroundRectangle;

   
  



GsOT		BreakoutOrderingTable[2];
	// 1 << BREAKOUT_OT_LENGTH: 2 to the power of OT_LENGTH
GsOT_TAG	BreakoutOrderingTableTags[2][1<<BREAKOUT_OT_LENGTH];


PACKET		BreakoutGpuPacketArea[2][MAX_BREAKOUT_OBJECTS * MAX_SIZEOF_PRIMITIVE];




	


int BreakoutQuitFlag = FALSE;

int BreakoutAutoPilotFlag = TRUE;

int BreakoutNumberOfPlayers = 1;




Brick BricksArray[MAX_BRICKS];




int Level; 
int PlayerOneNumberOfLives;
int PlayerTwoNumberOfLives;


  
Score HighScoreTable[5];
Score CurrentScore;
Score HighScore;



int count;
//u_long frameNumber = 0;	// specified in main



	// prototypes
	
//int main(void);

void RunBreakoutMainLoop (void);


void CrudestPause (int n);


void SetUpLevel1(void);
int DealWithPlayersDeath(void);
int DealWithPlayerFinishingTenLevels (void);

void ClearScores(void);
void DealWithNewScore (Score* last, Score* table, int tableLength,
											int* outputPosition);

void HandleFinalScore (void);

void InitialiseBreakout(void);
void InitialiseBreakout2 (void);

void ResetBreakout (void);


void InitialiseBreakoutObjects (void);


int CreateNewBrick (int type, int toughness);
void DestroyBrick (int brickID);
int CountNumberOfBricks (void);
void DestroyAllBricks (void);

void RestartTheBall (void);
void RestartPlayersBats (void);

void InitialiseBreakoutTexturesAndSprites(void);



void ClearBreakoutBackground(void);

void HandleBreakoutPadControls(void);
void SortAutoPilotActions (void);

void HandleAllBreakoutObjects(void);

void MoveBallOutOfBat (int playerBatY);

int RectanglesOverlap (RECT* first, RECT* second);
int XOverlap (RECT* first, RECT* second);
int YOverlap (RECT* first, RECT* second);
int RectanglesTouch (RECT* first, RECT* second);
int XTouch (RECT* first, RECT* second);
int YTouch (RECT* first, RECT* second);
int FindWhereRectanglesTouch (RECT* first, RECT* second);
int FindWherePointIsInRectangle (int x, int y, RECT *rect);
 



	// keep quantity within range
#define KeepWithinRange(x, l, h)	((x)=((x)<(l)?(l):(x)>(h)?(h):(x)))




  
 	
	
#if 0
	
int main (void)
{
	/****
	int playerWantsAnotherGame;
	int activeBuffer;
	int i;
	****/
			
	
	InitialiseBreakout();

	RunBreakoutMainLoop();	   

#if 0
	while (BreakoutQuitFlag == FALSE)
		{
		HandleBreakoutPadControls();

	   	HandleAllBreakoutObjects();

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) BreakoutGpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &BreakoutOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&BreakoutOrderingTable[activeBuffer], 0);

		GsSortSprite(&PlayerOnesSprite,
							&BreakoutOrderingTable[activeBuffer], 0);

		GsSortSprite(&BallsSprite,
							&BreakoutOrderingTable[activeBuffer], 0);

		for (i = 0; i < MAX_BRICKS; i++)
			{
			if (BricksArray[i].alive == TRUE)
				{
				GsSortSprite(&(BricksArray[i].sprite),
						&BreakoutOrderingTable[activeBuffer], 0);
				}
			}
	 	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &BreakoutOrderingTable[activeBuffer]);

		GsDrawOt(&BreakoutOrderingTable[activeBuffer]);

	 	FntPrint("Breakout!\n");
		FntPrint("Frame number: %d\n", frameNumber);
		FntPrint("Lives: %d\n", PlayerOneNumberOfLives);
		FntPrint("Score: %d\n", CurrentScore.points);
		FntPrint("Hsync count: %d\n", count);
		
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
					PlayerOneNumberOfLives = 5;
					DestroyAllBricks();
					CurrentScore.points = 0;
					SetUpLevel1();
					}
				else
					BreakoutQuitFlag = TRUE;
				}
			else
				SetUpLevel1();	
			}							
		
		if (PlayerOneNumberOfLives <= 0)			// player has lost
			{
			CrudestPause(30);
			playerWantsAnotherGame = DealWithPlayersDeath();
			if (playerWantsAnotherGame)
				{
				Level = 1;
				PlayerOneNumberOfLives = 5;
				DestroyAllBricks();
				CurrentScore.points = 0;
				SetUpLevel1();
				}
			else
				BreakoutQuitFlag = TRUE;
			}

		frameNumber++;
		}
#endif

		// cleanup
	ResetGraph(3);
	return(0);
}

#endif





void RunBreakoutMainLoop (void)
{
	int playerWantsAnotherGame;
	int activeBuffer;
	int i;

	while (BreakoutQuitFlag == FALSE)
		{
		HandleBreakoutPadControls();

	   	HandleAllBreakoutObjects();

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) BreakoutGpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &BreakoutOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&BreakoutOrderingTable[activeBuffer], 0);

		GsSortSprite(&PlayerOnesSprite,
							&BreakoutOrderingTable[activeBuffer], 0);

		GsSortSprite(&BallsSprite,
							&BreakoutOrderingTable[activeBuffer], 0);

		for (i = 0; i < MAX_BRICKS; i++)
			{
			if (BricksArray[i].alive == TRUE)
				{
				GsSortSprite(&(BricksArray[i].sprite),
						&BreakoutOrderingTable[activeBuffer], 0);
				}
			}
	 	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &BreakoutOrderingTable[activeBuffer]);

		GsDrawOt(&BreakoutOrderingTable[activeBuffer]);

	 	FntPrint("Breakout!\n");
		FntPrint("Frame number: %d\n", frameNumber);
		FntPrint("Lives: %d\n", PlayerOneNumberOfLives);
		FntPrint("Score: %d\n", CurrentScore.points);
		FntPrint("Hsync count: %d\n", count);
		
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
					PlayerOneNumberOfLives = 5;
					DestroyAllBricks();
					CurrentScore.points = 0;
					SetUpLevel1();
					}
				else
					BreakoutQuitFlag = TRUE;
				}
			else
				SetUpLevel1();	
			}							
		
		if (PlayerOneNumberOfLives <= 0)			// player has lost
			{
			CrudestPause(30);
			playerWantsAnotherGame = DealWithPlayersDeath();
			if (playerWantsAnotherGame)
				{
				Level = 1;
				PlayerOneNumberOfLives = 5;
				DestroyAllBricks();
				CurrentScore.points = 0;
				SetUpLevel1();
				}
			else
				BreakoutQuitFlag = TRUE;
			}

		frameNumber++;
		}
}




#if 0				 // redefined in main.c
void BreakoutFontPrinting (void)
{
	//FntPrint("Breakout!\n");
	//FntPrint("Frame number: %d\n", frameNumber);
	FntPrint("\nLives: %d\n", PlayerOneNumberOfLives);
	FntPrint("Score: %d\n", CurrentScore.points);
	if (BreakoutAutoPilotFlag == TRUE)
		{
		FntPrint("Rolling demo\n");
		}
	else
		{
		assert(BreakoutAutoPilotFlag == FALSE);
		FntPrint("Play On\n");
		}
	//FntPrint("Hsync count: %d\n", count);
		
	//FntFlush(-1);
}
#endif





void BreakoutMainLoopGameLogic (void)
{
	int numberBricks = -1;

	HandleBreakoutPadControls();

	HandleAllBreakoutObjects();

#if 0
	if (BreakoutNumberOfPlayers == 1 && numberBricks == 0)	  // time for next level
		{
		//printf("\n\nHave Noticed No Bricks Left\n\n\n");
		CrudestPause(30);
		Level++;
		if (Level > 10)	  // reset anew
			{
			Level = 1;
			PlayerOneNumberOfLives = 5;
			DestroyAllBricks();
			CurrentScore.points = 0;
			SetUpLevel1();
			}
		else
			SetUpLevel1();	
		}
#endif							
		
	switch(BreakoutNumberOfPlayers)
		{
		case 1:
			numberBricks = CountNumberOfBricks();

			if (numberBricks == 0)	  // time for next level
				{
				//printf("\n\nHave Noticed No Bricks Left\n\n\n");
				CrudestPause(30);
				Level++;
				if (Level > 10)	  // reset anew
					{
					Level = 1;
					PlayerOneNumberOfLives = 5;
					DestroyAllBricks();
					CurrentScore.points = 0;
					SetUpLevel1();
					}
				else
					SetUpLevel1();	
				}

			if (PlayerOneNumberOfLives <= 0)			// player has lost
				{
				#if 0
				CrudestPause(30);
				Level = 1;
				PlayerOneNumberOfLives = 5;
				DestroyAllBricks();
				HandleFinalScore();
				CurrentScore.points = 0;
				SetUpLevel1();
				#endif
				GameHasFinishedAndResultsNeedDisplayingFlag	= TRUE;
				}
			break;
		case 2:
			if (PlayerOneNumberOfLives <= 0)			// player has lost
				{
				#if 0
				CrudestPause(30);
				Level = 1;
				PlayerOneNumberOfLives = 5;
				DestroyAllBricks();
				HandleFinalScore();
				CurrentScore.points = 0;
				SetUpLevel1();
				#endif
				GameHasFinishedAndResultsNeedDisplayingFlag	= TRUE;
				}
			if (PlayerTwoNumberOfLives <= 0)			// player has lost
				{
				#if 0
				CrudestPause(30);
				Level = 1;
				PlayerTwoNumberOfLives = 5;
				DestroyAllBricks();
				HandleFinalScore();
				CurrentScore.points = 0;
				SetUpLevel1();
				#endif
				GameHasFinishedAndResultsNeedDisplayingFlag	= TRUE;
				}
			break;
		default:
			assert(FALSE);
		}
}







void BreakoutDrawing (GsOT *ot)
{
	int i;

	switch(BreakoutNumberOfPlayers)
		{
		case 1:
			GsSortBoxFill(&BackgroundRectangle,	ot, 1);

			GsSortSprite(&PlayerOnesSprite, ot, 0);

			GsSortSprite(&BallsSprite, ot, 0);

			for (i = 0; i < MAX_BRICKS; i++)
				{
				if (BricksArray[i].alive == TRUE)
					{
					GsSortSprite(&(BricksArray[i].sprite), ot, 0);
					}
				}
			break;
		case 2:
			GsSortBoxFill(&BackgroundRectangle,	ot, 1);

			GsSortSprite(&PlayerOnesSprite, ot, 0);

			GsSortSprite(&PlayerTwosSprite, ot, 0);

			GsSortSprite(&BallsSprite, ot, 0);
			break;
		default:
			assert(FALSE);
		}

	//GsSortClear(64, 64, 128, ot);
}



void CrudestPause (int n)
{
	int i;

	for (i = 0; i < n; i++)
		VSync(0);
} 	
	 



  


	// all levels are identical at present
void SetUpLevel1 (void)
{
	int id;
	int i, j;	
	
	switch(BreakoutNumberOfPlayers)
		{
		case 1:			 
			RestartPlayersBats();

			RestartTheBall();

			for (i = 0; i < MAX_BRICKS; i++)
				{
				DestroyBrick(i);
				}
		   
				// HERE: set up bricks
			for (i = 0; i < 3; i++)
				{
				for (j = 0; j < 13; j++)
					{
					id = CreateNewBrick(FULL_BRICK, 1);
					BricksArray[id].movement.x = MIN_X + (18 * j);
					BricksArray[id].movement.y = MIN_Y + 29 + (i * 18);
					}
				}
			for (i = 0; i < 2; i++)
				{
				for (j = 0; j < 12; j++)
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
			break;
		case 2:
			RestartPlayersBats();

			RestartTheBall();

			for (i = 0; i < MAX_BRICKS; i++)
				{
				DestroyBrick(i);
				}
			break;
		default:
			assert(FALSE);
		}
}


	  


  	// simpler GUI loop: ask for new game or quit
int DealWithPlayersDeath (void)
{
	int newGame = -77;
	int position;
	int count, activeBuffer;
		// these below: decent pause between choice and action
	int frameNow = 0, whenToLeaveLoop = -1, exitSet = FALSE;
	int i;
	int padStatus;

		// find position (if any) in high score table
		// update table
	DealWithNewScore( &CurrentScore, HighScoreTable, 5, &position);

	for (;;)
		{
		padStatus = PadRead();

		if ( (padStatus & PADL1) && (padStatus & PADR1) )
			{
			if (exitSet == FALSE)
				{
				newGame = FALSE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;		
				}
			}  
		if (padStatus & PADRdown)
			{
			if (exitSet == FALSE)
				{
				newGame = TRUE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;
				}
			}

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) BreakoutGpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &BreakoutOrderingTable[activeBuffer]);

			// only background rectangle gets put into ordering table
		GsSortBoxFill(&BackgroundRectangle,	
						&BreakoutOrderingTable[activeBuffer], 0);	
	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &BreakoutOrderingTable[activeBuffer]);

		GsDrawOt(&BreakoutOrderingTable[activeBuffer]);

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
	int newGame = -77;
	int position;
	int count, activeBuffer;
		// these below: decent pause between choice and action
	int frameNow = 0, whenToLeaveLoop = -1, exitSet = FALSE;
	int padStatus;
	int i;

	DealWithNewScore( &CurrentScore, HighScoreTable, 5, &position);

	for (;;)
		{
		padStatus = PadRead();

		if ( (padStatus & PADL1) && (padStatus & PADR1) )
			{
			if (exitSet == FALSE)
				{
				newGame = FALSE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;		
				}
			}  
		if (padStatus & PADRdown)
			{
			if (exitSet == FALSE)
				{
				newGame = TRUE;
				whenToLeaveLoop = frameNow + 60;
				exitSet = TRUE;
				}
			}

		activeBuffer = GsGetActiveBuff();
		GsSetWorkBase( (PACKET *) BreakoutGpuPacketArea[activeBuffer] );

		GsClearOt(0, 0, &BreakoutOrderingTable[activeBuffer]);

		GsSortBoxFill(&BackgroundRectangle,	
						&BreakoutOrderingTable[activeBuffer], 0);	
	  
		DrawSync(0);							
		count = VSync(0);			
		GsSwapDispBuff();		

		GsSortClear(60, 120, 120, &BreakoutOrderingTable[activeBuffer]);

		GsDrawOt(&BreakoutOrderingTable[activeBuffer]);

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

	//HighScore.points = 3000;
	//strcpy(HighScore.name, "Whizzkid");
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
	



void HandleFinalScore (void)
{
	if (CurrentScore.points > HighScore.points)
		{
		HighScore.points = CurrentScore.points;
		}
}



void InitialiseBreakout (void)
{
	int i;

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
		BreakoutOrderingTable[i].length = BREAKOUT_OT_LENGTH;
		BreakoutOrderingTable[i].org = BreakoutOrderingTableTags[i];
		}
		
	PadInit();
	
	FntLoad(960, 256);	
	FntOpen(16, 16, 256, 200, 0, 512);

	InitialiseBreakoutObjects();

		// load .TIM files into memory, link with sprites
	InitialiseBreakoutTexturesAndSprites();

	ClearBreakoutBackground();

	Level = 1;
	PlayerOneNumberOfLives = 5;

	SetUpLevel1();

	ClearScores();
}	  



void InitialiseBreakout2 (void)
{
	int i;	
	   
   	for (i = 0; i < 2; i++) 
   		{
		BreakoutOrderingTable[i].length = BREAKOUT_OT_LENGTH;
		BreakoutOrderingTable[i].org = BreakoutOrderingTableTags[i];
		}

	InitialiseBreakoutObjects();

		// load .TIM files into memory, link with sprites
	InitialiseBreakoutTexturesAndSprites();

	ClearBreakoutBackground();

	Level = 1;
	PlayerOneNumberOfLives = 5;
	PlayerTwoNumberOfLives = 5;

	SetUpLevel1();

	ClearScores();
}	  







void ResetBreakout (void)
{
	Level = 1;
	PlayerOneNumberOfLives = 5;
	PlayerTwoNumberOfLives = 5;
	ClearScores();
}






void InitialiseBreakoutObjects (void)
{
	int i;

		// sort out the player's bat
	PlayerOnesMovement.x = 0;
	PlayerOnesMovement.y = 0;
	PlayerOnesMovement.width = 40;		
	PlayerOnesMovement.height = 8;	
	PlayerOnesMovement.dx = 0;
	PlayerOnesMovement.dy = 0;
	InitGsSprite(&PlayerOnesSprite);

		// sort out the player's bat
	PlayerTwosMovement.x = 0;
	PlayerTwosMovement.y = 0;
	PlayerTwosMovement.width = 40;		
	PlayerTwosMovement.height = 8;	
	PlayerTwosMovement.dx = 0;
	PlayerTwosMovement.dy = 0;
	InitGsSprite(&PlayerTwosSprite);

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


  
	// just using a global array with IDs to track objects
	// scope for improvement here
int CreateNewBrick (int type, int toughness)
{
	int id = -1; 
	int i;

	for (i = 0; i < MAX_BRICKS; i++)
		{
		if (BricksArray[i].alive == FALSE)
			{
			id = i;
			break;
			}
		}

	if (id == -1)
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
	//BallsMovement.x = SCREEN_WIDTH/2 - (BallsMovement.width/2);
	BallsMovement.x = SCREEN_WIDTH/2 - (BallsMovement.width/2) + (rand() % 25);
	BallsMovement.y = SCREEN_HEIGHT/2 - (BallsMovement.height/2);
	BallsMovement.dx = 0;

	if (BackgroundGraphicsFlag == PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE)
		BallsMovement.dy = 1;
	else
		BallsMovement.dy = 3;
}

	
void RestartPlayersBats (void)
{
		// first player starts stationary at bottom-middle of screen
	PlayerOnesMovement.x = SCREEN_WIDTH/2 - (PlayerOnesMovement.width/2);
	PlayerOnesMovement.y = MAX_Y - PlayerOnesMovement.height;
	PlayerOnesMovement.dx = 0;
	PlayerOnesMovement.dy = 0;

		// second player starts stationary at bottom-middle of screen
	PlayerTwosMovement.x = SCREEN_WIDTH/2 - (PlayerTwosMovement.width/2);
	PlayerTwosMovement.y = MIN_Y + PlayerOnesMovement.height;
	PlayerTwosMovement.dx = 0;
	PlayerTwosMovement.dy = 0;
}



		
	

GsIMAGE Bat1ImageInfo, Bat2ImageInfo, BrickImageInfo;

RECT BallsPixels, BallsClut;

void InitialiseBreakoutTexturesAndSprites (void)
{ 
	u_short texturePageID3;
	u_long mask1 = 1L << 29;
	u_long mask2 = 1L << 30;
	int i;

		// player one's bat
	ProperInitialiseTexture(BREAKOUT_BAT_1_TEXTURE_ADDRESS, &Bat1ImageInfo);
	LinkSpriteToImageInfo( &PlayerOnesSprite, &Bat1ImageInfo);
	PlayerOnesSprite.mx = PlayerOnesSprite.my = 0;
	//PlayerOnesSprite.r = 255;
	//PlayerOnesSprite.g = 255;
	//PlayerOnesSprite.b = 255;


		// player two's bat
	ProperInitialiseTexture(BREAKOUT_BAT_2_TEXTURE_ADDRESS, &Bat2ImageInfo);
	LinkSpriteToImageInfo( &PlayerTwosSprite, &Bat2ImageInfo);
	PlayerTwosSprite.mx = PlayerTwosSprite.my = 0;
	//PlayerTwosSprite.r = 96;
	PlayerTwosSprite.g = 80;
	//PlayerTwosSprite.b = 96;


	 
	 
		// the bricks
	ProperInitialiseTexture(BREAKOUT_BRICK_TEXTURE_ADDRESS, &BrickImageInfo);
	for (i = 0; i < MAX_BRICKS; i++)
		{
		LinkSpriteToImageInfo( &BricksArray[i].sprite, &BrickImageInfo);
		BricksArray[i].sprite.attribute |= mask1;
		BricksArray[i].sprite.attribute |= mask2;
		BricksArray[i].sprite.scalex = ONE/4;	// squash to 1/4
		BricksArray[i].sprite.scaley = ONE/4;	// squash to 1/4
		//BricksArray[i].sprite.r = 255;
		//BricksArray[i].sprite.g = 255;
		//BricksArray[i].sprite.b = 255;
		}



		 // the ball
	setRECT( &BallsPixels, 704, 256, 4, 16);	   
	LoadImage( &BallsPixels, ball16x16);
	DrawSync(0);
	setRECT( &BallsClut, 704, 280, 16, 1); 
	LoadImage( &BallsClut, ballcolor[4]);
	DrawSync(0);

		// sort bricks' sprites
	texturePageID3 = GetTPage(0, 0, BallsPixels.x, BallsPixels.y);

	BallsSprite.w = BallsPixels.w * 4;	   
	BallsSprite.h = BallsPixels.h;
	BallsSprite.tpage = texturePageID3;
	BallsSprite.cx = BallsClut.x;
	BallsSprite.cy = BallsClut.y;
	BallsSprite.scalex = ONE/2;		 // squash by 1/2
	BallsSprite.scaley = ONE/2;		 // squash by 1/2
	//BallsSprite.r = 255;
	//BallsSprite.g = 255;
	//BallsSprite.b = 255;
}







	// set up background rectangle: main use is to stop sprites
	// having black-box edges
	// (sprites created very simply in PaintShopPro, 
	// converted via timutil) 

void ClearBreakoutBackground (void)
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
	BackgroundRectangle.r = 32;
	BackgroundRectangle.g = 32;
	BackgroundRectangle.b = 64;
}	


	
   
void HandleBreakoutPadControls (void)
{
	int pad1Status, pad2Status;

	pad1Status = PadRead();

	switch(BreakoutNumberOfPlayers)
		{
		case 1:		
			if (BreakoutAutoPilotFlag == FALSE)		// human player
				{
						
					// player moves the bat: Lleft and Lright
				if (pad1Status & PADLleft)
					{
					PlayerOnesMovement.dx -= PLAYER_SPEED;
					KeepWithinRange(PlayerOnesMovement.dx, 
						-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
					}
				if (pad1Status & PADLright)
					{
					PlayerOnesMovement.dx += PLAYER_SPEED;
					KeepWithinRange(PlayerOnesMovement.dx, 
						-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
					}
				}
			else
				{
				SortAutoPilotActions();
				}
			break;
		case 2:
				// player 1 moves the bat: Rleft and Rright
			if (pad1Status & PADLleft)
				{
				PlayerOnesMovement.dx -= PLAYER_SPEED;
				KeepWithinRange(PlayerOnesMovement.dx, 
					-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
				}
			if (pad1Status & PADLright)
				{
				PlayerOnesMovement.dx += PLAYER_SPEED;
				KeepWithinRange(PlayerOnesMovement.dx, 
					-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
				}

			pad2Status = pad1Status >> 16;

				// player 2 moves the bat: Rleft and Rright
				// player 2's moves inverted (upside down)
			if (pad2Status & PADLleft)
				{
				PlayerTwosMovement.dx += PLAYER_SPEED;
				KeepWithinRange(PlayerTwosMovement.dx, 
					-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
				}
			if (pad2Status & PADLright)
				{
				PlayerTwosMovement.dx -= PLAYER_SPEED;
				KeepWithinRange(PlayerTwosMovement.dx, 
					-MAX_PLAYER_X_SPEED, MAX_PLAYER_X_SPEED);
				}
			break;
		default:
			assert(FALSE);
		}	
}









#define SIMPLEST_AUTO_PILOT 0
#define JIGGLING_AUTO_PILOT 1


	// simple autopilot, same affects on game as human player

void SortAutoPilotActions (void)
{
	assert(BreakoutAutoPilotFlag == TRUE);

#if SIMPLEST_AUTO_PILOT
	{
	int ballX, playerX;
	int dx;

	ballX = BallsMovement.x + (BallsMovement.width/2);
	playerX = PlayerOnesMovement.x + (PlayerOnesMovement.width/2);
	
	dx = ballX - playerX;		// +ve iff player to left of ball

		// clip actual move to max possible movement speed
	if (dx > PLAYER_SPEED)
		dx = PLAYER_SPEED;
	else if (dx < -PLAYER_SPEED)
		dx = -PLAYER_SPEED;
		
	PlayerOnesMovement.dx = dx;
	}
#elif JIGGLING_AUTO_PILOT		
	{		   // very similar, but more jiggling
	int ballX, playerX;
	int dx;
	static int jiggleFlag = -1;
	static int jiggleStartFrame = -1; 
	int jiggleDuration = 3;		// arbitrary
	int safeLimit = 8;			// arbitrary
	int jiggleFrequency = 12;			// arbitrary

	ballX = BallsMovement.x + (BallsMovement.width/2);
	playerX = PlayerOnesMovement.x + (PlayerOnesMovement.width/2);
	
	dx = ballX - playerX;		// +ve iff player to left of ball

	if (abs(dx) <= safeLimit)
		{
		if (jiggleFlag != -1)		// if mid-jiggle, carry on
			{
			if (frameNumber - jiggleStartFrame >= jiggleDuration)   // time to end jiggle
				{
				jiggleFlag = -1;
				jiggleStartFrame = -1;

					// clip actual move to max possible movement speed
				if (dx > PLAYER_SPEED)
					dx = PLAYER_SPEED;
				else if (dx < -PLAYER_SPEED)
					dx = -PLAYER_SPEED;
					
				PlayerOnesMovement.dx = dx;
				}
			else		// do jiggle
				{
				switch(jiggleFlag)
					{
					case LEFT:
						PlayerOnesMovement.dx = -PLAYER_SPEED;
						break;
					case RIGHT:
						PlayerOnesMovement.dx = PLAYER_SPEED;
						break;
					default:
						assert(FALSE);
					}
				}	
			}
		else	// think of starting one
			{
			if ((rand() % jiggleFrequency) == 0)
				{
				if (rand() % 2 == 0)	
					jiggleFlag = LEFT;
				else
					jiggleFlag = RIGHT;
				jiggleStartFrame = frameNumber;
				switch(jiggleFlag)
					{
					case LEFT:
						PlayerOnesMovement.dx = -PLAYER_SPEED;
						break;
					case RIGHT:
						PlayerOnesMovement.dx = PLAYER_SPEED;
						break;
					default:
						assert(FALSE);
					}
				}
			else
				{
					// clip actual move to max possible movement speed
				if (dx > PLAYER_SPEED)
					dx = PLAYER_SPEED;
				else if (dx < -PLAYER_SPEED)
					dx = -PLAYER_SPEED;
					
				PlayerOnesMovement.dx = dx;
				}
			}
		}
	else		   // not safe, go for ball
		{
			// clip actual move to max possible movement speed
		if (dx > PLAYER_SPEED)
			dx = PLAYER_SPEED;
		else if (dx < -PLAYER_SPEED)
			dx = -PLAYER_SPEED;
			
		PlayerOnesMovement.dx = dx;
		}
	}
#else
	{
	printf("BAD autopilot type\n");
	}
#endif 
}



 

	// handle movements of player's bat, the ball;
	// handle collisions of ball with player's bat and with bricks
	// scope for optimisation here is Massive

	// this is NOT intended as an example of how to do collision-detection
	// and collision-handling: this was done quickly to make it work
	
void HandleAllBreakoutObjects (void)
{
	RECT player1, player2, ball, brick;	// collision rectangles
	int edge;			// of brick rectangle: where did ball hit?
	int xInitialDistance, xLeftOver;
	int yInitialDistance, yLeftOver;
	int ballMoved = FALSE;
	int part;	
	int i;

 	
	if (BreakoutNumberOfPlayers == 1)	{
		// sort bat's collision rectangle
	player1.x = PlayerOnesMovement.x;
	player1.y = PlayerOnesMovement.y;
	player1.w = PlayerOnesMovement.width;
	player1.h = PlayerOnesMovement.height;

		// sort ball's collision rectangle
	ball.x = BallsMovement.x;
	ball.y = BallsMovement.y;
	ball.w = BallsMovement.width;
	ball.h = BallsMovement.height;

		// handle ball hitting player's bat:
		// bat nudges ball along in x-direction only	
	if (RectanglesTouch(&player1, &ball) 
				|| RectanglesOverlap(&player1, &ball))
		{
		if (RectanglesOverlap(&player1, &ball))
			{
			MoveBallOutOfBat(PlayerOnesMovement.y);
			}
	
			// if below top-of-bat line, reflect off side of bat
		if (BallsMovement.y > 
				(MAX_Y-PlayerOnesMovement.height-BallsMovement.height))
			{
			if (BallsMovement.dx == 0)
				BallsMovement.dx = PlayerOnesMovement.dx;
			else if (SIGN_OF(BallsMovement.dx) 	// bat 'overtaking' ball
				== SIGN_OF(PlayerOnesMovement.dx))
				{
				BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
				}
			else		// moving in opposite directions
				{
				BallsMovement.dx = -BallsMovement.dx 
									+ (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
				}
			}
		else
			{
				// first: reflect off top of bat
			BallsMovement.dy = -BallsMovement.dy;
		   		
				// effect of player's bat: nudge along x-wise
				// effect depends on bat's velocity
			BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
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
						case OVERLAP:
							part = FindWherePointIsInRectangle (ball.x, ball.y, &brick);
								
							switch(part)
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
								case MIDDLE:
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
									assert(FALSE);
								}
							break;
						default:
							printf("BAD VALUE %d\n", edge);
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
		PlayerOneNumberOfLives--;
		CrudestPause(30);		// half a second
		RestartTheBall();
		RestartPlayersBats();
		}

		// update sprite display position
	BallsSprite.x = (short int) BallsMovement.x-1;
	BallsSprite.y = (short int) BallsMovement.y-1;
	   

	   	 
	 	// last section: handle the player's bat

		// update player's bat movement
	PlayerOnesMovement.x += PlayerOnesMovement.dx;
	PlayerOnesMovement.y += PlayerOnesMovement.dy;

		// HERE: deny momentum to the player's bat
	PlayerOnesMovement.dx = 0;
	PlayerOnesMovement.dy = 0;

		// bat stops at walls
	if (PlayerOnesMovement.x > MAX_X-PlayerOnesMovement.width)
		{
		PlayerOnesMovement.x = MAX_X-PlayerOnesMovement.width;
		PlayerOnesMovement.dx = 0;
		}
	else if (PlayerOnesMovement.x < MIN_X)
		{
		PlayerOnesMovement.x = MIN_X;
		PlayerOnesMovement.dx = 0;
		}

		// update sprite display position
	PlayerOnesSprite.x = (short int) PlayerOnesMovement.x;
	PlayerOnesSprite.y = (short int) PlayerOnesMovement.y;
	}
	else		  // two-player mode
		{
		assert(BreakoutNumberOfPlayers == 2);

		player1.x = PlayerOnesMovement.x;
		player1.y = PlayerOnesMovement.y;
		player1.w = PlayerOnesMovement.width;
		player1.h = PlayerOnesMovement.height;

		player2.x = PlayerTwosMovement.x;
		player2.y = PlayerTwosMovement.y;
		player2.w = PlayerTwosMovement.width;
		player2.h = PlayerTwosMovement.height;

			// sort ball's collision rectangle
		ball.x = BallsMovement.x;
		ball.y = BallsMovement.y;
		ball.w = BallsMovement.width;
		ball.h = BallsMovement.height;

		if (RectanglesTouch(&player1, &ball) 	// does ball hit player one's bat?
				|| RectanglesOverlap(&player1, &ball))
			{
			if (RectanglesOverlap(&player1, &ball))
				{
				MoveBallOutOfBat(PlayerOnesMovement.y);
				}
		
				// if below top-of-bat line, reflect off side of bat
			if (BallsMovement.y > 
					(MAX_Y-PlayerOnesMovement.height-BallsMovement.height))
				{
				if (BallsMovement.dx == 0)
					BallsMovement.dx = PlayerOnesMovement.dx;
				else if (SIGN_OF(BallsMovement.dx) 	// bat 'overtaking' ball
					== SIGN_OF(PlayerOnesMovement.dx))
					{
					BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
					}
				else		// moving in opposite directions
					{
					BallsMovement.dx = -BallsMovement.dx 
										+ (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
					}
				}
			else
				{
					// first: reflect off top of bat
				BallsMovement.dy = -BallsMovement.dy;
			   		
					// effect of player's bat: nudge along x-wise
					// effect depends on bat's velocity
				BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerOnesMovement.dx);
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
		else	   // check if ball hit player two's bat
			{
			if (RectanglesTouch(&player2, &ball) 
				|| RectanglesOverlap(&player2, &ball))
				{
				if (RectanglesOverlap(&player2, &ball))
					{
					MoveBallOutOfBat(PlayerTwosMovement.y);
					}
			
					// if below top-of-bat line, reflect off side of bat
				if (BallsMovement.y > 
						(MAX_Y-PlayerTwosMovement.height-BallsMovement.height))
					{
					if (BallsMovement.dx == 0)
						BallsMovement.dx = PlayerTwosMovement.dx;
					else if (SIGN_OF(BallsMovement.dx) 	// bat 'overtaking' ball
						== SIGN_OF(PlayerTwosMovement.dx))
						{
						BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerTwosMovement.dx);
						}
					else		// moving in opposite directions
						{
						BallsMovement.dx = -BallsMovement.dx 
											+ (BreakoutCollisionCoefficient * PlayerTwosMovement.dx);
						}
					}
				else
					{
						// first: reflect off top of bat
					BallsMovement.dy = -BallsMovement.dy;
				   		
						// effect of player's bat: nudge along x-wise
						// effect depends on bat's velocity
					BallsMovement.dx += (BreakoutCollisionCoefficient * PlayerTwosMovement.dx);
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
		if (BallsMovement.y < MIN_Y+BallsMovement.height)			// hits ceiling
			{								// player 2 loses a life
			PlayerTwoNumberOfLives--;
			CrudestPause(30);		// half a second
			RestartTheBall();
			RestartPlayersBats();
			}
		if (BallsMovement.y > MAX_Y-BallsMovement.height) 	 // hits floor
			{									// player 1 loses a life
			PlayerOneNumberOfLives--;
			CrudestPause(30);		// half a second
			RestartTheBall();
			RestartPlayersBats();
			}

			// update sprite display position
		BallsSprite.x = (short int) BallsMovement.x-1;
		BallsSprite.y = (short int) BallsMovement.y-1;
	   

	   	 
	 	// last section: handle the players' bats

			// update player's bat movement
		PlayerOnesMovement.x += PlayerOnesMovement.dx;
		PlayerOnesMovement.y += PlayerOnesMovement.dy;

			// HERE: deny momentum to the player's bat
		PlayerOnesMovement.dx = 0;
		PlayerOnesMovement.dy = 0;

			// bat stops at walls
		if (PlayerOnesMovement.x > MAX_X-PlayerOnesMovement.width)
			{
			PlayerOnesMovement.x = MAX_X-PlayerOnesMovement.width;
			PlayerOnesMovement.dx = 0;
			}
		else if (PlayerOnesMovement.x < MIN_X)
			{
			PlayerOnesMovement.x = MIN_X;
			PlayerOnesMovement.dx = 0;
			}

			// update sprite display position
		PlayerOnesSprite.x = (short int) PlayerOnesMovement.x;
		PlayerOnesSprite.y = (short int) PlayerOnesMovement.y;

			// similarly for player two
		PlayerTwosMovement.x += PlayerTwosMovement.dx;
		PlayerTwosMovement.y += PlayerTwosMovement.dy;

			// HERE: deny momentum to the player's bat
		PlayerTwosMovement.dx = 0;
		PlayerTwosMovement.dy = 0;

			// bat stops at walls
		if (PlayerTwosMovement.x > MAX_X-PlayerTwosMovement.width)
			{
			PlayerTwosMovement.x = MAX_X-PlayerTwosMovement.width;
			PlayerTwosMovement.dx = 0;
			}
		else if (PlayerTwosMovement.x < MIN_X)
			{
			PlayerTwosMovement.x = MIN_X;
			PlayerTwosMovement.dx = 0;
			}

			// update sprite display position
		PlayerTwosSprite.x = (short int) PlayerTwosMovement.x;
		PlayerTwosSprite.y = (short int) PlayerTwosMovement.y;
		}
}







	// push ball upwards, out of bat area
void MoveBallOutOfBat (int playerBatY)
{
	if (((int)BallsMovement.y) + ((int)BallsMovement.height)
			> ((int)playerBatY))
				{
				BallsMovement.y 
					= playerBatY - BallsMovement.height;
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
	int whereTheyTouch = -55;		// TOP, BOTTOM, LEFT, RIGHT
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
		default:
			assert(FALSE);
		}
		 
	return whereTheyTouch;
}
	

				  	  


int FindWherePointIsInRectangle (int x, int y, RECT *rect)
{
	int part;

	if (x == rect->x + (rect->w/2))		 // dead horizontal centre
		{
		if (y == rect->y + (rect->h/2))
			{
			part = MIDDLE;
			}
		else if (y < rect->y + (rect->h/2))
			{
			part = TOP;
			}
		else
			{
			part = BOTTOM;
			}
		}
	else if	(x < rect->x + (rect->w/2))   // left
		{
		if (y == rect->y + (rect->h/2))
			{
			part = LEFT;
			}
		else if (y < rect->y + (rect->h/2))
			{
			part = TOP_LEFT;
			}
		else
			{
			part = BOTTOM_LEFT;
			}
		}
	else		  // right
		{
		if (y == rect->y + (rect->h/2))
			{
			part = RIGHT;
			}
		else if (y < rect->y + (rect->h/2))
			{
			part = TOP_RIGHT;
			}
		else
			{
			part = BOTTOM_RIGHT;
			}
		}

	return part;
}
	
  	
