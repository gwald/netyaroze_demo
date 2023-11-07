#include "menu.h"



/****************************************************************************
					 globals
****************************************************************************/




int FramesSinceLastUserSelection;


int FrameWhenNextChangeState;
int DelayedNewState;


int OverallGameState;

int OpeningSequenceQuitFlag;
int MainMenuQuitFlag;
int SettingUpTheRaceQuitFlag;
int WaitingForTheRaceQuitFlag;
int DuringTheRaceQuitFlag;
int AfterTheRaceQuitFlag;
int ViewingTheTracksQuitFlag;


int MainMenuChoice;



int SettingUpTheRaceSelectionMode;

int SettingUpTheRaceSubSelectionMode;	
		
int SettingUpTheRaceTrackNumber;
int SettingUpTheRaceNumberOfDrivers;
int SettingUpTheRaceNumberOfLaps;







/****************************************************************************
					 macros
****************************************************************************/





#if 0
#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	
#endif







/****************************************************************************
					 functions
****************************************************************************/






void InitialiseTheGameState (void)
{
	FramesSinceLastUserSelection = 0;

	FrameWhenNextChangeState = -1;
	DelayedNewState = -1;

	OverallGameState = OPENING_SEQUENCE;

	ResetAllGuiLoopQuitFlags();

	MainMenuChoice = FREE_PRACTICE;

	ResetSettingUpTheRaceData();

	InitialiseGameGivenGameState();

	InitialiseRace();
}






void ResetAllGuiLoopQuitFlags (void)
{
	OpeningSequenceQuitFlag = FALSE;
	MainMenuQuitFlag = FALSE;
	SettingUpTheRaceQuitFlag = FALSE;
	WaitingForTheRaceQuitFlag = FALSE;
	DuringTheRaceQuitFlag = FALSE;
	AfterTheRaceQuitFlag = FALSE;
	ViewingTheTracksQuitFlag = FALSE;

	EndRaceNowFlag = FALSE;
}




void ResetSettingUpTheRaceData (void)
{
	SettingUpTheRaceSelectionMode = CHOOSE_TRACK;	
		
	SettingUpTheRaceTrackNumber = 1;
	SettingUpTheRaceNumberOfDrivers = 1;
	SettingUpTheRaceNumberOfLaps = 1;
}





	// no longer very applicable, always initialise to OPENING_SEQUENCE 

void InitialiseGameGivenGameState (void)
{
	switch(OverallGameState)
		{
		case OPENING_SEQUENCE:
			// do nowt, all is already set up
			break;
		case MAIN_MENU:
		case SETTING_UP_THE_RACE:		// should never get here
		case WAITING_FOR_THE_RACE:		// should never get here
		case DURING_THE_RACE:		    // should never get here
		case AFTER_THE_RACE:		    // should never get here
		case VIEWING_THE_TRACKS:		// should never get here
			assert(FALSE);			
			break;					  
		default:
			assert(FALSE);
		}
}




void SetANewGameState (int newState)
{
	int oldState;

	printf("game state %d, new state %d\n",
			  OverallGameState, newState);

	// make a noise ....

	oldState = OverallGameState;
	assert(newState != oldState);
	assert(newState >= 0 && newState < MAX_GAME_MODES);

	OverallGameState = newState;

	ResetAllGuiLoopQuitFlags();
	switch(oldState)
		{
		case OPENING_SEQUENCE:
			OpeningSequenceQuitFlag = TRUE;
			break;
		case MAIN_MENU:
			MainMenuQuitFlag = TRUE;
			break;
		case SETTING_UP_THE_RACE:
			SettingUpTheRaceQuitFlag = TRUE;
			break;
		case WAITING_FOR_THE_RACE:
			WaitingForTheRaceQuitFlag = TRUE;
			break;
		case DURING_THE_RACE:
			DuringTheRaceQuitFlag = TRUE;
			break;
		case AFTER_THE_RACE:
			AfterTheRaceQuitFlag = TRUE;
			break;
		case VIEWING_THE_TRACKS:
			ViewingTheTracksQuitFlag = TRUE;
			break;
		default:
			assert(FALSE);
		}

	HandleGameStateTransition(oldState, newState);
}



void SetDelayedChangeInGameState (int newState, int frameDelay)
{
	FrameWhenNextChangeState = frameNumber + frameDelay;
	DelayedNewState = newState;
}



	// call this handler in all gui loops

void DetectDelayedChangeInGameState (void)
{
	if (frameNumber == FrameWhenNextChangeState)
		{
		SetANewGameState (DelayedNewState);

		FrameWhenNextChangeState = -1;
		DelayedNewState = -1;
		}
}





	// here: must do things
	// eg after race, set flag to clear things,
	// after set up race, should create tunnel, etc

	// MAKE NOISES to show state transition

void HandleGameStateTransition (int oldState, int newState)
{
	//char *strings[1];
	//char buffer[64];

	assert(newState != oldState);

		// never go back to opening sequence
	assert(newState != OPENING_SEQUENCE);

	FramesSinceLastUserSelection = 0;


	if (oldState == MAIN_MENU && newState == SETTING_UP_THE_RACE)
		{
		assert(MainMenuChoice == FREE_PRACTICE
				|| MainMenuChoice == STRAIGHT_RACE
				|| MainMenuChoice == THE_WORKS
				|| MainMenuChoice == VIEW_THE_TRACKS);
		}
	if (oldState == MAIN_MENU && newState == WAITING_FOR_THE_RACE)
		{	
		assert(MainMenuChoice == ROLLING_DEMO);
		PrepareTheRaceForRollingDemo();

		//sprintf(buffer, "Building the tunnel");
		//strings[0] = &buffer[0];
		//GuiLoopForPrintingTextStrings (1, strings);
		}

	if (oldState == SETTING_UP_THE_RACE && newState == WAITING_FOR_THE_RACE)
		{
		assert(MainMenuChoice == FREE_PRACTICE
				|| MainMenuChoice == STRAIGHT_RACE
				|| MainMenuChoice == THE_WORKS);
		PrepareTheRaceFromSettingUpData();
			
		//sprintf(buffer, "Building the tunnel");
		//strings[0] = &buffer[0];
		//GuiLoopForPrintingTextStrings (1, strings);
		}
	if (oldState == SETTING_UP_THE_RACE && newState == VIEWING_THE_TRACKS)
		{
		assert(MainMenuChoice == VIEW_THE_TRACKS);

		PrepareTheTrackForViewing();
		}
	if (oldState == VIEWING_THE_TRACKS && newState == SETTING_UP_THE_RACE)
		{
		assert(MainMenuChoice == VIEW_THE_TRACKS);

		ResetTrackViewingData();
		}	

	if (newState == MAIN_MENU)			 // return to main menu
		{
		MainMenuChoice = FREE_PRACTICE;
		}

	if (newState == SETTING_UP_THE_RACE)
		{
		SortSettingUpTheRaceFromMainMenuChoice();
		}

		// exit race completely, reset all race data
	if (oldState == DURING_THE_RACE && newState == MAIN_MENU)
		{	
		InitTunnelToVoid();
		SetBasicTunnelParameters();
		InitialiseObjects();
		}

		// exit track viewer completely, reset all race data
	if (oldState == VIEWING_THE_TRACKS && newState == MAIN_MENU)
		{	
		InitTunnelToVoid();
		SetBasicTunnelParameters();
		InitialiseObjects();
		}

	if (oldState == DURING_THE_RACE && newState == AFTER_THE_RACE)
		{
		//sprintf(buffer, "End of race");
		//strings[0] = &buffer[0];
		//GuiLoopForPrintingTextStrings (1, strings);

		SortShipPositionsAndBestLapTimes();
		}
}





void SortSettingUpTheRaceFromMainMenuChoice (void)
{	
	switch(MainMenuChoice)
		{
		case FREE_PRACTICE:
		case VIEW_THE_TRACKS:
			SettingUpTheRaceSubSelectionMode = ONE_OF_THREE;
			break;
		case ROLLING_DEMO:
			assert(FALSE);	// don't go into setting up the race
			break;
		case STRAIGHT_RACE:
		case THE_WORKS:
			SettingUpTheRaceSubSelectionMode = ALL_THREE;
			break;
		default:
			assert(FALSE);
		}

	ResetSettingUpTheRaceData();
}



void HandleTheGameStateMachine (void)
{
	for (;;)
		{
		printf("deciding which game loop; state is %d\n", OverallGameState);
		switch (OverallGameState)
			{
			case OPENING_SEQUENCE:
				OpeningSequenceLoop();
				break;
			case MAIN_MENU:
				GuiLoopForTheMainMenu();
				break;
			case SETTING_UP_THE_RACE:
				GuiLoopForSettingUpTheRace();
				break;
			case WAITING_FOR_THE_RACE:
				GuiLoopWaitingForTheRaceToStart();
				break;
			case DURING_THE_RACE:
				GuiLoopDuringTheRace();
				break;
			case AFTER_THE_RACE:
				GuiLoopForPrintingTheRaceResults();
				break;
			case VIEWING_THE_TRACKS:
				GuiLoopForViewingTheTracks();
				break;
			default:
				assert(FALSE);
			}		
		}
}





 

void OpeningSequenceLoop (void)
{
	int bufferIndex;			  
	int openingSequenceStartFrame;
    int openingSequenceDuration = 96 + (3 * 48);
	GsSPRITE sprite;
	   	
	LinkSpriteToImageInfo( &sprite, &FireBackgroundImageInfo);
	// want to cover screen, so put at far top left
	sprite.x = -160; sprite.y = -120;
	sprite.scalex = (ONE * 320) / 240; 

	openingSequenceStartFrame = frameNumber;
	bufferIndex = GsGetActiveBuff();
	while (OpeningSequenceQuitFlag == FALSE)
		{		
		HandleSound();

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		if (frameNumber - openingSequenceStartFrame > 2)
			{
			GsSortSprite( &sprite, &Wot[bufferIndex], 0);
			}

		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In OpeningSequenceLoop\n\n");
		//	}

		#if 0
		if (frameNumber - openingSequenceStartFrame > 60)
			{
			CycleFireBackgroundTextureClut(openingSequenceStartFrame+60);
			}
		#endif

		SortFireBackgroundTextureRGB(&sprite, 
			(frameNumber - openingSequenceStartFrame) );

		if (frameNumber - openingSequenceStartFrame > openingSequenceDuration)
			{
			printf("End of opening sequence\n");
			SetANewGameState(MAIN_MENU);
			}
		}
}




void CycleFireBackgroundTextureClut (int startFrame)
{
	static int firstTimeFlag = TRUE;
	static u_short permanentStore[256];
	u_short temporaryStore[256];
	RECT clutRect;
	int cyclePhase;
	int clutCyclingPeriod = 128;
	int frequency = 10;
	int i;

	setRECT( &clutRect, FireBackgroundImageInfo.cx,
		FireBackgroundImageInfo.cy, 256, 1);

	if (firstTimeFlag == TRUE)
		{
		StoreImage( &clutRect, (u_long*) permanentStore);
		DrawSync(0);
		firstTimeFlag = FALSE;
		}
	else
		{
		if (((frameNumber - startFrame) % frequency) == 0)
			{
			cyclePhase = (frameNumber - startFrame) / clutCyclingPeriod;

			for (i = cyclePhase; i < 256; i++)
				{
				temporaryStore[i] 
					= permanentStore[i - cyclePhase];
				}
			for (i = 0; i < cyclePhase; i++)
				{
				temporaryStore[i] 
					= permanentStore[(256-cyclePhase)+i];
				}
			}

#if 0
		cyclePhase = (frameNumber - startFrame) % clutCyclingPeriod;

		for (i = cyclePhase; i < 256; i++)
			{
			temporaryStore[i] 
				= permanentStore[i - cyclePhase];
			}
		for (i = 0; i < cyclePhase; i++)
			{
			temporaryStore[i] 
				= permanentStore[(256-cyclePhase)+i];
			}
#endif

		LoadImage( &clutRect, (u_long*) temporaryStore);
		DrawSync(0);
		}
}




void SortFireBackgroundTextureRGB (GsSPRITE *sprite, int frame)
{
	if (frame >= 0 && frame < 96)
		{
		sprite->r = (frame * 4) / 3;
		sprite->g = (frame * 4) / 3;
		sprite->b = (frame * 4) / 3; 
		}
	else if (frame >= 96 && frame < 144)
		{
		sprite->g = 4 * (144 - frame);
		}
	else if (frame >= 144 && frame < 192)
		{
		sprite->b = 4 * (192 - frame);
		}
	else if (frame >= 192 && frame < 240)
		{
		sprite->r = 4 * (240 - frame);
		}	
}
			  
	  

	// gui loop: handles sound and text strings

void GuiLoopForTheMainMenu (void)
{
	int bufferIndex;			  // buffer index

	bufferIndex = GsGetActiveBuff();
	while (MainMenuQuitFlag == FALSE)
		{		
		//DealWithControllerForMainMenu();

		if (MainMenuChoice == FREE_PRACTICE)
			RegisterTextStringForDisplay ("Free Practice", -50, -40, 16);
		else
			RegisterTextStringForDisplay ("Free Practice", -50, -40, NORMAL_COLOUR);

		if (MainMenuChoice == ROLLING_DEMO)
			RegisterTextStringForDisplay ("Rolling Demo", -50, -20, 16);
		else
			RegisterTextStringForDisplay ("Rolling Demo", -50, -20, NORMAL_COLOUR);

		if (MainMenuChoice == STRAIGHT_RACE)
			RegisterTextStringForDisplay ("Straight Race", -50, 0, 16);
		else
			RegisterTextStringForDisplay ("Straight Race", -50, 0, NORMAL_COLOUR);

		if (MainMenuChoice == THE_WORKS)
			RegisterTextStringForDisplay ("The Works", -50, 20, 16);
		else
			RegisterTextStringForDisplay ("The Works", -50, 20, NORMAL_COLOUR);

		if (MainMenuChoice == VIEW_THE_TRACKS)
			RegisterTextStringForDisplay ("View the tracks", -50, 40, 16);
		else
			RegisterTextStringForDisplay ("View the tracks", -50, 40, NORMAL_COLOUR);

		//RegisterTextStringForDisplay ("Start and Select to quit", -50, 60, NORMAL_COLOUR);

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		
		FntFlush(-1);

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopForTheMainMenu\n\n");
		//	}

		DealWithControllerForMainMenu();
		}
}





void DealWithControllerForMainMenu (void)
{
	long pad;
	int mainMenuFramePause = 12;

	pad = PadRead();

	FramesSinceLastUserSelection++;

	if (pad & PADstart && pad & PADselect)
		{
		printf("\n\n\n\nExiting Now\n\n\n\n");
		CleanupAndExit();
		}

	if (FramesSinceLastUserSelection > mainMenuFramePause)
		{
		if (pad & PADLup)
			{
			if (MainMenuChoice == 0)
				{
				MainMenuChoice = NUMBER_MAIN_MENU_CHOICES-1;
				}
			else
				{
				MainMenuChoice--;
				}
			FramesSinceLastUserSelection = 0;
			}
		else if (pad & PADLdown)
			{
			if (MainMenuChoice == NUMBER_MAIN_MENU_CHOICES-1)
				{
				MainMenuChoice = 0;
				}
			else
				{
				MainMenuChoice++;
				}
			FramesSinceLastUserSelection = 0;
			}
		
			// chooose one main menu option
		if (pad & PADRdown)
			{
			switch(MainMenuChoice)
				{
				case ROLLING_DEMO:
					SetANewGameState(WAITING_FOR_THE_RACE);
					break;
				case FREE_PRACTICE:
				case STRAIGHT_RACE:
				case THE_WORKS:
				case VIEW_THE_TRACKS:
					SetANewGameState(SETTING_UP_THE_RACE);
					break;
				default:
					assert(FALSE);
				}
			}
		}
}

 


  
	// gui loop: handles sound and text strings

void GuiLoopForSettingUpTheRace (void)
{
	int bufferIndex;			  // buffer index
	char stringBuffer[128];
	int y;

	assert(MainMenuChoice == FREE_PRACTICE
			|| MainMenuChoice == STRAIGHT_RACE
			|| MainMenuChoice == THE_WORKS
			|| MainMenuChoice == VIEW_THE_TRACKS);

	assert(SettingUpTheRaceSubSelectionMode == ALL_THREE
			|| SettingUpTheRaceSubSelectionMode == ONE_OF_THREE);

	bufferIndex = GsGetActiveBuff();
	while (SettingUpTheRaceQuitFlag == FALSE)
		{		
		//DealWithControllerForSettingUpTheRace();

		y = -40;

		sprintf(stringBuffer, "Track %d\n", SettingUpTheRaceTrackNumber);
		if (SettingUpTheRaceSelectionMode == CHOOSE_TRACK)
			RegisterTextStringForDisplay (stringBuffer, -80, y, 16);
		else
			RegisterTextStringForDisplay (stringBuffer, -80, y, NORMAL_COLOUR);
		y += 20;

		if (SettingUpTheRaceSubSelectionMode == ALL_THREE)
			{
			sprintf(stringBuffer, "number of other drivers %d\n", SettingUpTheRaceNumberOfDrivers);
			if (SettingUpTheRaceSelectionMode == CHOOSE_NUMBER_DRIVERS)
				RegisterTextStringForDisplay (stringBuffer, -80, y, 16);
			else
				RegisterTextStringForDisplay (stringBuffer, -80, y, NORMAL_COLOUR);
			y += 20;

			sprintf(stringBuffer, "number of laps %d\n", SettingUpTheRaceNumberOfLaps);
			if (SettingUpTheRaceSelectionMode == CHOOSE_NUMBER_LAPS)
				RegisterTextStringForDisplay (stringBuffer, -80, y, 16);
			else
				RegisterTextStringForDisplay (stringBuffer, -80, y, NORMAL_COLOUR);
			y += 20;
			}
		else
			{
			assert(SettingUpTheRaceSubSelectionMode == ONE_OF_THREE);
			}

		switch(MainMenuChoice)
			{
			case STRAIGHT_RACE:
			case THE_WORKS:
				if (SettingUpTheRaceSelectionMode == GO_AHEAD_AND_RACE)
					RegisterTextStringForDisplay ("Go ahead and race", -80, y, 16);
				else
					RegisterTextStringForDisplay ("Go ahead and race", -80, y, NORMAL_COLOUR);
				break;
			case FREE_PRACTICE:
				if (SettingUpTheRaceSelectionMode == GO_AHEAD_AND_RACE)
					RegisterTextStringForDisplay ("Start free practice", -80, y, 16);
				else
					RegisterTextStringForDisplay ("Start free practice", -80, y, NORMAL_COLOUR);
				break;
			case VIEW_THE_TRACKS:
				if (SettingUpTheRaceSelectionMode == GO_AHEAD_AND_RACE)
					RegisterTextStringForDisplay ("View track", -80, y, 16);
				else
					RegisterTextStringForDisplay ("View track", -80, y, NORMAL_COLOUR);
				break;
			default:
				assert(FALSE);
			}
		y += 20;

		if (SettingUpTheRaceSelectionMode == RETURN_TO_MAIN)
			RegisterTextStringForDisplay ("Return to main", -80, y, 16);
		else
			RegisterTextStringForDisplay ("Return to main", -80, y, NORMAL_COLOUR);
		y += 20;

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);
			
		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		
		FntFlush(-1);

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopForSettingUpTheRace\n\n");
		//	}

		DealWithControllerForSettingUpTheRace();
		}
}


	
 

void DealWithControllerForSettingUpTheRace (void)
{
	long pad;
	int settingUpRaceFrameDelay = 12;

	pad = PadRead();

	FramesSinceLastUserSelection++;

	if (FramesSinceLastUserSelection > settingUpRaceFrameDelay)
		{
		if (pad & PADstart && pad & PADselect)
			{
			SetANewGameState(MAIN_MENU);
			}

		switch(SettingUpTheRaceSubSelectionMode)		   // which ones should user choose
			{
			case ONE_OF_THREE:			 // some sub-modes not available -> skip them
				if (pad & PADLdown)
					{
					switch(SettingUpTheRaceSelectionMode)
						{
						case CHOOSE_TRACK:
							SettingUpTheRaceSelectionMode = GO_AHEAD_AND_RACE;
							break;
						case GO_AHEAD_AND_RACE:
							SettingUpTheRaceSelectionMode = RETURN_TO_MAIN;
							break;
						case RETURN_TO_MAIN:
							SettingUpTheRaceSelectionMode = CHOOSE_TRACK;
							break;
						default:
							assert(FALSE);
						}
					FramesSinceLastUserSelection = 0;
					}
				else if (pad & PADLup)
					{
					switch(SettingUpTheRaceSelectionMode)
						{
						case CHOOSE_TRACK:
							SettingUpTheRaceSelectionMode = RETURN_TO_MAIN;
							break;
						case GO_AHEAD_AND_RACE:
							SettingUpTheRaceSelectionMode = CHOOSE_TRACK;
							break;
						case RETURN_TO_MAIN:
							SettingUpTheRaceSelectionMode = GO_AHEAD_AND_RACE;
							break;
						default:
							assert(FALSE);
						}
					FramesSinceLastUserSelection = 0;
					}
				break;
			case ALL_THREE:
				if (pad & PADLdown)
					{
					if (SettingUpTheRaceSelectionMode == NUMBER_SETTING_UP_RACE_CHOICES-1)
						SettingUpTheRaceSelectionMode = 0;
					else
						SettingUpTheRaceSelectionMode++;
					FramesSinceLastUserSelection = 0;
					}
				else if (pad & PADLup)
					{
					if (SettingUpTheRaceSelectionMode == 0)
						SettingUpTheRaceSelectionMode = NUMBER_SETTING_UP_RACE_CHOICES-1;
					else
						SettingUpTheRaceSelectionMode--;
					FramesSinceLastUserSelection = 0;
					}
				break;
			default:
				assert(FALSE);
			}

		switch (SettingUpTheRaceSelectionMode)
			{
			case CHOOSE_TRACK:
				if (pad & PADRright)
					{
					if (SettingUpTheRaceTrackNumber == NUMBER_SET_TRACKS)
						SettingUpTheRaceTrackNumber = 1;
					else
						SettingUpTheRaceTrackNumber++;
					FramesSinceLastUserSelection = 0;
					}
				else if (pad & PADRleft)
					{
					if (SettingUpTheRaceTrackNumber == 1)
						SettingUpTheRaceTrackNumber = NUMBER_SET_TRACKS;
					else
						SettingUpTheRaceTrackNumber--;
					FramesSinceLastUserSelection = 0;
					}
				break;
			case CHOOSE_NUMBER_DRIVERS:
				if (pad & PADRright)
					{											 // room for player also
					if (SettingUpTheRaceNumberOfDrivers == MAX_SHIPS_PER_RACE-1)
						SettingUpTheRaceNumberOfDrivers = 1;
					else
						SettingUpTheRaceNumberOfDrivers++;
					FramesSinceLastUserSelection = 0;
					}
				else if (pad & PADRleft)
					{
					if (SettingUpTheRaceNumberOfDrivers == 1)	 // room for player also
						SettingUpTheRaceNumberOfDrivers = MAX_SHIPS_PER_RACE-1;
					else
						SettingUpTheRaceNumberOfDrivers--;
					FramesSinceLastUserSelection = 0;
					}
				break;
			case CHOOSE_NUMBER_LAPS:
				if (pad & PADRright)
					{
					if (SettingUpTheRaceNumberOfLaps == MAX_LAPS_IN_RACE)
						SettingUpTheRaceNumberOfLaps = 1;
					else
						SettingUpTheRaceNumberOfLaps++;
					FramesSinceLastUserSelection = 0;
					}
				else if (pad & PADRleft)
					{
					if (SettingUpTheRaceNumberOfLaps == 1)
						SettingUpTheRaceNumberOfLaps = MAX_LAPS_IN_RACE;
					else
						SettingUpTheRaceNumberOfLaps--;
					FramesSinceLastUserSelection = 0;
					}
				break;
			case GO_AHEAD_AND_RACE:
				if ((pad & PADstart) || (pad & PADRdown))
					{
					switch(MainMenuChoice)
						{
						case FREE_PRACTICE:
						case STRAIGHT_RACE:
						case THE_WORKS:
							SetANewGameState(WAITING_FOR_THE_RACE);
							break;
						case VIEW_THE_TRACKS:
							SetANewGameState(VIEWING_THE_TRACKS);
							break;
						default:
							assert(FALSE);
						}
					}
				break;
			case RETURN_TO_MAIN:
				if ((pad & PADstart) || (pad & PADRdown))
					{
					SetANewGameState(MAIN_MENU);
					}
				break;
			default:
				assert(FALSE);
			}
		}
}






	// verify user selection data
	// copy into actual race data

void PrepareTheRaceFromSettingUpData (void)
{
	int i;

	switch(SettingUpTheRaceSubSelectionMode)
		{
		case ONE_OF_THREE:
			assert(MainMenuChoice == FREE_PRACTICE);

			SettingUpTheRaceNumberOfDrivers = 1;		// just the player

			SettingUpTheRaceNumberOfLaps = ENDLESS_LAPS;
			break;
		case ALL_THREE:
			assert(MainMenuChoice == STRAIGHT_RACE
					|| MainMenuChoice == THE_WORKS);

			assert(SettingUpTheRaceNumberOfDrivers >= 0);
			assert(SettingUpTheRaceNumberOfDrivers < MAX_SHIPS_PER_RACE);

			assert(SettingUpTheRaceNumberOfLaps >= 0);
			assert(SettingUpTheRaceNumberOfLaps < MAX_LAPS_IN_RACE);
			assert(SettingUpTheRaceNumberOfLaps < MAX_LAPS_PER_OBJECT+1);
			break;
		default:
			assert(FALSE);
		}	

		// now copy user selection data into real race data
	FrameWhenRaceStarts	= frameNumber + FRAME_DELAY_BEFORE_RACE;		

	assert(SettingUpTheRaceTrackNumber >= 1);
	assert(SettingUpTheRaceTrackNumber <= NUMBER_SET_TRACKS);
	ChosenSetTrack = SettingUpTheRaceTrackNumber-1;
		// don't set #drivers till later (below)
	NumberLapsInRace = SettingUpTheRaceNumberOfLaps;


		// build the list of ships in race
	switch(MainMenuChoice)
		{
		case FREE_PRACTICE:
			assert(SettingUpTheRaceNumberOfDrivers == 1);
			NumberOfShipsInRace = 1;

			RacingShips[0] = &PlayersShip;
			PlayersShip.specialMovementFlag = HUMAN_PLAYER;

			RaceOnFlag = FALSE;
			break;
		case STRAIGHT_RACE:
		case THE_WORKS:
			NumberOfShipsInRace = SettingUpTheRaceNumberOfDrivers+1;		// player also
			assert(NumberOfShipsInRace < MAX_SHIPS_PER_RACE);

			RacingShips[0] = &PlayersShip;
			PlayersShip.specialMovementFlag = HUMAN_PLAYER;

			for (i = 0;	i < NumberOfShipsInRace-1; i++)	
				{
				RacingShips[i+1] = &OtherShips[i];
				OtherShips[i].specialMovementFlag = AI_FLIER_3;
				}
			RaceOnFlag = TRUE;
			break;
		default:
			assert(FALSE);
		}
	TheViewShip = &PlayersShip;



		// HERE: tell the user we are building the tunnel
		// bring up nice TIM, print message near bottom of screen
	printf("\n\nCreating the tunnel now\n\n");

		// build the specified tunnel
	SortTunnelToSetTrack(ChosenSetTrack);

 

	PrepareShipsForRace();	
}





	// set actual race data randomly
	// including roving camera views, flip between ships, etc

void PrepareTheRaceForRollingDemo (void)
{
	int viewShipID;
	int i;

	assert(FALSE);		// under developed

		// choose track and number of drivers randomly
		// rolling demo will continue until 
	SettingUpTheRaceTrackNumber = rand() % NUMBER_SET_TRACKS;
	SettingUpTheRaceNumberOfDrivers = rand() % MAX_SHIPS_PER_RACE;
	SettingUpTheRaceNumberOfLaps = ENDLESS_LAPS;

	ChosenSetTrack = SettingUpTheRaceTrackNumber;
	NumberOfShipsInRace = SettingUpTheRaceNumberOfDrivers;
	NumberLapsInRace = SettingUpTheRaceNumberOfLaps;

		// player not participating
	PlayersShip.alive = FALSE;

		// build list of ships in the race
	for (i = 0; i < NumberOfShipsInRace; i++)
		{
		RacingShips[i] = &OtherShips[i];
		}

		// set view ship randomly
	viewShipID = rand() % NumberOfShipsInRace;
	TheViewShip = &OtherShips[viewShipID];

		// when race begins
	FrameWhenRaceStarts = frameNumber + 120;



	PrepareShipsForRace();
}




void RestartRollingDemo (void)
{

	assert(FALSE);

#if 0
	PrepareTheRaceForRollingDemo();
	assert(FALSE);	// call below destroys all polygons ie tunnel
	InitialiseObjects();	   
#endif
}



	// set up the ships for racing
	// should be assigning ship types for movement data
void PrepareShipsForRace (void)
{
	ObjectHandler *ship;
	VECTOR offsetVector;
	MATRIX offsetMatrix;
	int i, j;

	setVECTOR( &offsetVector, 0, 0, TunnelSectionLength/2);
	InitMatrix( &offsetMatrix);

	for (i = 0; i < NumberOfShipsInRace; i++)
		{
		ship = RacingShips[i];

		ship->alive = TRUE;
		ship->racingFlag = TRUE;

		PutObjectInTunnel(ship, i, &offsetVector, &offsetMatrix);

		ship->placeInRace = NumberOfShipsInRace - i;   // place: 1 to N
		InitialPositions[i] = ship->placeInRace;

		ship->furthestSection = ship->tunnelSection;
		ship->furthestLap = 0;
		ship->advancedFlag = FALSE;

		ship->framesWhenLapsEnd[0] = FrameWhenRaceStarts;
		for (j = 1; j < MAX_LAPS_PER_OBJECT; j++)
			{
			ship->framesWhenLapsEnd[j] = 0;
			}
		ship->bestLapTime = 0;

			// blaggery for ai_flier_3
		ship->pad0 = -1;
		ship->pad1 = 32;
		ship->pad2 = i;

			// set movement parameters
		ship->accelerationSpeed = 32;
		ship->maximumSpeed = 1024;
		setVECTOR( &ship->rotationPower, ONE/1024, ONE/1024, ONE/1024);
		ship->movementMomentumFlag = TRUE;
		ship->movementFrictionCoefficient = 64;
		ship->rotationMomentumFlag = TRUE;
		ship->rotationFrictionCoefficient = 512;
		ship->collisionRadius = 125;	  
			#if 0  
				// CHANGE THESE BELOW LATER   
			#endif 
		ship->tunnelCollisionTreatmentFlag = DETECT_AND_HANDLE_COLLISIONS;
		ship->shipCollisionTreatmentFlag = IGNORE_COLLISIONS;
		}


		// update tunnel visually
	BringAliveLocalTunnelAroundShip(TheViewShip);

	TheView.super = &TheViewShip->coord;
	TheView.vrx = 0;
	TheView.vry = 0;
	TheView.vrz = 0;

	TheView.vpx = 0;
	TheView.vpy = 0;
	TheView.vpz = 220;
	TheView.rz = 0;

	ProjectionDistance = 192;
	GsSetProjection(ProjectionDistance);
	//printf("PrepareShipsForRace: TheView.super: %d\n", TheView.super);
	//printf("&PlayersShip.coord: %d\n", &PlayersShip.coord);

	PrintMainRaceData();
}

 


void PrepareTheTrackForViewing (void)
{
	assert(SettingUpTheRaceTrackNumber >= 1);
	assert(SettingUpTheRaceTrackNumber <= NUMBER_SET_TRACKS);
	ChosenSetTrack = SettingUpTheRaceTrackNumber-1;


		// HERE: tell the user we are building the tunnel
		// bring up nice TIM, print message near bottom of screen
	printf("\n\nCreating the chosen track now\n\n");

		// build the specified tunnel
	SortTunnelToSetTrack(ChosenSetTrack);

		// sort the viewer
	ResetTrackViewingData();
	SetUpTheViewer();
}






	// NOTE: completely non-interactive, so we set limit 
	// to prevent it going on too long

void GuiLoopWaitingForTheRaceToStart (void)
{
	int bufferIndex;			  
	int i;
	u_short zValue;
	ObjectHandler* object;
	MATRIX tmpls;
	int frameWhenWaitingBegins;
	int maxFramesOfWaiting = 180;
	int timeLeft, totalTimeLeft; 

	frameWhenWaitingBegins = frameNumber;
	bufferIndex = GsGetActiveBuff();
	while (WaitingForTheRaceQuitFlag == FALSE)
		{					
		HandleSound();

		totalTimeLeft = FrameWhenRaceStarts - frameWhenWaitingBegins;
		timeLeft = frameNumber - frameWhenWaitingBegins;
		if (timeLeft < (totalTimeLeft / 3) )
			{
			RegisterTextStringForDisplay ("Ready", 
							-40, -30, NORMAL_COLOUR);
			}
		else if (timeLeft < ((2 * totalTimeLeft) / 3)	)
			{ 
			RegisterTextStringForDisplay ("Steady", 
							-44, 0, NORMAL_COLOUR);
			}
		else
			{
			RegisterTextStringForDisplay ("Go", 
							-28, 30, NORMAL_COLOUR);
			}

		//HandleAllObjects();	 // NO NEED: nothing moves

		//printf("00007\n");
		GsSetRefView2( &TheView);
		//printf("00008\n");

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

	  		// very few large objects
			// no need to clip yet
		for (i = 0; i < MAX_OBJECTS; i++)
			{
			if (ObjectArray[i] != NULL)
				if (ObjectArray[i]->alive == TRUE)
					{
					object = ObjectArray[i];

					if (object->displayFlag == TMD)
						{
						GsGetLs(&(object->coord), &tmpls);	   // local to screen matrix
								   
						GsSetLightMatrix(&tmpls);
									
						GsSetLsMatrix(&tmpls);
									
						GsSortObject4( &(object->handler), 
								&Wot[bufferIndex], 
									3, getScratchAddr(0));
						}
					else if (object->displayFlag == SPRITE)
							  // speed-up here: use a fast flag
						{	  // some can use GsSortFastSprite
						zValue = SortSpriteObjectPosition(object);
						GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
						}
					}
			}

		DrawTheTunnel( &Wot[bufferIndex]);
	
		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
					
		FntPrint("hsync: %d\n", hsync);
		
		FntFlush(-1);

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopWaitingForTheRaceToStart\n\n");
	   	//  }

		assert(frameNumber - frameWhenWaitingBegins
				<= maxFramesOfWaiting);
		if (frameNumber == FrameWhenRaceStarts)
			{
			printf("\n\nRace Starting\n\n\n");
			SetANewGameState(DURING_THE_RACE);
			}
		}
}








/**
	(InitialPositions)
	FinalPositions, esp. top 3

	track: id, name, total length

	race data: number ships, number laps

	for each ship:
		bestLapTime
		average speed

	fastest lap of race	(new lap record?)
**/

void GuiLoopForPrintingTheRaceResults (void)
{
	int bufferIndex;			  // buffer index
	char stringBuffer[128];
	long pad;

	bufferIndex = GsGetActiveBuff();
	while (AfterTheRaceQuitFlag == FALSE)
		{		
		sprintf(stringBuffer, "Not printing race results yet\n");
		RegisterTextStringForDisplay (stringBuffer, -120, 0, NORMAL_COLOUR);

		sprintf(stringBuffer, "Press any key to return to main\n");
		RegisterTextStringForDisplay (stringBuffer, -120, 20, NORMAL_COLOUR);

		pad = PadRead();

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		hsync = VSync(0);
		if (hsync > maxHsync)
			maxHsync = hsync;
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		
		FntFlush(-1);

		if (pad != 0)
			{
			Pause(30);
			SetANewGameState(MAIN_MENU);
			}

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopForPrintingTheRaceResults\n\n");
		//	}
		}
}







void GuiLoopDuringTheRace (void)
{
	int bufferIndex;			  
	int i;
	u_short zValue;
	ObjectHandler* object;
	MATRIX tmpls;
	char string1[32], string2[32];

	// NOTE: really use two flags here
	// DuringTheRaceQuitFlag and EndRaceNowFlag

	bufferIndex = GsGetActiveBuff();
	while (DuringTheRaceQuitFlag == FALSE)
		{		
		FntPrint("frame: %d\n", frameNumber);

		FntPrint("VP: %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);

		FntPrint("proj: %d\n", ProjectionDistance);
	
			
		FntPrint("section %d\n", PlayersShip.tunnelSection);
		FntPrint("furth sect %d\n", PlayersShip.furthestSection);
		FntPrint("furth lap %d\n", PlayersShip.furthestLap);			

		//DealWithControllerPad();

		HandleTimer();

		HandleSound();

		HandleAllObjects();

		UpdateTheTunnel();

		switch(RaceOnFlag)
			{
			case TRUE:
				HandleRacePositions();
				sprintf(string1, "Position %d", PlayersShip.placeInRace);
				RegisterTextStringForDisplay (string1, -50, -60, 
						NORMAL_COLOUR - (16 << 5));
				sprintf(string2, "speed %d", abs(PlayersShip.velocity.vz) );
				RegisterTextStringForDisplay (string2, -50, -40, 
						NORMAL_COLOUR - (16 << 5));
				break;
			case FALSE:		// do nowt
				break;
			default:
				assert(FALSE);
			}
				

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

	  		// very few large objects
			// no need to clip yet
		for (i = 0; i < MAX_OBJECTS; i++)
			{
			if (ObjectArray[i] != NULL)
				if (ObjectArray[i]->alive == TRUE)
					{
					object = ObjectArray[i];

					if (object->displayFlag == TMD)
						{
						GsGetLs(&(object->coord), &tmpls);	   // local to screen matrix
								   
						GsSetLightMatrix(&tmpls);
									
						GsSetLsMatrix(&tmpls);
									
						GsSortObject4( &(object->handler), 
								&Wot[bufferIndex], 
									3, getScratchAddr(0));
						}
					else if (object->displayFlag == SPRITE)	  
								// speed-up here: use a fast flag
						{	    // some can use GsSortFastSprite
						zValue = SortSpriteObjectPosition(object);
						GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
						}
					}
			}

		DrawTheTunnel( &Wot[bufferIndex]);

		//DrawSync(0);		// will make sure everything gets drawn
							// will also drop frames if draw time long
	
		hsync = VSync(0);
		if (hsync > maxHsync)
			maxHsync = hsync;
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		FntPrint("hsync: %d\n", hsync);
		FntPrint("maxHsync: %d\n", maxHsync);
		if (frameNumber == 60)
			maxHsync = hsync;
		else if (maxHsync > 1500)		// pause held down
			{
			printf("massive hsync: %d\n", maxHsync);		
			maxHsync = 0;
			}
		
		FntFlush(-1);

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopDuringTheRace\n\n");
		//	}

		if (EndRaceNowFlag == TRUE)
			{
			SetANewGameState(MAIN_MENU);
			}
		}
}






	// HERE: bring up background TIM, shimmer

void ClosingSequenceLoop (void)
{
	int bufferIndex;			  
	//int i;
	//u_short zValue;
	//ObjectHandler* object;
	//MATRIX tmpls;
	int closingSequenceStartFrame;
	int closingSequenceDuration = 120;

	closingSequenceStartFrame = frameNumber;
	bufferIndex = GsGetActiveBuff();
	for (;;)
		{		
		//RegisterTextStringForDisplay ("display test", -50, 80, NORMAL_COLOUR - (16 << 5));
		
		HandleSound();

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);
	
		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		
		FntFlush(-1);

		frameNumber++;

		if (frameNumber % 10 == 0)
			{
			printf("In ClosingSequenceLoop\n\n");
			}

		if (frameNumber - closingSequenceStartFrame > closingSequenceDuration)
			{
			printf("End of closing sequence\n");
			CleanupAndExit();
			}
		}
}






	// should be in text_str module really
#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 8

void GuiLoopForPrintingTextStrings (int numberStrings, char *strings[])
{
	int bufferIndex;			  // buffer index
	char stringBuffer[128];
	int stringLength;
	int x, y;
	int i, j;

	assert(numberStrings > 0);
	assert(numberStrings < MAX_TEXT_STRINGS);
	for (i = 0; i < numberStrings; i++)
		{
		assert(strings[i] != NULL);
		assert(strlen(strings[i]) > 0);
		}

	bufferIndex = GsGetActiveBuff();
	y = -numberStrings * CHARACTER_HEIGHT / 2;
	for (i = 0; i < 2; i++)		// two frames only
		{		
		for (j = 0; j < numberStrings; j++)
			{	 
			stringLength = strlen(strings[j]);
			x = -stringLength * CHARACTER_WIDTH / 2;
			RegisterTextStringForDisplay (stringBuffer, x, y, NORMAL_COLOUR);
			y += CHARACTER_HEIGHT * 2;
			} 

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		
		FntFlush(-1);
		}
}




void PrintMainRaceData (void)
{
	int i;

	printf("\n\n\nHere is the race data\n\n\n\n");

	printf("FrameWhenRaceStarts : %d\n", FrameWhenRaceStarts);
	printf("NumberLapsInRace : %d\n", NumberLapsInRace);
	printf("NumberOfShipsInRace : %d\n", NumberOfShipsInRace);	

	for (i = 0; i < MAX_SHIPS_PER_RACE; i++)
		{
		printf("addr of racing ship %d is %08x\n", i, (u_int) RacingShips[i]);
		}
	printf("addr of race view ship is %08x\n", (u_int) TheViewShip);
	printf("addr of players ship is %08x\n", (u_int) (&PlayersShip) );
	for (i = 0; i < MAX_OTHER_SHIPS; i++)
		{
		printf("addr of other ship %d is %08x\n", i, (u_int) (&OtherShips[i]) );
		}

	printf("ChosenSetTrack : %d\n", ChosenSetTrack);
																								
	for (i = 0; i < MAX_SHIPS_PER_RACE; i++)
		{
		printf("initial %d and final position %d for racing ship %d\n",
			InitialPositions[i], FinalPositions[i], i);
		}

	printf("EndRaceNowFlag : %d\n", EndRaceNowFlag);
	printf("RaceOnFlag : %d\n", RaceOnFlag);
}







void GuiLoopForViewingTheTracks (void)
{
	int bufferIndex;			  

	// NOTE: really use two flags here
	// DuringTheRaceQuitFlag and EndRaceNowFlag

	bufferIndex = GsGetActiveBuff();
	while (ViewingTheTracksQuitFlag == FALSE)
		{		
		FntPrint("frame: %d\n", frameNumber);

		FntPrint("VP: %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);

		FntPrint("proj: %d\n", ProjectionDistance);
	
		HandleTimer();

		HandleSound();	

		HandleTrackViewer();

		GsSetRefView2( &TheView);

		UpdateTheTunnel();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		DrawTheTunnel( &Wot[bufferIndex]);
	
		hsync = VSync(0);
		if (hsync > maxHsync)
			maxHsync = hsync;
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		FntPrint("hsync: %d\n", hsync);
		FntPrint("maxHsync: %d\n", maxHsync);
		if (frameNumber == 60)
			maxHsync = hsync;
		else if (maxHsync > 1500)		// pause held down
			{
			printf("massive hsync: %d\n", maxHsync);		
			maxHsync = 0;
			}
		
		FntFlush(-1);

		frameNumber++;

		//if (frameNumber % 10 == 0)
		//	{
		//	printf("In GuiLoopForViewingTheTracks\n\n");
		//	}

		DealWithControlsForTrackViewer();
		}
}





void DealWithControlsForTrackViewer (void)
{
	long pad;
	int trackViewerFramePause = 15;

	pad = PadRead();

	FramesSinceLastUserSelection++;

	if (FramesSinceLastUserSelection > trackViewerFramePause)
		{
		if (pad & PADstart && pad & PADselect)
			{
			SetANewGameState(MAIN_MENU);
			}
		}

	if (pad & PADLup)
		{
		if (TrackViewerSpeed < TunnelSectionLength/2)
			TrackViewerSpeed += 25;
		}
	else if (pad & PADLdown)
		{
		if (TrackViewerSpeed > 25)
			TrackViewerSpeed -= 25;
		}

	if (pad & PADR1)
		{
		TrackViewerZangle += 2;
		}
	else if (pad & PADR2)
		{
	    TrackViewerZangle -= 2;
		}

	if (TrackViewerZangle >= 360)
		TrackViewerZangle -= 360;
	else if (TrackViewerZangle < 0)
		TrackViewerZangle += 360;
}

 
