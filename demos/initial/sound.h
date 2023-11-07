// simple sound module



#include <libps.h>

#include "asssert.h"

#include "general.h"

#include "address.h"

#include "test1.h"			// frameNumber needed





#define SOUND_ON 1		// compile-time overall switch







	// value of status flag for each sound channel
#define OFF 0
#define WAITING_TO_START 1
#define PLAYING 2



typedef struct
{	
	int value;
	int statusFlag;
	int frameToStartOn;
	int duration;
	int program, tone, note;
	int leftVolume, rightVolume;
} VoiceChannelHandler;


#define MAX_VOICES 24		// psx standard




	// prototypes
void InitialiseSound (void);

void HandleSound (void);

void TurnOnSound (int whichProgram, int whichTone, int whichNote, 
						int delayFromNow, int howLong,
									int leftVolume, int rightVolume);

void CleanUpSound (void);

