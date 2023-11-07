$PSDocId: Document Release 1.0 for Run-time Library Release 4.0$

PS-X Demonstration Program "todview"
==================================

* todview is a sample program that displays animation contained in a single TOD file. The TOD filename and the TMD filename to be used are written in the makefile.

The following operations are performed in the program:
viewpoint movement 
reading and rendering of each frame of TOD data

* Execution
make
siocons auto

* Descriptions of files

main.c			main routine
te.h			TMD ID table

te.tmd			modelling data
jaanken.tod		TOD data

* The control pad operations are as follows:

arrow key (left):	viewpoint moves along the X axis in a positive direction
arrow key (right):  	viewpoint moves along the X axis in a negative direction.
arrow key (down):	viewpoint moves along the Y axis in a positive direction.
arrow key (up):		viewpoint moves along the Y axis in a negative direction
L1 button:		viewpoint moves along the Z axis in a positive direction
L2 button:		viewpoint moves along the Z axis in a negative direction
circle button, X button, square button, triangle button:
			playback animation
START button:		returns viewpoint to initial position
SELECT button:	        exit


Copyright (C) 1994,1995 by Sony Computer Entertainment
All Rights Reserved

