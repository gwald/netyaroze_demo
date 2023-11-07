/************************************************************
 *															*
 *						vector.h							*
 *				miscellaneous vector routines				*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




// prototypes header file


void RandomVector (VECTOR* vector);
void RandomVectorOfGivenSize (VECTOR* vector, u_long size);
void ScaleVectorToUnit (VECTOR* vector);
void ScaleVectorToSize (VECTOR* vector, u_long size);
void AnyUnitPerpendicularVector (VECTOR* input, VECTOR* output);
void FindNormalVector (VECTOR* first, VECTOR* second, VECTOR* output);
