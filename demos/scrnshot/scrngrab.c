/***************************************************************
*                                                              *
*       Copyright (C) 1997 by Sony Computer Entertainment      *
*                       All rights Reserved                    *
*                                                              *
*                      S. Ashley 16th Jan 97                   *
*                                                              *
***************************************************************/

#include "scrngrab.h"

// grab screen data and send to the console in an encrypted form
// a return of 0 is a fail, only happens if the coordinates are wrong
int dumpScreen(int topX, int leftY, int bottomX, int rightY)
{
	int y,x;
	u_short p[1024];
	RECT rect;
	u_char c;
	u_char old = 0;
	u_long i,t = 0;

	if (topX > bottomX || topX < 0 || bottomX > 1023 || leftY > rightY || leftY < 0 || rightY > 511)
		return 0;

	printf("screen dump start");   // represents the start of the dump and is used as a marker

	for (y = topY; y <= botY; y++)
		{
		rect.x = topX;
		rect.y = y;
		rect.w = botX - topX + 1;
		rect.h = 1;
		StoreImage(&rect, (u_long *)p);


		for (x = topX; x <= botX; x++)
			for (i = 0; i < 3; i++)          // RGB
				{
				switch (i)					 // separate colour data
					{
					case 0:	c = (p[x] & 0x1F) + 32; break;		  
					case 1:	c = ((p[x] & 0x3E0)>>5) + 32; break;
					case 2:	c = ((p[x] & 0x7C00)>>10) + 32;	break;
					}

				if (c == old)
					t++;
				else
					{
					if (t == 1)			// 2 the same
						printf("A");
					else if (t == 2)    // 3 the same
						printf("B");
					else if (t == 3) 	// 4 the same
						printf("C");
					else if (t > 3) 	// 5 or more the same (this stops file ever getting bigger)
						printf("D%c%c%c%c",((t & 0xFC0000)>>18)+32,((t & 0x3F000)>>12) + 32,((t & 0xFC0)>>6) + 32,(t & 0x3F)+32);
					
					if (old > 0)
						printf("%c",old);
					t = 0;
					old = c;
					}
				}
		}

	if (t == 1)			// 2 the same
		printf("A");
	else if (t == 2)    // 3 the same
		printf("B");
	else if (t == 3) 	// 4 the same
		printf("C");
	else if (t > 3) 	// 5 or more the same (this stops file ever getting bigger)
		printf("D%c%c%c%c",((t & 0xFC0000)>>18)+32,((t & 0x3F000)>>12) + 32,((t & 0xFC0)>>6) + 32,(t & 0x3F)+32);

	if (old > 0)
		printf("%c",old);

	printf("screen dump finish.");	// the end of the dump, as is used as the end marker

	return 1;
}