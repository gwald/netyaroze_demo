// 
// sprite.c 
//
// Sprite loading & manipulation functions
// GNU Version
// 
// Andrew Murray - 9th October 2002
// 

#include <libps.h>

#include "sprite.h"

// --------------------------------------------------------------------------------
// Function Definitions
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Name:	LoadSprite() 
// Desc:	Will load an image into the passed image structure pointer.
// --------------------------------------------------------------------------------
void LoadSprite(GsIMAGE *pImage, unsigned long* addr)
{
	// use a rect to set up areas on VRAM for image loading
	RECT rect;
	
	// this stores the address of the sprite in main memory
	unsigned long *spriteAddr;

	// grab the address of the buffer for sprite loading
	spriteAddr = addr;
	
	// increment address to point past TIM header
	// (+4 bytes i.e. sizeof(unsigned long))
	spriteAddr++;
	
	// put the TIM info inside the GsIMAGE structure "image"
	GsGetTimInfo(spriteAddr, pImage);
	
	// set an area in VRAM to load the image onto
	setRECT(&rect, pImage->px, pImage->py, pImage->pw, pImage->ph);

	// load the image into the specified area in VRAM
	LoadImage(&rect, pImage->pixel);
    
	// if the image has a colour lookup table
	if((pImage->pmode >> 3) & 0x01)
	{
		// set the area in VRAM to load the CLUT into
		setRECT(&rect, pImage->cx, pImage->cy, pImage->cw, pImage->ch);

		// load the CLUT onto the VRAM
		LoadImage(&rect, pImage->clut);
	}
}


// --------------------------------------------------------------------------------
// Name:	InitSprite()
// Desc:	Will create a new sprite from the passed image structure.
// --------------------------------------------------------------------------------
void InitSprite(GsSPRITE* pSprite, GsIMAGE *pImage)
{
	// check for bit depth
	switch(pImage->pmode)
	{
		// 4 bit image quarter of actual image width
		case FOURBITIMAGE:
			pSprite->attribute = FOURBITCOLOUR;
			pSprite->w = pImage->pw * 4;
			pSprite->tpage = GetTPage(0, 0, pImage->px, pImage->py);
			break;
			
		// 8 bit image half of actual image width
		case EIGHTBITIMAGE:
			pSprite->attribute = EIGHTBITCOLOUR;
			pSprite->w = pImage->pw * 2;
			pSprite->tpage = GetTPage(1, 0, pImage->px, pImage->py);
			break;
			
		// 16 bit image takes up full image width
		default:
			pSprite->attribute = SIXTEENBITCOLOUR;
			pSprite->w = pImage->pw;
			pSprite->tpage = GetTPage(2, 0, pImage->px, pImage->py);
			break;
	}
		  
	// set sprite default attributes
	pSprite->x = 0;
	pSprite->y = 0;

	pSprite->h = pImage->ph;

	// sprite offset adjust for animation
	pSprite->u = 0;
	pSprite->v = 0;
	
	// default brightness RGB
	pSprite->r = 128;
	pSprite->g = 128;
	pSprite->b = 128;
	
	// CLUT position in VRAM
	pSprite->cx = pImage->cx;
	pSprite->cy = pImage->cy;

	// ensure that image is of even width
	pSprite->mx = pSprite->w/2;
	pSprite->my = pSprite->h/2;
		
	pSprite->scalex = ONE;
	pSprite->scaley = ONE;
	
	pSprite->rotate = 0;
}
