#include "profiler.h"



static int InternalFrameCounter;
static int StartingFrame;
static int WhichTimeThisFrame;
static int NumberTimesPerFrame;

static int ProfilerStatus;
static int CountingStatus;

static int LocalTime;

static u_long ProfilingHistory[MAX_FRAMES_OF_PROFILING][MAX_PROFILES_PER_FRAME];







void InitProfiler (int frameToStartOn)
{
	StartingFrame = frameToStartOn;
	WhichTimeThisFrame = 0;
	ProfilerStatus = NOT_YET_BEGUN_PROFILING;
	InternalFrameCounter = 0;
	CountingStatus = -1;
	NumberTimesPerFrame = 0;

	//printf("\n\nProfiling will begin on frame %d\n", frameToStartOn);
	//printf("And will last for %d frames\n\n\n", MAX_FRAMES_OF_PROFILING);
}






	// turn into macro
void MeasureTime (void)
{
	if (InternalFrameCounter != frameNumber)	   // new frame
		{
		assert(InternalFrameCounter == frameNumber-1);

			// main mode update
		if (frameNumber == StartingFrame)
			{
			ProfilerStatus = DURING_PROFILING;
			CountingStatus = 1;
			} 
		else if (frameNumber == StartingFrame + MAX_FRAMES_OF_PROFILING)
			{
			ProfilerStatus = AFTER_PROFILING;
			PrintProfilingData();
			}

		if (ProfilerStatus == DURING_PROFILING && CountingStatus == 1)
			{
			if (frameNumber == StartingFrame+1)
				{
				CountingStatus = -1;
				NumberTimesPerFrame = LocalTime;
				}
			}

		InternalFrameCounter = frameNumber;
		LocalTime = 0;		// no times yet this frame
		}
	

	switch(ProfilerStatus)
		{
		case NOT_YET_BEGUN_PROFILING:	
			// just wait
			break;
		case DURING_PROFILING:
			ProfilingHistory[InternalFrameCounter-StartingFrame][LocalTime] = GetRCnt(1);
			LocalTime++;
			break;
		case AFTER_PROFILING:
			// do nowt
			break;
		default:
			{
			printf("bad status value %d\n", ProfilerStatus);
			exit(1);
			}
		}			
}
		 														   




void PrintProfilingData (void)
{
	int frame, time;
	u_long difference;
	u_long first, second;

	printf("\n\n\n\n-------------START OF PROFILING DATA--------------\n\n\n");
	printf("Profiling begun on frame %d, ended on frame %d\n\n",
		StartingFrame, StartingFrame + MAX_FRAMES_OF_PROFILING-1);
	printf("Profiled %d counts per frame\n\n", NumberTimesPerFrame);

	for (frame = 0; frame < MAX_FRAMES_OF_PROFILING; frame++)
		{
		printf("\n\n\nData on frame %d\n\n\n", StartingFrame + frame); 
		for (time = 1; time <= NumberTimesPerFrame; time++)
			{
			printf("count number %d was %u\n", time, 
				ProfilingHistory[frame][time-1]);
			}
		for (time = 1; time < NumberTimesPerFrame; time++)
			{
			first = ProfilingHistory[frame][time-1];
			second = ProfilingHistory[frame][time];
			if (second < first)		 // wrap-around
				second += 65536;
			difference = second - first;
			printf("time lag between count %d and %d was %u\n", 
					time, time+1, difference);
			}
		}
	printf("\n\n\n\n-------------END OF PROFILING DATA--------------\n\n\n");		
}