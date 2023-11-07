#include <libps.h>

#include "general.h"

#include "asssert.h"

#include "address.h"

#include "2D1.h"




#define MAX_TEXT_STRINGS 20
#define MAX_STRING_LENGTH 50



typedef struct
{
	int x, y;
	int scaleX, scaleY;
	int length;
	char data[MAX_STRING_LENGTH];
} TextString;





	// module initialiser: InitialiseAll
void InitialiseTextStrings (void);

	// module handler: once every frame
void DisplayTextStrings (GsOT* orderingTable);

	// internal
GsSPRITE* GetSpriteForCharacter (char character);

	// main interface function
void RegisterTextStringForDisplay (char* string, int x, int y);