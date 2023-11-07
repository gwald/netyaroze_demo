/************************************************************
 *															*
 *						Tim.c								*
 *															*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


// TO DO:
// organise file better => less messy

// another TIM loader: one that specifies where pixels and clut will go
// ie two extra arguments, both RECT * type

// function to get GsSPRITE from TIM file:
// watch for width/height distortion due to 4,8,16-bit texture type




#include <libps.h>

#include "tim.h"		// prototypes header






/***************************************************************** 

NOTE:	this approach relies Entirely on the TIM files
		holding the right contents about where they expect
		to be loaded in the frame buffer.
		When it is decided where they should go,
		edit the TIM files using the <timutil> utility,
		and specify the desired destinations. 
	
Method:

1.	Decide where to load the TIM files
 
2.	Put relevant commands in batch file
		e.g. local dload <TIM file name>.tim <address>
		e.g. local dload wheel256.tim 80190000

3.	Edit the TIM files using the <timutil> utility,
		so that they hold the correct destinations 
		in the frame buffer 

4.	Set up #defined-constants in C file to refer to the
		main memory addresses where the TIM files will be loaded

5.	During program initialisation, (after GsInitGraph), 
		call the function LoadAllTimFiles once,
		or call LoadTimFile for each TIM, with the relevant address

*********************************************************************/



#if 0		// example #define constants for TIM handling

#define TEXTURE_1_ADDRESS	0x80120000
#define TEXTURE_2_ADDRESS	0x80130000
#define TEXTURE_3_ADDRESS	0x80140000
#define TEXTURE_4_ADDRESS	0x80150000
#define TEXTURE_5_ADDRESS	0x80160000
#define TEXTURE_6_ADDRESS	0x80170000

#endif
		

#if 0
	// example of a simple call-once function 
	// that does all the TIM files in one go

	// will need editing for each new project
void LoadAllTimFiles (void)
{
	LoadTimFile(TEXTURE_1_ADDRESS);
	LoadTimFile(TEXTURE_2_ADDRESS);
	LoadTimFile(TEXTURE_3_ADDRESS);
	LoadTimFile(TEXTURE_4_ADDRESS);
	LoadTimFile(TEXTURE_5_ADDRESS);
	LoadTimFile(TEXTURE_6_ADDRESS);
}
#endif





	// Function to load TIM picture/texture to the frame buffer
	// the argument is the address in main memory where TIM file 
	// will be loaded; ensure exact match with the batch file 
	// that actually loads the TIM file

	// This function relies entirely on the TIM file for information
	// about where it should be loaded onto the frame buffer

void LoadTimFile (u_long mainMemoryAddress)
{
	RECT	rectangle;		// destination rectangle on frame buffer
	GsIMAGE	timImage;		// structure holding info on TIM file

		// get information about TIM file into a GsIMAGE structure
		// use (address+4) to skip the header of the TIM file 
	GsGetTimInfo( (u_long *)(mainMemoryAddress+4), &timImage);

		// (x,y) coordinates of the top left point 
		// of the image data on the frame buffer
	rectangle.x = timImage.px;
	rectangle.y = timImage.py;

		// width and height of image
	rectangle.w = timImage.pw;
	rectangle.h = timImage.ph;

		// transfer data to the frame buffer (DMA transfer request) 
	LoadImage(&rectangle, timImage.pixel);

		// wait for DMA transfer to finish
		// (LoadImage is a non-blocking function)
	DrawSync(0);

		// if pixel mode requires a CLUT, load the CLUT
		// CLUTs are required for 8-bit and 4-bit textures
	if( (timImage.pmode>>3) & 0x01)
		{
			// (x,y) coordiantes of top-left point 
			// of CLUT on frame buffer
		rectangle.x = timImage.cx;
		rectangle.y = timImage.cy;

			// width and height of CLUT
		rectangle.w = timImage.cw;
		rectangle.h = timImage.ch;	   // note: always one

			// Load CLUT data to frame buffer
		LoadImage(&rectangle, timImage.clut);
			// wait for DMA transfer to finish
		DrawSync(0);
		}
}





	// this function also loads TIM file into frame buffer
	// from main memory; but it forces the data to go to 
	// a certain position on the frame buffer, rather than using the
	// TIM file's own info on where it should go

	// if no CLUT is needed, function will still work fine,
	// just pass any dummy pointer-to-RECT as third argument

void LoadTimFileToGivenDestinations (u_long mainMemoryAddress,
				RECT* pixelDestination, RECT* clutDestination)
{
	GsIMAGE	timImage;		// structure holding info on TIM file

		// get information about TIM file into a GsIMAGE structure
		// use (address+4) to skip the header of the TIM file 
	GsGetTimInfo( (u_long *)(mainMemoryAddress+4), &timImage);

		// transfer data to the frame buffer (DMA transfer request) 
	LoadImage(pixelDestination, timImage.pixel);

		// wait for DMA transfer to finish
		// (LoadImage is a non-blocking function)
	DrawSync(0);

		// if pixel mode requires a CLUT, load the CLUT
		// CLUTs are required for 8-bit and 4-bit textures
	if( (timImage.pmode>>3) & 0x01)
		{
			// Load CLUT data to frame buffer
		LoadImage(clutDestination, timImage.clut);
			// wait for DMA transfer to finish
		DrawSync(0);
		}
}





