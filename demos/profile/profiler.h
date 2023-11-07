#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "tuto7.h"





#define MAX_FRAMES_OF_PROFILING 50

#define MAX_PROFILES_PER_FRAME 50




#define NOT_YET_BEGUN_PROFILING 0
#define DURING_PROFILING 1
#define AFTER_PROFILING 2



void InitProfiler (int frameToStartOn);
void MeasureTime (void);
void PrintProfilingData (void);