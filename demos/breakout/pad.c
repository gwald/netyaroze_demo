//	Set of pad routines to present same interface
//	to pad as dtlh2500

// Better: just use PadInit and PadRead, and dump the nonsense arguments



#include <libps.h>



	// buffers
volatile u_char *bb0, *bb1;



	// prototypes
void PadInit (void);
	// NOTE: argument id is not used
u_long PadRead (void);






  
	// set up buffers
void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}


	// nice and comprehensible ...
u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}
	


