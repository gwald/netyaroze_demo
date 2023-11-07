//--------------------------------------------------------------------------
// File: sound.c
// Author: George Bain
// Date: July 23, 1998
// Description: Sound related routines
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h"
#include "sound.h"

//--------------------------------------------------------------------------
// Function: PlaySFX()
// Description: Play the sound effect
// Parameters: voice_ptr sound_effect: address of sndfx variable
// Returns: none
// Notes: See Library Reference Manual
//--------------------------------------------------------------------------

void PlaySFX( voice_ptr sound_effect )
 {

	 SsUtKeyOn(sound_effect->vabid,
	       	   sound_effect->program,
			   sound_effect->tone,
			   sound_effect->note,
			   sound_effect->fine,
			   sound_effect->vol_l,
			   sound_effect->vol_r);
	
 }// end PlaySFX  


							 


//--------------------------------------------------------------------------
// Function: InitSFX()
// Description: Setup our sound effects structure
// Parameters: Lots...see Library Reference Manual
// Returns: none
// Notes: See Library Reference Manual
//--------------------------------------------------------------------------

void InitSFX( voice_ptr sound_effect, short vab_id, short program, 
              short tone, short note, short fine, short vol_l, short vol_r )
 {

	sound_effect->vabid   = vab_id;
	sound_effect->program = program;
	sound_effect->tone    = tone;
	sound_effect->note    = note;
	sound_effect->fine    = fine;
	sound_effect->vol_l   = vol_l;
	sound_effect->vol_r   = vol_r; 

 }// end InitSFX





//--------------------------------------------------------------------------
// Function: InitSound()
// Description: Init sound system. Load in vabs and seqs, set reverb and volume
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void InitSound( void )
 {  
    
 	// return vab_id (0-15), open VAB and transfer to sound buffer	
	vab_id = SsVabTransfer((u_char*)VH_ADDR, (u_char*)VB_ADDR,-1,1); 

	if( vab_id < 0 )
	  {
	     printf(" vab:%d", vab_id);
		 printf(" ERROR: VAB fail (LINE:%d)(FILE:%s)\n",
			      __LINE__,__FILE__);

       }    
	 
	// open SEQ, max of 32 SEQ's can be opened at the same time 
	seq_id = SsSeqOpen((u_long*)SEQ_ADDR,vab_id); 	

	if( seq_id < 0 )
	  {
	    printf(" seq:%d", seq_id);
	    printf(" ERROR: SEQ fail (LINE:%d)(FILE:%s)\n",
			     __LINE__,__FILE__);

       }
	
	// set reverb 
	SetupReverb();
	   
	// set main volume
	SsSetMVol(MAIN_VOL,MAIN_VOL);

	// set SEQ volume 
	SsSeqSetVol(seq_id,SEQ_VOL,SEQ_VOL);

 }// end InitSound





//--------------------------------------------------------------------------
// Function: SetupReverb()
// Description: Set reverb type, depth, delay and feedback
// Parameters: none
// Returns: none
// Notes: Look at the Library Reference Manual
//--------------------------------------------------------------------------

void SetupReverb( void )
 {

	short type;
	
	// set reverb type
	type = SsUtSetReverbType( SS_REV_TYPE_HALL );

    if( type < 0 )
	    printf(" ERROR: Reverb fail \n");

    // turn on reverb
	SsUtReverbOn();

    // wait awhile
	VSync(0);
	VSync(0);

    // set depth
	SsUtSetReverbDepth(127,127);

	// set delay
	SsUtSetReverbDelay(127);

    // set feedback
	SsUtSetReverbFeedback(127);

 }// SetupReverb





//--------------------------------------------------------------------------
// Function: PlayMusic()
// Description: Start playing the SEQ file.  Will play the SEQ 1 time only!
// Parameters: none
// Returns: none
// Notes: Look at the Library Reference Manual
//--------------------------------------------------------------------------

void PlayMusic( void )
 {	   
	// read in music and begin playing
	SsSeqPlay(seq_id, SSPLAY_PLAY, 1);		

 }// end PlaySound





//--------------------------------------------------------------------------
// Function: StopSound()
// Description: Close down the sound system
// Parameters: none
// Returns: none
// Notes: Look at the Library Reference Manual
//--------------------------------------------------------------------------

void StopSound( void )
 {

    // stop SEQ data reading
	SsSeqStop(seq_id);

    // wait awhile
	VSync(0);
	VSync(0);
	
	// Close VAB data
	SsVabClose(vab_id);

    // close SEQ data
	SsSeqClose(seq_id);

 }// end StopSound


//----------------------------------EOF-------------------------------------
