/************************************************************
 *															*
 *						Asssert.h							*
 *															*
 *		mis-spelt on purpose to avoid name conflicts 		*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




/********************************************************************

	assert(C expression):
		
	a helpful debugging macro that keeps code clean	and uncluttered.

	
	assert:
		if expression true, do nothing;
		if expression false, the assertion "fires":
			terminate program,
			print out message:
			"Assertion (expression) failed at line X of file Y"

	-- i.e. it is most useful for checking that a given 
	expression is true,
	rather than writing

	if (expression == 0)
		{
		// error handling
		}
	
	-- furthermore, the assertion-firing tells you exactly where it
	failed, hence you immediately know precisely where the trouble is. 

	Any valid C expression can be used; 
	usually use expressions that evaluate to 1 or 0 (true/false)


	This version of assert uses printf to print to the siocons
	console, ie to the MS-DOS box on the PC linked up to a Yaroze machine

	Main advantage of assert:
		it can be turned on and off by #define-switch
		i.e. by modifying this file only,
		no need to alter any using modules;
	and when assert is turned off,
		the assertion statements generate no code
		i.e. do not compromise efficiency or memory space at all	

   default is assertions ON;
   to turn them	OFF, 
   #define ASSERTIONS_ON (to any value)

*********************************************************************/






#ifdef ASSERTIONS_OFF

	#define assert(ignore)

#else

	#define assert(expression)									\
		( (expression) ? 1 :									\
				( printf("Assertion failure! " #expression		\
				"At line %d of file '%s'.\n",					\
				__LINE__, __FILE__), exit(1), 0) )

#endif


