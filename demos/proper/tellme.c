#include <libps.h>




void main (void)
{
	int whichMode;

	whichMode = GetVideoMode();

	//printf("mode is %d\n", whichMode);
	//printf("1 is PAL, 0 is NTSC\n");

	switch(whichMode)
		{
		case MODE_NTSC:
			printf("mode is NTSC\n");
			break;
		case MODE_PAL:
			printf("mode is PAL\n");
			break;
		default:
			printf("BAD value returned by GetVideoMode\n");
		}
}