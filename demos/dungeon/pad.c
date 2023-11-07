#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <libps.h>
#include "pad.h"

extern	void	EnterCriticalSection( void );
extern	void	ExitCriticalSection( void );
extern	void	GetPadBuf( volatile u_char**, volatile u_char** );
/****************************************************************************

*****************************************************************************/
static	long	RepTime  = 20;
static	long	RepSpeed = 2;
static volatile u_char *bb0, *bb1;	/* パッドデータ（内部変数） */
/****************************************************************************

*****************************************************************************/
void	PadInit( long mode )
{
	GetPadBuf(&bb0, &bb1);
	ExitCriticalSection();
}

long	PadRead( int id )
{
	long	pad;

	pad = (~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
/*
	if( pad==PADselect ) {
		DrawSync(0);
		exit();
	}
*/
	return( pad );
}

void	PadStop(void)
{
}

long	PadRead2( short port )
{
	static long rept   = 0;
	static long padrec = 0;
	long	pad, trgdata, repdata;

	pad = PadRead( port );
	repdata = pad&padrec;
	trgdata = pad^repdata;

	if( repdata ) {
		rept++;
		if( rept>=RepTime ) {
			rept -= RepSpeed;
			trgdata |= repdata;
		}
	} else	rept = 0;
	padrec = pad;
	return( trgdata );
}
