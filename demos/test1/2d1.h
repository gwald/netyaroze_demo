#include <libps.h>

#include "general.h"

#include "asssert.h"




#define SIXTEEN_BIT_MASK (1 << 25)
#define EIGHT_BIT_MASK (1 << 24)



void InitialiseTexture (long address);

	// the second argument is a global declared next to the #define for
	// the TIM file main memory address;
	// it allows endless sprites to be easily linked to it later on
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);

void InitGsSprite (GsSPRITE* sprite);

	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);