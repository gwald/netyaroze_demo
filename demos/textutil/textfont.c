/***************************************************************
*                                                              *
* Copyright (C) 1995 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 29/11/96                                           *
*                                                              *
***************************************************************/


#include <libps.h>
#include "textfont.h"
#include "myassert.h"

extern u_long PSDCNT;


void initTextFont (u_long *timAddress)
{
	int i;
	GsIMAGE image;
	RECT frameRect;
	u_long eightBitTextureAttributeMask = 1L << 24;
	u_long sixteenBitTextureAttributeMask = 1L << 25;
	u_long mask0 = 1L << 28;
	u_long mask1 = 1L << 29;
	u_long mask2 = 1L << 30;
	u_char pixper16bits;

//GsIncFrame();  
//(PSDCNT=(PSDCNT++ == 0?1:PSDCNT)), \
//                      (PSDIDX= (PSDIDX==0?1:0));

	spriteStringCurrentBuffer = GsGetActiveBuff();
	spriteStringPacketPointer = spriteStringPacketArea[spriteStringCurrentBuffer];

	GsGetTimInfo(++timAddress, &image); 

		// load tpage	
	frameRect.x = image.px;
	frameRect.y = image.py;
	frameRect.w = image.pw;
	frameRect.h = image.ph;
	LoadImage(&frameRect, image.pixel);
	DrawSync(0);

	switch (image.pmode)
		{
		case 8: // 4-bit CLUT
			// load clut
			frameRect.x = image.cx;
			frameRect.y = image.cy;
			frameRect.w = image.cw;
			frameRect.h = image.ch;
			LoadImage(&frameRect, image.clut);
			DrawSync(0); 
			pixper16bits = 4;
			break;
		case 9: // 8-bit CLUT
			// load clut
			frameRect.x = image.cx;
			frameRect.y = image.cy;
			frameRect.w = image.cw;
			frameRect.h = image.ch;
			LoadImage(&frameRect, image.clut);
			DrawSync(0); 
			pixper16bits = 2;
			spriteString.attribute |= eightBitTextureAttributeMask;
			break;
		case 2: // 16-bit
			pixper16bits = 1;
			spriteString.attribute |= sixteenBitTextureAttributeMask;
			break;
		default: // 24-bit and mixed
			return;
		}
   
	spriteString.w = 16;
	spriteString.h = 16;
	spriteString.cx = image.cx;
	spriteString.cy = image.cy;
	spriteString.mx = 0;
	spriteString.my = 0;

	spriteString.tpage = GetTPage(image.pmode & 3, 0, image.px, image.py);

	spriteString.attribute = 0;
	spriteString.attribute |= mask1;
	spriteString.attribute |= mask2;
				
	spriteString.rotate = 0;
}


void textPrintToBox(RECT stringBox, u_char r, u_char g, u_char b, char *string, GsOT *otp, u_short pri)
{
	u_short MAX_Xchar = 0;
	u_short Xchar = 0;
	u_short Ychar = 1;
	u_short pixPerX;
	u_short pixPerY;
	RECT textBox = stringBox;
	long Xscale;
	long Yscale;
	char *countString = string;
	PACKET *currentPacketArea;

	currentPacketArea = GsGetWorkBase();

	if (spriteStringCurrentBuffer != GsGetActiveBuff())
		{
		spriteStringCurrentBuffer = GsGetActiveBuff();
		spriteStringPacketPointer = spriteStringPacketArea[spriteStringCurrentBuffer];
		}

	GsSetWorkBase(spriteStringPacketPointer);

		// find number of characters both x and y to calculate scaling
	while(*countString != NULL)
		{
		switch (*countString)
			{
			case 10:
			case 13:
				Ychar++;
				Xchar = 0;
				break;
			default:
				Xchar++;
				if (Xchar > MAX_Xchar)
					{
					MAX_Xchar = Xchar;
					}
			}
		countString++;
		}

		// calculate scaling so text fits box
	pixPerX = textBox.w / MAX_Xchar;
	pixPerY = textBox.h / Ychar;
	Xscale = ONE * pixPerX / 16;
	Yscale = ONE * pixPerY / 16;

	while (*string != NULL)
		{
		switch (*string)
			{
			case 10:
			case 13:
				textBox.y += pixPerY;
				textBox.x = stringBox.x;
				break;
			default:
				spriteString.x = textBox.x;
				spriteString.y = textBox.y;
				textBox.x += pixPerX;

				spriteString.u = (*string % 16) * 16;
				spriteString.v = (*string / 16) * 16;

				spriteString.r = r;
				spriteString.g = g;
				spriteString.b = b;

				spriteString.scalex = Xscale;
				spriteString.scaley = Yscale;

				GsSortSprite(&spriteString, otp, pri);
			}
		string++;
		}
	spriteStringPacketPointer = GsGetWorkBase();
	GsSetWorkBase(currentPacketArea);
}

	
void textPrintToSize(u_short x, u_short y, u_short sizeInPixels, u_char r, u_char g, u_char b, char *string, GsOT *otp, u_short pri)
{
	u_short Xpos = x;
	u_short Ypos = y;
	long scale;
	PACKET *currentPacketArea;

	currentPacketArea = GsGetWorkBase();

	if (spriteStringCurrentBuffer != GsGetActiveBuff())
		{
		spriteStringCurrentBuffer = GsGetActiveBuff();
		spriteStringPacketPointer = spriteStringPacketArea[spriteStringCurrentBuffer];
		}

	GsSetWorkBase(spriteStringPacketPointer);


		// calculate scaling
	scale = ONE * sizeInPixels / 16;

 	while (*string != NULL)
		{
		switch (*string)
			{
			case 10:
			case 13:
				Ypos += sizeInPixels;
				Xpos = x;
				break;
			default:
				spriteString.x = Xpos;
				spriteString.y = Ypos;
				Xpos += sizeInPixels;

				spriteString.u = (*string % 16) * 16;
				spriteString.v = (*string / 16) * 16;

				spriteString.r = r;
				spriteString.g = g;
				spriteString.b = b;

				spriteString.scalex = scale;
				spriteString.scaley = scale;

				GsSortSprite(&spriteString, otp, pri);
			}
		string++;
		}
	spriteStringPacketPointer = GsGetWorkBase();
	GsSetWorkBase(currentPacketArea);
}
