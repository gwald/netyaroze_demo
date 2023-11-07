/************************************************************
 *															*
 *						sound.c								*
 *			   											    *															*
 *				LPGE     29/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


// sound module source





#include "sound.h"




// NOTE: incomplete: SHOULD ENSURE, in TurnOnSound,
// that that kind of sound NOT already playing
// eg naive mistake: if player presses R2, they fire, whenever they fire,
// play brief laser sound. Problem: player keeps R2 down, multiple
// invocations of laser sounds makes it sound awful


static short vab0;



static VoiceChannelHandler TheVoices[MAX_VOICES];


static int jasonTuneVoiceID;



void InitialiseSound (void)
{
#if SOUND_ON
	int i;

		// transfer VAB data
	vab0 = SsVabTransfer((u_char *)VH0_ADDRESS, (u_char *)VB0_ADDRESS, -1, 1);  
	if (vab0 < 0) 
		{
		printf("VAB open failed\n");
		exit(1);
		}

	SsSetMVol(127, 127);

		// background tune as VAB noise rather than using an SEQ:
		// just turn it on and never touch again
	jasonTuneVoiceID = SsUtKeyOn(vab0, 0, 0, 60, 0, 64, 64);

	for (i = 0; i < MAX_VOICES; i++)
		{
		TheVoices[i].value = 0;
		TheVoices[i].statusFlag = OFF;
		TheVoices[i].frameToStartOn = -1;
		TheVoices[i].duration = -1;
		TheVoices[i].program = -1;
		TheVoices[i].tone = -1;
		TheVoices[i].note = 60;		// default 60 (seems very common ... )
		TheVoices[i].leftVolume = 127;			  // default: max volume levels
		TheVoices[i].rightVolume = 127;
		}
#endif
}

	   


void HandleSound (void)
{
#if SOUND_ON
	int i;
	VoiceChannelHandler* data;

	for (i = 0; i < MAX_VOICES; i++)
		{
		data = &TheVoices[i];

		switch(data->statusFlag)
			{
			case OFF:
				break;
			case WAITING_TO_START:
				if (frameNumber == data->frameToStartOn)
					{
						// turn on
					data->value = SsUtKeyOn(vab0, data->program, data->tone, 
										data->note, 0, data->leftVolume, data->rightVolume);
					data->statusFlag = PLAYING;
					}	
				break;
			case PLAYING:
				if (frameNumber == data->frameToStartOn + data->duration)
					{
						// turn off
					SsUtKeyOff(data->value, vab0, data->program, data->tone, data->note);
					data->statusFlag = OFF;
					}
				break;
			default:
				assert(FALSE);
			}
		}
#endif
}

				 

void TurnOnSound (int whichProgram, int whichTone, int whichNote, int delayFromNow, int howLong,
									int leftVolume, int rightVolume)
{
#if SOUND_ON
	int soundIndex = -1;			// index into array TheVoices
	int i;
	VoiceChannelHandler* data;

	assert(whichProgram >= 0);
	assert(whichTone >= 0);

	for (i = 0; i < MAX_VOICES; i++)
		{
		if (TheVoices[i].statusFlag == OFF)	   // a free one
			{
			soundIndex = i;
			break;
			}
		}

	if (soundIndex == -1)
		{
		printf("no free sound voices\n");
		return;
		}

	data = &TheVoices[soundIndex];

	data->statusFlag = WAITING_TO_START;
	data->program = whichProgram;
	data->tone = whichTone;
	data->note = whichNote;
	data->frameToStartOn = frameNumber + delayFromNow;
	data->duration = howLong;
	data->leftVolume = leftVolume;
	data->rightVolume = rightVolume;
#endif
}


		


void CleanUpSound (void)
{	 
#if SOUND_ON
		// turn off background tune
	SsUtKeyOff(jasonTuneVoiceID, vab0, 0, 0, 60);

		// close entire VAB file
	SsVabClose(vab0);
#endif
}
