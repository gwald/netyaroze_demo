#include <stdio.h>
#include <assert.h>
#include <math.h>
#define ONE 4096
#include "asincos.c"




#define PI 3.14159



void main (void)
{
    int i;
    double angle, sine, cose;
    FILE* fp;
	int sineValue;
	int coseValue;
	int	rasine1, rasine2;
	int racose1, racose2; 

    fp = fopen("test123", "w");
    assert(fp != NULL);

    for (angle = 0; angle <= ((2 * PI) + 0.05); angle += PI/4)
        {
        sine = sin(angle);
        cose = cos(angle);
		sineValue = ((int) (sine * 2048));
		coseValue = ((int) (cose * 2048));
		printf("sineValue: %d, coseValue: %d\n",
			sineValue, coseValue);
		rasin(sineValue, &rasine1, &rasine2);
		racos(coseValue, &racose1, &racose2);
        fprintf(fp, "angle: %f, sin: %f, cos: %f\n",
            angle, sine, cose);
        fprintf(fp, "arcsin of sin: %f, arccos of cos: %f\n",
            asin(sine), acos(cose));
		fprintf(fp, "rasin of sin: %d and %d\n",
			rasine1, rasine2);
		fprintf(fp, "racos of cos: %d and %d\n",
			racose1, racose2);
        }

    fclose(fp);
}
