// first and simplest 2d module



#include "2d1.h"


	// NO overall initialiser



	// just loads texture; no image info stored
void InitialiseTexture (long address)
{
	RECT rect;
	GsIMAGE timInfo;

	GsGetTimInfo((u_long *)(address+4), &timInfo);

	rect.x = timInfo.px;	
	rect.y = timInfo.py;		
	rect.w = timInfo.pw;	
	rect.h = timInfo.ph;	


	LoadImage(&rect, timInfo.pixel);

	if ( (timInfo.pmode>>3) & 0x01 ) 
		{
		rect.x = timInfo.cx;	
		rect.y = timInfo.cy;	
		rect.w = timInfo.cw;	
		rect.h = timInfo.ch;	

		LoadImage(&rect, timInfo.clut);
   		}

	DrawSync(0);
}





	// the second argument is a global declared next to the #define for
	// the TIM file main memory address;
	// it allows endless sprites to be easily linked to it later on
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo)
{
	RECT rect;

		// +4 because we need to skip head of TIM file
	GsGetTimInfo( (u_long *)(address+4), imageInfo);

	rect.x = imageInfo->px;	
	rect.y = imageInfo->py;		
	rect.w = imageInfo->pw;	
	rect.h = imageInfo->ph;	

	LoadImage(	&rect, imageInfo->pixel);

		// if image needs a CLUT, load it up 
		// (pmode: 8 or 9 ==> texture is 4-bit or 8-bit)
	if ( (imageInfo->pmode>>3)&0x01) 
		{
		rect.x = imageInfo->cx;	
		rect.y = imageInfo->cy;	
		rect.w = imageInfo->cw;	
		rect.h = imageInfo->ch;	

		LoadImage( &rect, imageInfo->clut);
		}

		// wait for loading to finish
	DrawSync(0);
}




	// automatically sorts out sprite attribute to sync with TIM;
	// sprite ready to use as is after this call
void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo)
{
	int widthCompression;	  // on frame buffer
	int tPageType;
	int texturePageX, texturePageY, xOffset, yOffset;

	InitGsSprite(sprite);
		
	FindTopLeftOfTexturePage(imageInfo, 
		&texturePageX, &texturePageY, &xOffset, &yOffset);

	switch(imageInfo->pmode)
		{
		case 0:		   // 16-bit
			sprite->attribute |= SIXTEEN_BIT_MASK;		
			widthCompression = 1;
			tPageType = 2;	
			break;
		case 8:		   	 // 4-bit
			widthCompression = 4;
			tPageType = 0;
			sprite->cx = imageInfo->cx;
			sprite->cy = imageInfo->cy;	
			break;
		case 9:		   // 8-bit
			sprite->attribute |= EIGHT_BIT_MASK;		
			widthCompression = 2;
			tPageType = 1;
			sprite->cx = imageInfo->cx;
			sprite->cy = imageInfo->cy;		
			break;
		default:
			printf("Only 4, 8 and 16 bit modes supported\n");
			printf("check case value for 16-bit: try 2\n");
			assert(FALSE);		// other modes not supported
		}

	sprite->tpage 
			= GetTPage(tPageType, 0, texturePageX, texturePageY);

	sprite->w = imageInfo->pw * widthCompression;
	sprite->h = imageInfo->ph;

	sprite->u = xOffset;
	sprite->v = yOffset;
}



	// init to void
void InitGsSprite (GsSPRITE* sprite)
{
		// initialise sprite to dummy
	sprite->attribute = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->w = 0;
	sprite->h = 0;
	sprite->tpage = 0;
	sprite->u = 0;
	sprite->v = 0;
	sprite->cx = 0;
	sprite->cy = 0;
	sprite->r = 128;
	sprite->g = 128;
	sprite->b = 128;
	sprite->mx = 0;
	sprite->my = 0;
	sprite->scalex = ONE;
	sprite->scaley = ONE;
	sprite->rotate = 0;
}






	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v)
{
	int testX, testY;

	testX = imageInfo->px;
	testY = imageInfo->py;

	*u = 0;
	*v = 0;
	
	for (;;)
		{
		if (testX % 320 != 0)
			{
			testX--;
			(*u)++;
			}
		else
			break;
		}

	for (;;)
		{
		if (testY % 256 != 0)
			{
			testY--;
			(*v)++;
			}
		else
			break;
		}

	*x = testX;
	*y = testY;
}




#if 0		// ALL VERY MESSY: PUT INTO 2d2.c

	// print functions for getting info on TIMs and GsIMAGEs
	
	// already have dumpSPRITE and dumpIMAGE;
	// since most TIMs will be linked to GsIMAGE via
	// ProperInitialiseTexture





	// the two flags: 1/0, TRUE or FALSE for print / no-print

void PrintGsImage (GsIMAGE* image, int textureFlag, int clutFlag)
{
	dumpIMAGE(image);

	if (textureFlag)
		{
		int i, j, index;
		
		printf("\nNow printing entire texture data\n");
		printf("number u_short pixels to print %d\n\n\n", 
									image->pw * image->ph);

		for (i = 0; i < image->pw; i++)
			{
			for (j = 0; j < image->ph; j++)
				{
				index = (j * image->pw) + i;
				printf("%ul\n", image->pixel[index]);
				}
			printf("\n");
			}
		printf("\n\n\n--------- End Of Texture ---------\n\n\n\n\n\n");
		}

	if (clutFlag)
		{
		int i, j, index;
		u_short value1, value2;

		if (image->pmode != 8 && image->pmode != 9)
			{
			printf("image Neither 4 or 8 bit: no clut, dodgy call\n");
			return;
			}

		printf("\nNow printing entire clut data\n\n\n");
		printf("number u_short clut cells to print %d\n\n\n", 
									image->cw * image->ch);

		for (i = 0; i < image->cw; i++)
			{
			for (j = 0; j < image->ch; j++)
				{
				index = (j * image->cw) + i;
				value1 = image->clut[index] >> 16;
				value2 = image->clut[index] & 0xffff;
				printf("%u %u\n", value1, value2);
				}
			printf("\n");
			}

		printf("\n\n--------- End Of Clut ---------\n\n\n\n");
		}
}
	



u_short RgbToPixel (int r, int g, int b)
{
	u_short pixel;
	
	pixel = r;
	pixel += (g << 5);
	pixel += (b << 10);

	return pixel;
}



void BreakdownPixel (u_short pixel, int* r, int* g, int* b)
{
	*r = pixel & 31L;
	*g = (pixel >> 5) & 31L;
	*b = (pixel >> 10) & 31L;
}
	




u_short clut[256];
u_short pixels[32 * 64], pixel;
u_long *clutP, *pixelP;
RECT rect1, rect2;


void CreateFirstImage (GsIMAGE* image)
{
	int i, j;
	int index;

	printf("at start of CreateFirstImage\n");

	for (i = 0; i < 32; i++)
		{
		for (j = 0; j < 64; j++)
			{
			pixel = (j << 8) + j;
			index = (j * 32) + i;
			pixels[index] = pixel;
			}
		}

	for (i = 0; i < 256; i++)
		clut[i] = RgbToPixel(i % 32, i % 32, i % 32);

	
	setRECT( &rect1, 0, 490, 256, 1);
	clutP = (u_long*) clut;
	LoadImage( &rect1, clutP);

	setRECT( &rect2, 640, 128, 32, 64);
	pixelP = (u_long*) pixels;
	LoadImage( &rect2, clutP);

	DrawSync(0);

	image->pmode = 8;
	image->px = 640;
	image->py = 128;
	image->pw = 32;
	image->ph = 64;

	image->pixel = (u_long*) pixels;

	image->cx = 0;
	image->cy = 490;
	image->cw = 256;
	image->ch = 1;

	image->clut = (u_long*) clut;

	printf("at end of CreateFirstImage\n");
}





	// see function StoreScreen 

void CreateTimFromImage (GsIMAGE* image, u_long timAddress)
{
	u_long* pointer;
	int w, h;
	int pModeFlag;

	dumpIMAGE(image);
	printf("destination: 0x%X\n", timAddress);

	w = image->pw;
	h = image->ph;
		
	switch(image->pmode)
		{
		case 0:				 // 16 bit
			pModeFlag = 2;
			break;
		case 8:				  // 4 bit
			pModeFlag = 0;
			pModeFlag |= 1<<3;		// clut exists
			break;
		case 9:				  // 8 bit
			pModeFlag = 1;
			pModeFlag |= 1<<3;		// clut exists
			break;
		default:
			printf("only 4, 8 and 16 bit modes supported\n");
			exit(1);
		}

	assert(timAddress >= 0x80090000);
	assert(timAddress < 0x801fffff);

	pointer = (u_long*) timAddress;	

	*(pointer+0) = 0x00000010;		/* TIM file ID */
	*(pointer+1) = pModeFlag;		

	assert(pModeFlag == 2);		// clut section not sorted

		// HERE: changes for clut section
		// header bnum, RECT clut, memory copy from image->clut to TIM

	*(pointer+2) = (w*h/2+3)*4;		/* pixel bnum */
	*(pointer+3) = ((0 & 0xffff) << 16) | (640 & 0xffff);
						/* pixel DX,DY: at 640, 0 */
	*(pointer+4) = ((h & 0xffff) << 16) | (w & 0xffff);
						/* pixel W,H */

	// HERE: memory copy from image->pixel to TIM
	// eg for (i = 0; i < w; i++)
		// another j loop;    *pointer+4 + index = ....
}
#endif