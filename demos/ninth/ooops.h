int rsin (int angle)
	{
	angle &= 4095;

	if (angle < 1024)
		return SinCosTable[angle];

	if (angle < 2048)	
		return SinCosTable[2048 - angle];

	if (angle < 3072) 
		return -SinCosTable[angle & 1023];

	return -SinCosTable[4096 - angle];
	}


