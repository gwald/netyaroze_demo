#include"atan.h"




	// Need for extra level:
	// the LUT and access functions were used for calculating
	// angle theta-Y on the x-z plane;
	// this ratan stub converts back to proper theta-Z on xy plane

int ratan (int x, int y)
{
	int angle = SUBratan(x, y);
	
	angle = 1024 - angle;

	if (angle < 0)
		angle += 2048;

	if (angle > 2048)
		angle -= 2048;

	return angle;
}



	// call with value as actual value * ONE
int ratan2 (int value)
{
	return ( ratan(ONE, value) );
}




	// Note: origin (x,z)=(0,1), clockwise rotation
	// this was originally done for atan on the x-z plane

int SUBratan (int x, int z)
{
  	if (x*z == 0)		  // if either is zero
    	return(ratan_zero(x,z));

  	if (x>0)
  		{
    	if (z>0)
    		{
      		x = abs(x);
      		z = abs(z);
      		if (x<=z)
				return(ArcTangentTable[255*x/z]);
      		else
				return(1024-ArcTangentTable[255*z/x]);
    		}
    	else
    		{
      		x = abs(x);
      		z = abs(z);
      		if (x<=z)
				return(2048-ArcTangentTable[255*x/z]);
      		else
				return(1024+ArcTangentTable[255*z/x]);
    		}
  		}
  	else
  		{
    	if (z>0)
    		{
      		x = abs(x);
      		z = abs(z);
      		if (x<=z)
				return(-ArcTangentTable[255*x/z]);
      		else
				return(-1024+ArcTangentTable[255*z/x]);
    		}
    	else
    		{
      		x = abs(x);
      		z = abs(z);
      		if (x<=z)
				return(-2048+ArcTangentTable[255*x/z]);
      		else
				return(-1024-ArcTangentTable[255*z/x]);
    		}
  		}
}




int ratan_zero (int x, int z)
{
  	if (x == 0)
	  	{
	    if(z >= 0)
	      	return(0);
	    else
	      	return(2048);
	  	}
  	else if (z == 0)
	  	{
	    if(x > 0)
	      	return(1024);
	    else
	      	return(-1024);
	  	}
	else
    	return(0);
}










int ArcTangentTable[256] = 
{
    2,    5,    7,   10,   12,   15,   17,   20,   
    22,   25,   27,   30,   33,   35,   38,   40,
   43,   45,   48,   50,   53,   55,   58,   60,   
   63,   65,   68,   71,   73,   76,   78,   81,
   83,   86,   88,   91,   93,   96,   98,  101,  
   103,  106,  108,  110,  113,  115,  118,  120,
  123,  125,  128,  130,  133,  135,  137,  140,  
  142,  145,  147,  150,  152,  154,  157,  159,
  162,  164,  166,  169,  171,  174,  176,  178,  
  181,  183,  185,  188,  190,  192,  195,  197,
  199,  202,  204,  206,  208,  211,  213,  215,  
  218,  220,  222,  224,  227,  229,  231,  233,
  236,  238,  240,  242,  244,  247,  249,  251,  
  253,  255,  258,  260,  262,  264,  266,  268,
  270,  273,  275,  277,  279,  281,  283,  285,  
  287,  289,  291,  294,  296,  298,  300,  302,
  304,  306,  308,  310,  312,  314,  316,  318,  
  320,  322,  324,  326,  328,  330,  332,  334,
  335,  337,  339,  341,  343,  345,  347,  349,  
  351,  353,  354,  356,  358,  360,  362,  364,
  365,  367,  369,  371,  373,  375,  376,  378,  
  380,  382,  383,  385,  387,  389,  390,  392,
  394,  396,  397,  399,  401,  402,  404,  406,  
  407,  409,  411,  412,  414,  416,  417,  419,
  421,  422,  424,  425,  427,  429,  430,  432,  
  433,  435,  437,  438,  440,  441,  443,  444,
  446,  447,  449,  450,  452,  453,  455,  456,  
  458,  459,  461,  462,  464,  465,  467,  468,
  470,  471,  472,  474,  475,  477,  478,  479, 
  481,  482,  484,  485,  486,  488,  489,  490,
  492,  493,  495,  496,  497,  499,  500,  501,  
  502,  504,  505,  506,  508,  509,  510,  512
};

