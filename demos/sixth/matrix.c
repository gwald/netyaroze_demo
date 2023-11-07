#include "matrix.h"




void InitMatrix (MATRIX* matrix)
{
	matrix->m[0][0] = ONE;
	matrix->m[0][1] = 0;
	matrix->m[0][2] = 0;
	matrix->m[1][0] = 0;
	matrix->m[1][1] = ONE;
	matrix->m[1][2] = 0;
	matrix->m[2][0] = 0;
	matrix->m[2][1] = 0;
	matrix->m[2][2] = ONE;

	matrix->t[0] = 0;
	matrix->t[1] = 0;
	matrix->t[2] = 0;
}




int MatrixIsUnitMatrix (MATRIX* matrix)
{
	if (matrix->m[0][0] != ONE
		|| matrix->m[1][1] != ONE
		|| matrix->m[2][2] != ONE)
			return FALSE;

	if (matrix->m[0][1] != 0
		|| matrix->m[0][2] != 0
		|| matrix->m[1][0] != 0
		|| matrix->m[1][2] != 0
		|| matrix->m[2][0] != 0
		|| matrix->m[2][1] != 0)
			return FALSE;

	if (matrix->t[0] != 0
		|| matrix->t[1] != 0
		|| matrix->t[2] != 0)
			return FALSE;

	return TRUE;
}





void CopyMatrix (MATRIX* from, MATRIX* to)
{
	assert(from != to);

	to->m[0][0] = from->m[0][0];
	to->m[0][1] = from->m[0][1];
	to->m[0][2] = from->m[0][2];
	to->m[1][0] = from->m[1][0];
	to->m[1][1] = from->m[1][1];
	to->m[1][2] = from->m[1][2];
	to->m[2][0] = from->m[2][0];
	to->m[2][1] = from->m[2][1];
	to->m[2][2] = from->m[2][2];

	to->t[0] = from->t[0];
	to->t[1] = from->t[1];
	to->t[2] = from->t[2];
}
	



	// MAIN USE is for the matrix part Not coordinate part ...

void FlipMatrix (MATRIX* input, MATRIX* output)
{
	assert(input != output);

	output->m[0][0] = input->m[0][0];
	output->m[0][1] = input->m[1][0];
	output->m[0][2] = input->m[2][0];
	output->m[1][0] = input->m[0][1];
	output->m[1][1] = input->m[1][1];
	output->m[1][2] = input->m[2][1];
	output->m[2][0] = input->m[0][2];
	output->m[2][1] = input->m[1][2];
	output->m[2][2] = input->m[2][2];

	output->t[0] = -input->t[0];
	output->t[1] = -input->t[1];
	output->t[2] = -input->t[2];
}