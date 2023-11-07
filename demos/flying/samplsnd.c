/*
 *	sound: sound sample program
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 */




#include <libps.h>
#include "pad.h"

	// program termination flag
int	  end_flag;

short seq1;	 
short vab0;	
short tempo;
short def_tempo;

#define VH_NAME "\\DATA\\SOUND\\STD0.VH;1"
#define VB_NAME "\\DATA\\SOUND\\STD0.VB;1"
#define VH0_ADDR (unsigned char*)0x80140000L
#define VB0_ADDR (unsigned char*)0x80160000L
#define SEQ_ADDR (unsigned long*)0x80100000L

#define DRUM 127
#define AAHV 85
#define HANDC03 39
#define CLHIH06 42
#define HIHAT10 46
#define RBELL15 53


void
usage( void )
{
	printf( "==================================================\n");
	printf( "         [L2]                    [R2] \n");
	printf( "         [L1]                    [R1] \n");
	printf( "          Å™                      Å¢  \n");
	printf( "        Å©Å{Å®                  Å†Å{Åõ\n");
	printf( "          Å´   [SELECT] [START]   Å~  \n");
	printf( "==================================================\n");
	printf( " [SELECT] : Stop SEQ     [START] : Play/Pause SEQ\n" );
	printf( "     [L2] : SEQ Pan Left    [R2] : SEQ Pan Right\n" );
	printf( "     [L1] : Ritardando      [R1] : Accelerando\n" );
	printf( "    Åõ+Å™ : Bend Up           Å¢ : Hand Clap\n" );
	printf( "    Åõ+Å´ : Bend Down         Å~ : Cymbal\n" );
	printf( "    Åõ+Å® : SE Pan Right      Åõ : Aah\n" );
	printf( "    Åõ+Å© : SE Pan Left       Å† : Bell\n" );
	printf( "==================================================\n");
	printf( "         [SELECT]+[START] : return to CIP\n" );
	printf( "==================================================\n");
}




static u_long PadRead(long id);	
u_long padd;			
volatile u_char *bb0, *bb1;


static u_long PadRead(long id)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}


void
padToCallSnd (unsigned long padd)
{
	static unsigned long opadd;
	static short se_voice1;
	static short se_voice2;
	static short se_voice3;
	static short se_voice4;
	static short bend = 64;
	short lvol, rvol;

	if( (padd & PADstart) && !(opadd & PADstart) ) {
		if( SsIsEos( seq1, 0 ) == 0 ) {
			SsSetTempo( seq1, 0, def_tempo );
			SsSeqSetVol( seq1, 127, 127 );
			SsSeqPlay( seq1, SSPLAY_PLAY, 0);
		} else {
			SsSeqPause( seq1 );
		}
	} 

	if( padd & PADR1 ) {
		tempo++;
		if( tempo > 240 ) tempo = 240;
		SsSetTempo( seq1, 0, tempo );
	} 

	if( padd & PADL1 ) {
		tempo--;
		if( tempo < 40 ) tempo = 40;
		SsSetTempo( seq1, 0, tempo );
	} 

	if( (padd & PADRright) && !(opadd & PADRright) ) {
		se_voice1 = SsUtKeyOn( vab0, AAHV, 0, 60, 0, 127, 127 );
		bend = 64;
	} 
	if( !(padd & PADRright) && (opadd & PADRright) ) {
		SsUtKeyOff( se_voice1, vab0, AAHV, 0, 60 );
	} 

	if( (padd & PADRup) && !(opadd & PADRup) ) {
		se_voice2 = SsUtKeyOn( vab0, DRUM, 3, HANDC03, 0, 127, 127 );
	}
	if( !(padd & PADRup) && (opadd & PADRup) ) {
		SsUtKeyOff( se_voice2, vab0, DRUM, 3, HANDC03 );
	} 

	if( (padd & PADRdown) && !(opadd & PADRdown) ) {
		SsUtKeyOff( se_voice3, vab0, DRUM, 6, CLHIH06 );
		se_voice3 = SsUtKeyOn( vab0, DRUM, 10, HIHAT10, 0, 127, 127 );
	} 
	if( !(padd & PADRdown) && (opadd & PADRdown) ) {
		SsUtKeyOff( se_voice3, vab0, DRUM, 10, HIHAT10 );
		se_voice3 = SsUtKeyOn( vab0, DRUM, 6, CLHIH06, 0, 127, 127 );
	} 

	if( (padd & PADRleft) && !(opadd & PADRleft) ) {
		se_voice4 = SsUtKeyOn( vab0, DRUM, 15, RBELL15, 0, 127, 127 );
	} 
	if( !(padd & PADRleft) && (opadd & PADRleft) ) {
		SsUtKeyOff( se_voice4, vab0, DRUM, 15, RBELL15 );
	}

	if( padd & PADLup ) {
		bend++;
		if( bend > 127 ) bend = 127;
		SsUtPitchBend( se_voice1, vab0, AAHV, 60, bend );
	} 

	if( padd & PADLdown ) {
		bend--;
		if( bend < 0 ) bend = 0;
		SsUtPitchBend( se_voice1, vab0, AAHV, 60, bend );
	} 

	if( padd & PADLleft ) {
		SsUtGetVVol( se_voice1, &lvol, &rvol );
		lvol++;rvol--;
		if( rvol < 0 ) rvol = 0;
		if( lvol > 127 ) lvol = 127;
		SsUtSetVVol( se_voice1, lvol, rvol );
	} 

	if( padd & PADLright ) {
		SsUtGetVVol( se_voice1, &lvol, &rvol );
		lvol--;rvol++;
		if( rvol > 127 ) rvol = 127;
		if( lvol < 0 ) lvol = 0;
		SsUtSetVVol( se_voice1, lvol, rvol );
	} 

	if( padd & PADL2 ) {
		SsSeqGetVol( seq1, 0, &lvol, &rvol );
		lvol++; rvol--;
		if( rvol < 1 ) rvol = 1;
		if( lvol > 127 ) lvol = 127;
		if( rvol > lvol )
			SsSeqSetVol( seq1, lvol, 127 );
		else
			SsSeqSetVol( seq1, 127, rvol );
	} 

	if( padd & PADR2 ) {
		SsSeqGetVol( seq1, 0, &lvol, &rvol );
		lvol--;rvol++;
		if( rvol > 127 ) rvol = 127;
		if( lvol < 1 ) lvol = 1;
		if( rvol > lvol )
			SsSeqSetVol( seq1, lvol, 127 );
		else
			SsSeqSetVol( seq1, 127, rvol );
	} 

	if( (padd & PADselect) && !(opadd & PADselect) ) {
		SsSeqStop (seq1);
	} 

		// quit
	if( (padd & PADselect) && (padd & PADstart) ) {
		SsSeqStop (seq1);
		VSync(0); 
		end_flag = 1;
	} 
	opadd = padd;
}

main (void)
{
	unsigned long padd;
	int i, cnt;
	CdlFILE vhfile, vbfile;
	unsigned long tempo_raw;
	unsigned char *temptr;
	unsigned long *check_head;

	GetPadBuf(&bb0, &bb1);	
	padd=0;			
	

	ResetGraph(0);


	check_head = SEQ_ADDR;
	if( check_head[0] != 0x53455170 ) {
		printf("==============================\n");
		printf(" SEQ not loaded at 0x%lX\n", SEQ_ADDR );
		printf("==============================\n");
		return;
	}



	//	not needed ...  printf( "âπåπÉfÅ[É^ÇÇbÇcÇ©ÇÁì«Ç›çûÇ›íÜ" );
	SsUtSetReverbType( SS_REV_TYPE_STUDIO_C );
	SsUtReverbOn();


	for (i = 0; i < 10; i++) {	
		if (CdSearchFile(&vhfile, VH_NAME) != 0)
			break;		
		else
			printf("%s not found.\n", VH_NAME);
	}
	printf( "ÅÙ" );
	for (i = 0; i < 10; i++) {	
		CdReadFile(VH_NAME, (void*)VH0_ADDR, vhfile.size);

		
		while ((cnt = CdReadSync(1, 0)) > 0 )
			VSync(0);	

		if (cnt == 0)
			break;	
	}
	
	for (i = 0; i < 10; i++) {	
		if (CdSearchFile(&vbfile, VB_NAME) != 0)
			break;		
		else
			printf("%s not found.\n", VB_NAME);
	}
	printf( "ÅÙ" );
	for (i = 0; i < 10; i++) {	
		CdReadFile(VB_NAME, (void*)VB0_ADDR, vbfile.size);
		
		while ((cnt = CdReadSync(1, 0)) > 0 )
			VSync(0);	

		if (cnt == 0)
			break;	
	}
	printf( "ÅÙ" );


/* ----------------------------------------------------------------
 *	  Open & transfer VAB data
 * ---------------------------------------------------------------- */
	vab0 = SsVabTransfer( VH0_ADDR, VB0_ADDR, -1, 1);
	if( vab0 < 0 ) {
		printf( "VAB open failed(vab0 = %d)\n", vab0 );
		return;
	}
	printf( "äÆóπÅB\n" );

/* ----------------------------------------------------------------
 *	  Open SEQ/SEP data
 * ---------------------------------------------------------------- */
	seq1 = SsSeqOpen (SEQ_ADDR, vab0);
	temptr = (unsigned char*)SEQ_ADDR + 10L;
	tempo_raw = (unsigned long)temptr[0] << 16;
	tempo_raw += (unsigned long)temptr[1] << 8;
	tempo_raw += (unsigned long)temptr[2];

	if( tempo_raw > 0 )
		tempo = (short)(60000000L / tempo_raw);
	else
		tempo = 120;
	def_tempo = tempo;
	SsSetMVol (127, 127);
	SsUtSetReverbDepth( 40, 40 );
	usage();
	SsSeqSetVol( seq1, 127, 127 );
	SsSeqPlay( seq1, SSPLAY_PLAY, 0);
	while (!end_flag) {
		VSync (0);
		padd = PadRead (0);
		padToCallSnd (padd);
	}

/* ----------------------------------------------------------------
 *	  Close SEQ/SEP,VAB data
 * ---------------------------------------------------------------- */

	SsSeqClose (seq1);
	SsVabClose (vab0);

/* ----------------------------------------------------------------
 *	  Stop & Quit Sound System
 * ---------------------------------------------------------------- */

	return;
}

