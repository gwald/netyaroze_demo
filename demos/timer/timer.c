#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "general.h"

#include "timer.h"

#include "main.h"




int GameFrameRateStrategy;		// flag: how fast to try and run it

int AutoTimerSortItOutFlag;

int PredictedFrameRate;

int HsyncRecord[MAX_FRAME_CHUNK];

int AverageHsync;







void InitialiseTimer (void)
{
	int i;

	if (MAX_FRAME_CHUNK % SORT_SELF_OUT_FREQUENCY != 0)
		{
		printf("Bad #define values\n");
		exit(1);
		}

	AverageHsync = 0;

	for (i = 0; i < MAX_FRAME_CHUNK; i++)
		HsyncRecord[i] = 0;

	AutoTimerSortItOutFlag = TRUE;

	PredictedFrameRate = 0;

	SetGameFrameRateStrategy(TRY_IN_ONE_FRAME);
}


void SetGameFrameRateStrategy (int flag)
{
	assert(flag >= TRY_IN_ONE_FRAME && flag <= TRY_IN_FIVE_FRAMES);

	GameFrameRateStrategy = flag;
}



int GetGameFrameRateStrategy (void)
{
	return GameFrameRateStrategy;
}



void GetStringDescribingFrameRateStrategy (char *string)
{
	switch(GameFrameRateStrategy)
		{
		case TRY_IN_ONE_FRAME:			 // fall-thru
		case TRY_IN_TWO_FRAMES:		 
		case TRY_IN_THREE_FRAMES:
		case TRY_IN_FOUR_FRAMES:
		case TRY_IN_FIVE_FRAMES:
			sprintf(string, "Try in %d frames", GameFrameRateStrategy);
			break;
		default:
			assert(FALSE);
		}
}


void HandleGameTiming (void)
{
	int hsync;
	int cell;
	int i;

	switch(GameFrameRateStrategy)
		{
		case TRY_IN_ONE_FRAME:
			DrawSync(0);
			hsync = VSync(0);
			break;
		case TRY_IN_TWO_FRAMES:		 // fall-thru
		case TRY_IN_THREE_FRAMES:
		case TRY_IN_FOUR_FRAMES:
		case TRY_IN_FIVE_FRAMES:
			DrawSync(0);
			hsync = VSync(0);
			for (i = 0; i < GameFrameRateStrategy-1; i++)
				VSync(0);
			break;
		default:
			assert(FALSE);
		}

	cell = frameNumber % MAX_FRAME_CHUNK;
	HsyncRecord[cell] = hsync;

	if (frameNumber % SORT_SELF_OUT_FREQUENCY == 0)
		{
		int i;

		AverageHsync = 0;
		#if 0
		for (i = 0; i < MAX_FRAME_CHUNK; i++)
			AverageHsync += HsyncRecord[i];
		AverageHsync /= MAX_FRAME_CHUNK;
		#endif
		if (cell == 0)
			{
            for (i = MAX_FRAME_CHUNK - SORT_SELF_OUT_FREQUENCY; i < MAX_FRAME_CHUNK; i++)
				AverageHsync += HsyncRecord[i];
			}
		else
			{
            for (i = cell - SORT_SELF_OUT_FREQUENCY; i < cell; i++)
				AverageHsync += HsyncRecord[i];
			}
		AverageHsync /= SORT_SELF_OUT_FREQUENCY;

		if (AutoTimerSortItOutFlag == TRUE)
			{
			PredictFrameRateByHSyncMeasure();
			//SetStrategyByHsyncMeasure();
			}
		}
}



   

	// set 'strategy' by hysncs
	// but: first 'strategy' always fastest

void SetStrategyByHsyncMeasure (void)
{
	int singleFrameLimit;
	int videoMode;
	int averageHsync;
	int numberOfFramesPerFrame;

	videoMode = GetVideoMode();

	switch(videoMode)
		{
		case MODE_NTSC:
			singleFrameLimit = SCAN_LINES_PER_FRAME_IN_NTSC;		
			break;
		case MODE_PAL:
			singleFrameLimit = SCAN_LINES_PER_FRAME_IN_PAL;		 
			break;
		default:
			assert(FALSE);
		}
	
		// count how many frames it takes, set strategy accordingly
	numberOfFramesPerFrame = 1;
	averageHsync = AverageHsync;
	while (averageHsync > singleFrameLimit)
		{
		numberOfFramesPerFrame++;
		averageHsync -=	singleFrameLimit;
		}

	SetGameFrameRateStrategy(numberOfFramesPerFrame);
}




	// just predict overall fps given hsync averages

void PredictFrameRateByHSyncMeasure (void)
{
	int singleFrameLimit;
	int videoMode;
	int averageHsync;
	int numberOfFramesPerFrame;

	videoMode = GetVideoMode();

	switch(videoMode)
		{
		case MODE_NTSC:
			singleFrameLimit = SCAN_LINES_PER_FRAME_IN_NTSC;	  
			break;
		case MODE_PAL:
			singleFrameLimit = SCAN_LINES_PER_FRAME_IN_PAL;
			break;
		default:
			assert(FALSE);
		}
	
		// count how many frames it takes, set strategy accordingly
	numberOfFramesPerFrame = 1;
	averageHsync = AverageHsync;
	while (averageHsync > singleFrameLimit)
		{
		numberOfFramesPerFrame++;
		averageHsync -=	singleFrameLimit;
		}

	assert(numberOfFramesPerFrame > 0);

	switch(videoMode)
		{
		case MODE_PAL:
			PredictedFrameRate = MAX_FRAME_RATE_UNDER_PAL / numberOfFramesPerFrame;
			break;
		case MODE_NTSC:
			PredictedFrameRate = MAX_FRAME_RATE_UNDER_NTSC / numberOfFramesPerFrame;
			break;
		default:
			assert(FALSE);
		}
}

 
