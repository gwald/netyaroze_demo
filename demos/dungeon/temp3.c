#include "atan.h"






void main (void);


void main (void)
{
	int x, y;
	int angle;

	for (x = -10; x < 10; x++)	
		{
		for (y = -10; y < 10; y++)
			{
			angle = ratan(x,y);
			printf("x: %d, y: %d, angle: %d\n", x, y, angle);
			printf("in degrees: %d\n", ((angle * 360)>>12));
			}
		}
}
	