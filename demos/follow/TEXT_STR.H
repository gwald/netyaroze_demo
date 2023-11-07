#ifndef TEXT_STR_H_INCLUDED





#include <libps.h>

#include "general.h"

#include "asssert.h"

#include "address.h"

#include "2D1.h"

#include "dump.h"




#define MAX_TEXT_STRINGS 20
#define MAX_STRING_LENGTH 100



typedef struct
{
	int x, y;
	int scaleX, scaleY;
	int length;
	int colour;
	char data[MAX_STRING_LENGTH];
} TextString;





	// module initialiser: InitialiseAll
void InitialiseTextStrings (void);

	// module handler: once every frame
void DisplayTextStrings (GsOT* orderingTable);

	// internal
GsSPRITE* GetSpriteForCharacter (char character);

#define NORMAL_COLOUR 16912

#define RED_COLOUR 31

	// main interface function
	// set colour to 16912 for normal colour
void RegisterTextStringForDisplay (char* string, int x, int y, int colour);







#define TEXT_STR_H_INCLUDED 1

#endif
