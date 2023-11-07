// standard controller pad interface module



#include "pad.h"



	// buffers
volatile u_char *bb0, *bb1;




  
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
	


