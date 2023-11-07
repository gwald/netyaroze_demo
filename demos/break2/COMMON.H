


#define TRUE 1
#define FALSE 0





void InitGsSprite (GsSPRITE* sprite);

void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);
void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);
void SortSpriteCentreToRealCentre (GsSPRITE *sprite);
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);