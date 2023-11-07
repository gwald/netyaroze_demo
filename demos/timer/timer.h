#ifndef TIMER_H_INCLUDED


	// length of time of self-monitoring
#define MAX_FRAME_CHUNK 60

#define SORT_SELF_OUT_FREQUENCY 10


	// assume no game takes more than five frames
#define TRY_IN_ONE_FRAME 1
#define TRY_IN_TWO_FRAMES 2
#define TRY_IN_THREE_FRAMES 3
#define TRY_IN_FOUR_FRAMES 4
#define TRY_IN_FIVE_FRAMES 5


extern int AutoTimerSortItOutFlag;

extern int PredictedFrameRate;



extern int AverageHsync;



	// approximate only
#define SCAN_LINES_PER_FRAME_IN_NTSC 262
#define SCAN_LINES_PER_FRAME_IN_PAL	287

#define MAX_FRAME_RATE_UNDER_PAL 50
#define MAX_FRAME_RATE_UNDER_NTSC 60



void InitialiseTimer (void);

void SetGameFrameRateStrategy (int flag);

int GetGameFrameRateStrategy (void);

void GetStringDescribingFrameRateStrategy (char *string);

void HandleGameTiming (void);

void SetStrategyByHsyncMeasure (void);

void PredictFrameRateByHSyncMeasure (void);




#define TIMER_H_INCLUDED 1

#endif
