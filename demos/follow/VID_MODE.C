#include "vid_mode.h"







int DefaultVideoMode = MODE_NTSC;

int ScreenWidth;
int ScreenHeight;

int MaximumHSyncsPerSingleFrame;
int MaxFramesPerSecond;







void InitialiseVideoMode (void)
{
	assert(DefaultVideoMode == MODE_PAL
			|| DefaultVideoMode == MODE_NTSC);

	SetVideoMode(DefaultVideoMode);

	switch(DefaultVideoMode)
		{
		case MODE_PAL:
			ScreenWidth = 320;
			ScreenHeight = 256;
			MaximumHSyncsPerSingleFrame = 270;		// UNTESTED
			MaxFramesPerSecond = 50;
			break;
		case MODE_NTSC:
			ScreenWidth = 320;
			ScreenHeight = 240;
			MaximumHSyncsPerSingleFrame = 240;		// UNTESTED
			MaxFramesPerSecond = 60;
			break;
		default:
			assert(FALSE);
		}
}

	





void PrintCurrentVideoMode (void)
{
	int currentMode;
	
	currentMode = GetVideoMode();

	printf("\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("Currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("Currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}
}
	






	// simplest overall initialiser

void SortVideoMode (void)
{
	int currentMode;
	
	currentMode = GetVideoMode();

	printf("\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}

	printf("If everything is black and white\n");
	printf("or there are lines at bottom or top of screen\n");
	printf("Then you need to change video mode\n\n");
}

