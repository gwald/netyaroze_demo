// 
// sprite.h
// 
// Sprite loading & manipulation functions
// GNU Version
// 
// Andrew Murray - 9th October 2002
// 

#ifndef SPRITE_H
#define SPRITE_H

// image bit depths
#define FOURBITIMAGE (0x08)
#define EIGHTBITIMAGE (0x09)

// sprite attributes

// alter sprite RGB values on/off
#define RGB_ADJUST_ON (0 << 6)
#define RGB_ADJUST_OFF (1 << 6)

// sprite bit patterns
#define FOURBITCOLOUR (0 << 24)
#define EIGHTBITCOLOUR (1 << 24)
#define SIXTEENBITCOLOUR (1 << 25)

// rotation and scaling on/off
#define ROTSCALE_ON (0 << 27)
#define ROTSCALE_OFF (1 << 27)

// semi transparency rates
#define HALFBACK_PLUS_HALFFRONT (0 << 28) + (0 << 29)
#define FULLBACK_PLUS_FULLFRONT (1 << 28) + (0 << 29)
#define FULLBACK_MINUS_FULLFRONT (0 << 28) + (1 << 29)
#define FULLBACK_PLUS_HALFFRONT (1 << 28) + (1 << 29)

// semi transparency on/off
#define SEMITRANS_ON (1 << 30)
#define SEMITRANS_OFF (0 << 30)

// sprite display on/off
#define DISPLAY_ON (0 << 31)
#define DISPLAY_OFF (1 << 31)

#define BUFFERSIZE (100 * 1024)

// sprite related macros
#define setSpriteXY (s, _x, _y) \
					(s)->x = (_x), (s)->y = (_y)

#define setSpriteUV (s, _u, _v) \
					(s)->u = (_u), (s)->v = (_v)

#define setSpriteRGB (s, _r, _g, _b) \
					 (s)->r = (_r), \
					 (s)->g = (_g), \
					 (s)->b = (_b)
					 
#define setSpriteScale	(s, _sx, _sy) \
						(s)->scalex = (_sx) \
						(s)->scaley = (_sy)
						
// ----------------------------------------------
// Prototypes
// ----------------------------------------------

void	LoadSprite	(GsIMAGE *pImage, unsigned long* addr);
void 	InitSprite	(GsSPRITE *pSprite, GsIMAGE *pImage);

#endif

