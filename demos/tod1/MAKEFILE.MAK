all:
	ccpsx -O -Xo$80010000 main.c todanim.c objctl.c -omain.cpe,main.sym,main.map

apd_save:
	ccpsx -O -Xo$80010000 -DAPD_SAVE main.c todanim.c objctl.c \
	\psx\lib\autopad.obj -otodviews.cpe

apd_load:
	ccpsx -O -Xo$80010000 -DAPD_LOAD main.c todanim.c objctl.c \
	\psx\lib\autopad.obj -otodviewl.cpe

for_menu:
	ccpsx -O -Xo$80010000 \psx\lib\none3.obj main.c todanim.c objctl.c -otodview.cpe
	cpe2x todview.cpe
	del   todview.cpe

load:
	pqbload te.tmd      800c0000
	pqbload jaanken.tod 800e0000
