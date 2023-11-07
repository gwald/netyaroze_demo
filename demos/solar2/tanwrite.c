

#include <stdio.h>
#include <assert.h>

#include <math.h>




#define KeepWithinRange(x, min, max)			\
	{											\
	if ((x) < (min))							\
		(x) = (min);							\
	else if ((x) > (max))						\
		(x) = (max);							\
	}

void main (int argc, char *argv[])
{
	FILE* fp;
	int i;
	double angle;
	int tangent;

	assert(argc == 2);
	fp = fopen(argv[1], "w");
	assert(fp != NULL);

	fprintf(fp, "\n\nshort int TangentLUT[] = \n{\n");

	for (i = 0; i < 4096; i++)
		{
		angle = (((double)i) / ((double)4096)) * ((double)6.28318);
		tangent = (int) (4096 * tan(angle));
		KeepWithinRange(tangent, -2000000000, 2000000000);
		if (i % 100 == 0)
			{
			printf("i is %d, angle %f, tangent %d\n",
				i, angle, tangent);
			}
		fprintf(fp, "\t%d,\n", tangent);
		}

	fprintf(fp, "}\n\n");
	fclose(fp);
}
	