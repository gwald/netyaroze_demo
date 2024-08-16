/*
 * LIBSOUND.C
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 *
 * This file contains data definitions and function library to handle
 * sound effects and music in this sample.  Care has been taken to make
 * this function library independent of the rest of the code in this sample
 *
 * This library can be easily updated to play any music and sound effect
 * with minimal changes.
 *
 * The structure SOUNDFILE provides acess to various VABs and SEQs.  Plaese
 * make an entry for each VAB/SEQ combination in this table.  In case there
 * is no SEQ to go with a VAB make the corresponding entry for that SEQ as
 * NULL.
 *
 * The structure VOICE provides access to various sound effects contained
 * in any of the VABs loaded through the SOUNDFILE table.  The entries in
 * this table are self explanatory.  For details please checkout the details
 * below.
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"
#include "libsound.h"
#include "libdata.h"

/*
 * Sound related variables and functions
 * ---------------------------------------------------------------------------
 */

#define	SOUND_MAIN_VOLUME	127					// Main volume level
#define SOUND_MUSIC_VOLUME	127					// Game music SEQ volume level
#define	SOUND_SFX_VOLUME	127					// Sound effects volume

/* Data structure to handle various sound files - VAB/SEQ files */

typedef struct __soundfile {					// Structure to handle various sound files
	unsigned long	*VHptr;						// Pointer to VAB header in main memory
	unsigned long	*VBptr;						// Pointer to VAB data in main memory
	unsigned long	*SEQptr;					// Pointer to SEQ data in main memory
	
	short			VabId;						// VabId assigned by PlayStation OS
	short			SeqId;						// Sequence Id assigned by PlayStation OS
} SOUNDFILE;

#define SOUND_MAXFILES		1					// Maximum number of sound files

static SOUNDFILE	Sounds[ SOUND_MAXFILES ] = {
	{ ADR_MUSIC_VH, ADR_MUSIC_VB, ADR_MUSIC_SEQ, -1, -1 },
	};

/*
 * Data structures to handle various sound effects
 * - "SoundIndex" provides an index into the sound file table above.
 *   This means that this particular voice sample is contained in
 *   the which VAB.
 *
 * - "VabId" is the id secured by the initialization routines for
 *   the VAB - indexed by SoundIndex.
 *
 * - "Program" is the index to the program in VAB where this voice belongs
 *   This value is predefined at the time of the VAB creation.
 *
 * - "Tone" is the index to the voice in a particular "Program".
 *   This value is predefined at the time of the VAB creation.
 *
 * - "Note" is the note being played in this voice.
 *   This value is predefined at the time of the VAB creation.
 * 
 * - "Fine" is the finer pitch value, usually set to 0.
 *   This value is predefined at the time of the VAB creation.
 *
 * - "LVolume" and "RVolume" is the maximum volume of this voice
 *   This value is predefined at the time of the VAB creation and
 *   is used along with the panning (to be implemented later)
 *   of the sound depending upon the origin of the voice in th world.
 */

typedef struct __voice {						// Index into the SOUNDFILE table above
	short	SoundIndex;							// Index into the sound file list
	short	VabId;								// Vab Id used here
	short	Program;							// Program from the above vab
	short	Tone;								// Tone for this voice
	short	Note;								// Pitch specification for this voice
	short	Fine;								// Fine pitch specification
	short	LVolume;							// Maximum left volume for the voice
	short	RVolume;							// Maximum right volume for the voice
} VOICE;

static VOICE		Voices[ SOUND_VC_MAXNO ] = {
	{ 0, -1, 0x0000, 0, 0x0018, 0x0000, 96, 96 },
	{ 0, -1, 0x0000, 1, 0x0019, 0x0000, 96, 96 },
	{ 0, -1, 0x0000, 2, 0x001A, 0x0000, 96, 96 },
	{ 0, -1, 0x0000, 3, 0x001B, 0x0000, 96, 96 },
	};

/*
 * Data structure to handle a voice table -
 * manages a FIFO queue for voices to be played
 */

typedef struct __voiceq {						// Queue structure to handle voices
	short	VoiceId;							// Voice Id used for this entry
	VOICE	*Voice;								// Voice used in this queue
} VOICEQ;

#define	SOUND_VQ_MAXNO		10					// Maximum length of FIFO queue for Voice IDs

static VOICEQ		VoiceQ[ SOUND_VQ_MAXNO ];	// FIFO queue for Voice IDs assigned
static short		VoiceQTop = -1;				// Top of the FIFO queue for Voice IDs

/* Sound data on memory playback preparation */

void SoundInit(void)
{
	short i;
	for (i = 0; i < SOUND_MAXFILES; i++) {
		/* VAB opening and transmission to sound buffer */
		if ((Sounds[i].VHptr != NULL) && (Sounds[i].VBptr != NULL)) {
			Sounds[i].VabId = SsVabTransfer(
									(unsigned char *)Sounds[i].VHptr,
									(unsigned char *)Sounds[i].VBptr,
									-1, 1);
			if (Sounds[i].VabId < 0)
				printf("SsVabTransfer(i = %d) failed (VabId = %d)\n",
									i, Sounds[i].VabId);
		}
		else {
			Sounds[i].VabId = -1;
		}
		
		/* SEQ opening */
		if (Sounds[i].SEQptr != NULL) {
			Sounds[i].SeqId = SsSeqOpen(
									Sounds[i].SEQptr,
									Sounds[i].VabId);
			if (Sounds[i].SeqId < 0)
				printf("SsSeqOpen(i = %d) failed (VabId = %d)\n",
									i, Sounds[i].SeqId);
		}
		else {
			Sounds[i].SeqId = -1;
		}
	}
	
	/* Initialize saucer voices */
	for (i = 0; i < SOUND_VC_MAXNO; i++) {
		Voices[i].VabId = Sounds[ Voices[i].SoundIndex ].VabId;
	}
	
	/* Initialize voice FIFO queue */
	SoundVoiceQInit();
	
	/* Set the main volume */
	SsSetMVol(SOUND_MAIN_VOLUME, SOUND_MAIN_VOLUME);
	return;
}

/* Sound playback start */

void SoundPlayMusic(void)
{
	/* Set Volume for the sequence */
	SsSeqSetVol(Sounds[0].SeqId, SOUND_MUSIC_VOLUME, SOUND_MUSIC_VOLUME);
	
	/* Start play back of game music */
	SsSeqPlay(Sounds[0].SeqId, SSPLAY_PLAY, SSPLAY_INFINITY);
	return;
}

/* Sound playback termination */

void SoundStop(void)
{
	short i;
	
	/* Release all acquired voices */
	SoundVoiceQFlush();
	
	/* Stop the game music */
	SsSeqStop(Sounds[0].SeqId);
	
	/* Wait for music to stop - give it some time, will you ? */
	VSync(0);
	VSync(0);
	
	for (i = 0; i < SOUND_MAXFILES; i++) {
		/* Release SEQuence ID */
		if (Sounds[i].SeqId != -1)	SsSeqClose(Sounds[i].SeqId);
		
		/* Release VAB ID */
		if (Sounds[i].VabId != -1)	SsVabClose(Sounds[i].VabId);
	}
	return;
}

/* Initialize the voice FIFO queue */

void SoundVoiceQInit(void)
{
	VoiceQTop = -1;
	return;
}

/* Enter the given voice in the voice queue by acquiring a new voice */

void SoundVoiceQPlay(short voice_index)
{
	VOICE *voice = &Voices[ voice_index ];
	short i, top = VoiceQTop+1;
	if (top >= SOUND_VQ_MAXNO) {
		/*
		 * Stop the oldest voice in the queue currently being
		 * used for sound effect.  Release this voice id and
		 * remove the voice from the queue.
		 */
		if (VoiceQ[0].VoiceId != -1) {
			VOICE *tmp = VoiceQ[0].Voice;
			SsUtKeyOff(VoiceQ[0].VoiceId, tmp->VabId, tmp->Program, tmp->Tone, tmp->Note);
		}
		for (i = 1 ; i < top ; i++) {
			VoiceQ[i-1].VoiceId	= VoiceQ[i].VoiceId;
			VoiceQ[i-1].Voice	= VoiceQ[i].Voice;
		}
		top --;
	}
	
	/*
	 * Play the sound effect for the given voice by requesting a
	 * new voice id and putting this voice in the voice queue.
	 */
	VoiceQ[top].Voice	= voice;
	VoiceQ[top].VoiceId	= SsUtKeyOn(voice->VabId,
									voice->Program,
									voice->Tone,
									voice->Note,
									voice->Fine,
									voice->LVolume,
									voice->RVolume);
	
	/* Set the new top of the queue */
	VoiceQTop = top;
	return;
}

/* Flush voice queue */

void SoundVoiceQFlush(void)
{
	short i;
	for (i = 0 ; i <= VoiceQTop ; i++) {
		if (VoiceQ[i].VoiceId != -1) {
			VOICE *tmp = VoiceQ[i].Voice;
			SsUtKeyOff(VoiceQ[i].VoiceId, tmp->VabId, tmp->Program, tmp->Tone, tmp->Note);
		}
	}
	VoiceQTop = -1;
	return;
}

/*
 * End of File
 * ----------------------------------------------------------------------------
 */
