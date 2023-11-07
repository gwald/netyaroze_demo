#include "storescr.h"

	// stores screen as bona fide TIM file;
	// use siocons Dsave command to transfer directly to PC file

	// This stolen directly from sample\japanese\sscreen

void StoreScreen (u_long *destination, int x, int y, int w, int h)
{
	RECT rect;

	*(destination+0) = 0x00000010;		/* ID */
	*(destination+1) = 0x00000002;		/* FLAG(15bit Direct,No Clut) */
	*(destination+2) = (w*h/2+3)*4;		/* pixel bnum */
	*(destination+3) = ((0 & 0xffff) << 16) | (640 & 0xffff);
						/* pixel DX,DY: at 640, 0 */
	*(destination+4) = ((h & 0xffff) << 16) | (w & 0xffff);
						/* pixel W,H */

	// NO CLUT since 16-bit mode used

	rect.x = x; 
	rect.y = y;
	rect.w = w; 
	rect.h = h;
	DrawSync(0);
	StoreImage(&rect, destination+5);		

	printf("Press [F10][F4] for dsave.\n"); 
	printf("Dsave[0]: filename %08x %x\n", destination, (w*h/2+5)*4);

	DrawSync(0);
	VSync(0);
}
