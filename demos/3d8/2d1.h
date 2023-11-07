#ifndef TWO_D_1_H_INCLUDED 




#include <libps.h>

#include <stdio.h>

#include "general.h"

#include "asssert.h"

#include "dump.h"




#define SIXTEEN_BIT_MASK (1 << 25)
#define EIGHT_BIT_MASK (1 << 24)



void InitialiseTexture (long address);

	// the second argument is a global declared next to the #define for
	// the TIM file main memory address;
	// it allows endless sprites to be easily linked to it later on
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);

void SortSpriteCentreToRealCentre (GsSPRITE *sprite);

void InitGsSprite (GsSPRITE* sprite);

void CopySprite (GsSPRITE *from, GsSPRITE *to);

int Get32TPageNumber (int x, int y);

	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);


void GetTexturePage (GsIMAGE* imageInfo, int *tPageID,
							int* u, int* v);

void GetTexturePageOfRectangle (RECT *rect, int *tPageID, int *u, int *v);




void PrintGsImage (GsIMAGE* image, int textureFlag, int clutFlag);



u_short RgbToPixel (int r, int g, int b);
void BreakdownPixel (u_short pixel, int* r, int* g, int* b);



void CreateFirstImage (GsIMAGE* image);



#define TWO_D_1_H_INCLUDED 1

#endif