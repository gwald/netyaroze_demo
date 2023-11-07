/*
 *		"snd.c"
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
*/

#include <libps.h>

#define MVOL		127	
#define SVOL		127	


static short vab,seq;

void snd_set(u_long *seq_a, u_char *vab_h, u_char *vab_b);
			
void snd_play();	
void snd_stop();   


void snd_set(u_long *seq_a, u_char *vab_h, u_char *vab_b)
{
	SsSetMVol(0, 0);			

    SsUtSetReverbType(SS_REV_TYPE_STUDIO_C);		
	
	vab = SsVabTransfer(vab_h, vab_b, -1, 1);
	if (vab < 0) {
		printf("SsVabTransfer failed (%d)\n", vab);
		return;
	}

	seq = SsSeqOpen(seq_a, vab);
	if (seq < 0)
		printf("SsSeqOpen failed (%d)\n", seq);
}


void snd_play()
{
	SsSetMVol(MVOL, MVOL);			
	SsSeqSetVol(seq, SVOL, SVOL);	   
	SsUtReverbOn ();		  
	SsUtSetReverbDepth (80, 80);		

	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);  
}


void snd_stop()
{
	SsSeqStop(seq);			  
	VSync(0);
	VSync(0);
	SsSeqClose(seq);			
	SsVabClose(vab);		   
}



