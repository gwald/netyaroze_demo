/*

	FFT routine....

	This takes an array of shorts, and performs a 256 point FFT,

*/


signed long real[256];					// FFT values....
signed long imag[256];


short b_table [] = {
    0, 128, 64, 192, 32, 160, 96, 224,
    16, 144, 80, 208, 48, 176, 112, 240,
    8, 136, 72, 200, 40, 168, 104, 232,
    24, 152, 88, 216, 56, 184, 120, 248,

    4, 132, 68, 196, 36, 164, 100, 228,
    20, 148, 84, 212, 52, 180, 116, 244,
    12, 140, 76, 204, 44, 172, 108, 236,
    28, 156, 92, 220, 60, 188, 124, 252,

    2, 130, 66, 194, 34, 162, 98, 226,
    18, 146, 82, 210, 50, 178, 114, 242,
    10, 138, 74, 202, 42, 170, 106, 234,
    26, 154, 90, 218, 58, 186, 122, 250,

    6, 134, 70, 198, 38, 166, 102, 230,
    22, 150, 86, 214, 54, 182, 118, 246,
    14, 142, 78, 206, 46, 174, 110, 238,
    30, 158, 94, 222, 62, 190, 126, 254,

    1, 129, 65, 193, 33, 161, 97, 225,
    17, 145, 81, 209, 49, 177, 113, 241,
    9, 137, 73, 201, 41, 169, 105, 233,
    25, 153, 89, 217, 57, 185, 121, 249,

    5, 133, 69, 197, 37, 165, 101, 229,
    21, 149, 85, 213, 53, 181, 117, 245,
    13, 141, 77, 205, 45, 173, 109, 237,
    29, 157, 93, 221, 61, 189, 125, 253,

    3, 131, 67, 195, 35, 163, 99, 227,
    19, 147, 83, 211, 51, 179, 115, 243,
    11, 139, 75, 203, 43, 171, 107, 235,
    27, 155, 91, 219, 59, 187, 123, 251,

    7, 135, 71, 199, 39, 167, 103, 231,
    23, 151, 87, 215, 55, 183, 119, 247,
    15, 143, 79, 207, 47, 175, 111, 239,
    31, 159, 95, 223, 63, 191, 127, 255
};

short s_table [] = {
    0,   6,  12,  18,  24,  31,  37,  43,
    49,  55,  61,  68,  74,  79,  85,  91,
    97, 103, 109, 114, 120, 125, 131, 136,
    141, 146, 151, 156, 161, 166, 171, 175,
    180, 184, 188, 193, 197, 201, 204, 208,
    212, 215, 218, 221, 224, 227, 230, 233,
    235, 237, 240, 242, 244, 245, 247, 248,
    250, 251, 252, 253, 253, 254, 254, 254,
    254, 254, 254, 254, 253, 253, 252, 251,
    250, 248, 247, 245, 244, 242, 240, 237,
    235, 233, 230, 227, 224, 221, 218, 215,
    212, 208, 204, 201, 197, 193, 188, 184,
    180, 175, 171, 166, 161, 156, 151, 146,
    141, 136, 131, 125, 120, 114, 109, 103,
    97, 91, 85, 79, 74, 68, 61, 55,
    49, 43, 37, 31, 24, 18, 12, 6,
    0, -6, -12, -18, -24, -31, -37, -43,
    -49, -55, -61, -68, -74, -79, -85, -91,
    -97, -103, -109, -114, -120, -125, -131, -136,
    -141, -146, -151, -156, -161, -166, -171, -175,
    -180, -184, -188, -193, -197, -201, -204, -208,
    -212, -215, -218, -221, -224, -227, -230, -233,
    -235, -237, -240, -242, -244, -245, -247, -248,
    -250, -251, -252, -253, -253, -254, -254, -254,
    -255, -254, -254, -254, -253, -253, -252, -251,
    -250, -248, -247, -245, -244, -242, -240, -237,
    -235, -233, -230, -227, -224, -221, -218, -215,
    -212, -208, -204, -201, -197, -193, -188, -184,
    -180, -175, -171, -166, -161, -156, -151, -146,
    -141, -136, -131, -125, -120, -114, -109, -103,
    -97, -91, -85, -79, -74, -68, -61, -55,
    -49, -43, -37, -31, -24, -18, -12, -6
};


void fft()
{

    register long i, j, stage, ik, h, d, k2;
    register int s, c, dx, dy;


	d = 256;
    for (stage = 1; stage < 256; stage *= 2)
    {
		h = 0;
		k2 = stage << 1;
		d >>=1;
		for (j = 0; j < stage; j ++)
		{
	    	c = s_table[h + 64];
		    s = s_table[h];

		    for (i = j; i < 256; i += k2)
		    {
				ik = i + stage;

				dx = (s * imag[ik] + c * real[ik]) / 256;
				dy = (c * imag[ik] - s * real[ik]) / 256;

				real[ik] = (real[i]-dx)/2;
				real[i]	= (real[i]+dx)/2;
				imag[ik] = (imag[i] - dy)/2;
				imag[i] = (imag[i]+dy)/2;
		    }
	    h += d;
		}
    }


}
