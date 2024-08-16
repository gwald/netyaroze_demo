/*
 * LIBSOUND.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBSOUND_H
#define	__YAROZE_LIBSOUND_H

/*
 * This file contains info regarding sound library used in this sample.
 * ----------------------------------------------------------------------------
 */

#define	SOUND_VC_PROPELLER	0					// Sound of propeller
#define	SOUND_VC_COLLISION	1					// Sound of collision
#define	SOUND_VC_CRASH		2					// Sound of crashing
#define	SOUND_VC_GRAB		3					// Sound of grabbing enemy remains

#define	SOUND_VC_MAXNO		4					// Maximum number of voices

extern void  SoundInit(void);
extern void  SoundPlayMusic(void);
extern void  SoundStop(void);

extern void  SoundVoiceQInit(void);
extern void  SoundVoiceQPlay(short voice_index);
extern void  SoundVoiceQFlush(void);

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
