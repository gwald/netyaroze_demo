//		basic text module


#include "text_str.h"


GsSPRITE LetterSprites[26];
GsSPRITE NumberSprites[10];
	 
TextString AllStrings[MAX_TEXT_STRINGS];
int NumberOfStrings;



	// address of ascii texture held in "address.h"
GsIMAGE AsciiTextureInfo;







void InitialiseTextStrings (void)
{
	int i;

	ProperInitialiseTexture(ASCII_TEXTURE_ADDRESS, &AsciiTextureInfo);
	
	for (i = 0; i < 26; i++)
		{
		LinkSpriteToImageInfo( &LetterSprites[i], &AsciiTextureInfo);
		LetterSprites[i].w = LetterSprites[i].h = 8;
		if (i < 15)
			{
			LetterSprites[i].u += 8 + (i * 8);
			LetterSprites[i].v += 16;
			}
		else
			{
			LetterSprites[i].u += ((i-15) * 8);
			LetterSprites[i].v += 24;
			}
		}
	for (i = 0; i < 10; i++)
		{
		LinkSpriteToImageInfo( &NumberSprites[i], &AsciiTextureInfo);
		NumberSprites[i].w = NumberSprites[i].h = 8;
		NumberSprites[i].u += (i * 8);
		NumberSprites[i].v += 8;
		}

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		AllStrings[i].x = 0;
		AllStrings[i].y = 0;
		AllStrings[i].length = 0;
		AllStrings[i].data[0] = '\n';
		}

	NumberOfStrings = 0;
}





void DisplayTextStrings (GsOT* orderingTable)
{
	int i, j;
	TextString* thisOne;
	GsSPRITE* sprite;
	int x, y;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length != 0)
			{
			thisOne = &AllStrings[i];
			x = thisOne->x;
			y = thisOne->y;

			for (j = 0; j < thisOne->length; j++)
				{
				sprite = GetSpriteForCharacter(thisOne->data[j]);

				if (sprite != NULL)
					{
					sprite->x = x;
					sprite->y = y;

					GsSortSprite(sprite, orderingTable, 0); 
					}
				x += 8;
				}

				// mark string as EMPTY
			thisOne->length = 0;
			}
		}

	NumberOfStrings = 0;
}






GsSPRITE* GetSpriteForCharacter (char character)
{
	GsSPRITE* result;
	int index;

	if ( ((character >= 48 && character <= 57)
		|| (character >= 65 && character <= 90) 
		|| (character >= 97 && character <= 122)) == FALSE)
			return NULL;

		// make all one case
	if (character >= 65 && character <= 90)
		character += 32;

	if (character >= 97 && character <= 122)
		{
		index = ((int) character) - 97;
		result = &LetterSprites[index];
		}
	else
		{
		index = ((int) character) - 48;
		result = &NumberSprites[index];
		}

	return result;
}





void RegisterTextStringForDisplay (char* string, int x, int y)
{
	int i, id = -1;
	TextString* thisOne;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length == 0)
			{
			id = i;
			break;
			}
		}

	assert(id != -1);
	thisOne = &AllStrings[id];

	thisOne->x = x;
	thisOne->y = y;
	thisOne->length = strlen(string);
	assert(thisOne->length > 0);
	assert(thisOne->length < MAX_STRING_LENGTH);

	strcpy( thisOne->data, string);

	NumberOfStrings++;
	assert(NumberOfStrings < MAX_TEXT_STRINGS);
}
