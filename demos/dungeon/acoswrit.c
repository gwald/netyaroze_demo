#include <stdio.h>
#include <math.h>
#include <assert.h>


#define PI 3.14159

void main (int argc, char* argv[])
{
	FILE* fp;
	int i;
	double realAngle, realValue;
	int intAngle, intValue;
		
	assert(argc == 2);

	fp = fopen(argv[1], "w");
	assert(fp != NULL);

	fprintf(fp, "int ArcCosTable[] = \n{\n");

	printf("arccosines: -1 %f, -0.5 %f\n",
		acos(-1), acos(-0.5) );
	printf("0: %f, 0.5: %f, 1: %f\n", 
		acos(0), acos(0.5), acos(1) );
	
	for (i = 0; i < 4097; i++)
		{
		intValue = i - 2048;
		realValue = ((double)intValue) / ((double)2048);
		assert(realValue >= -1 && realValue <= 1);
		realAngle = acos(realValue);
		intAngle = (realAngle * 4096) / (2 * PI);
		fprintf(fp, "\t%d,\n", intAngle);
			
		if (i % 2000 == 0)
			{
			printf("i: %d, realValue: %f\n", i, realValue);
			printf("realAngle: %f, intAngle: %d\n", realAngle, realValue);
			}
		}

	fprintf(fp, "}\n\n");

	fclose(fp);
}

	