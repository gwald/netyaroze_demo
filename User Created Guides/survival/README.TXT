-------------------------------------------------------------------------------
Copyright (C) 1993-1997 by Sony Computer Entertainment, Inc.
All rights Reserved.
-------------------------------------------------------------------------------
                     A 3D Demo for Net Yaroze PlayStation
					 ------------------------------------

This example demonstrate the 3D capabilities of the PlayStation.  This sample
uses the a free flying view point which tracks the player object in the 3D
space.  The source code is provided and care has been taken to break down the
source code to make libraries of various nature.

The name convention followed is LibName.C and a LibName.H to go along with it.
Each of these libraries contain source code and data which is transparent from
the rest of the source code and can be easily used in other application with
minimal changes, and sometimes no changes. I have tried to put as much comments
as possible for now.  The usage for each of these libraries is defined either
at the begining of the .C or the .H file.  For a better understanding of the
usage you should check out the game play code where these libraries are used.

LibMain.H
---------
  
  Contains #include statements for all system include files and some
  generic #define macros which are used through out this sample source.
  
LibData.H
---------
  
  Contains the macros for the main memory adresses where model,	texture
  and sound effects are loaded.
  
LibTmd.H
--------
  
  Contains data structures for all primitives in a .TMD file.  This file
  also contains data structures used by LibXplod to create individual TMDs
  created for each particle.
  
LibXplod.C with LibXplod.h
--------------------------
  
  This library provides the use of particle explosion using the PlayStation
  library routines.  This file also contains routines to do a reverse particle
  explosion where all the particles of an exploded object come back in an
  animated fashion to regenerate the object.  The effect can be  seen whenever
  the player object dies (particle explosion happens) and then it comes back
  to life (reverse particle explosion happens).  This library used LibMath for
  sin/cos functions. This library can be used as is with no modifications.
  
LibMath.C with LibMath.H
------------------------
  
  This file contains some generic math routines which are used by this sample.
  This file can be used as is with no modifications.  Whereas, if you add any
  more routines to this file please post it back up so that others can use it.
  
LibPad.C with LibPad.H
----------------------
  
  This file contains routines/data related to the controller status read. This
  file can me used as is with no modifications.
  
LibSound.C with LibSound.H
--------------------------
  
  This file contains routines/data for handling sound effects and game music.
  The library is transparent to the rest of the source code except for the
  memory addresses where the sound effects and music data is loaded.  Please
  look into these files to find out the details anout it's usage.  This library
  can be used as is with no modifications.
  
LibObjc.C with LibObjc.H
------------------------
  
  This file contains data structure definitions and functions used to handle
  various objects in the world.  The routines are very generic and they work
  on the OBJECT data structure.  These routines for initializing objects,
  settings their rotation matrix, drawing (sorting them into the ordering table),
  collision detection with other objects and walls, handling the positin FIFO
  to handle their state and for taking care of automatic polygon division. These
  functions are used by the main game play routines in main.c.
  
Main.C
------
  
  This file contains the game play code.  A generic object data structure is
  used to handle various objects in the world.  All routines are implemented
  areound this data structure.  Care has been taken to make the source code
  modular to perform a generic operation.  Some of the object handling routines
  may look very big but a closer look will show you their modularity in terms of
  the function they are performing.
  
  The contents of this routines are divided into sections devided by dashed lines.
  These sections are created according to the generality of the functions they
  perform.
  
  The first section contains routines to initializes system and the PlayStation
  library. 	This part also contains the code for initializing various models
  and texture maps used in this sample.
  
  The second section contains routines to initialize various objects in the world.
  
  The third section contains routine to initialize the view point.
  
  The fourth section contains the main routines which initializes system library,
  other local libraries, generic data used in this sample and the main game play
  handling loop.
  
  The fifth section contains routines to handle the player object.
  
  The sixth section contains routines to handle the enemy objects.
  
  The seventh section contains routines to handle the enemy remains.
  
  The eigth section contains routine to handle the view point.
  
  The ninth section contains routines to set/clear automatic polygon flag of
  various object's in the world.
  
  The final section contains to prepare the display.
  
Data Files
----------
  
  Cube.TMD with Cube.TIM
  Slab.TMD with Slab.TIM
  Robo.TMD with Robo1.TIM, Robo2.TIM and Prop.TMD
  Ship.TMD with Ship.TIM
  Ball.TMD
  Sfx.SEQ with Sfx.VH and Sfx.VB
  
MAKEFILE
--------
  
  To build the executable.
  
AUTO
----
  
  An auto file to be used in siocons auto load function.  This file contains
  commands to load all data files into designated memory area.  This file
  works in conjunction with the LibData.H.  In this sample, I have tried
  to load the data in the lower memory area just after the PlayStation runtime
  library.  The source code gets loaded after this data area.  Even though
  the .TIM area and the Sfx.VB area are useless as soon as the initialization
  is complete (.TIM gets loaded into VRAM and .VB gets loaded into SPU RAM),
  I do not re-use this area.  This allows me to load only the executable
  during the code development process.
  
RunDemo.BAT
-----------
  
  This file is an independent MS-DOS batch file which uses some of the utility
  function provided with siocons to load and execute the sample from MS-DOS
  prompt.  Just type RunDemo at the dos prompt and the batch file will build
  the executable, load all data files, load the executable and execute it.

-------------------------------------------------------------------------------

How to run this demo ?
----------------------
  
  1. Run make - will build the executable "main"
  2. Run siocons - will start the console program for the PlayStation
  3. Press F3 for auto load - type "auto" at the prompt and hit Enter -
     will load the data files into main memory.
  4. Press F4 for executable load - type "main" at the prompt and hit Enter -
     will load the executable into main memory.
  5. Type GO at the prompt.  This will start the program on the PlayStation.
  6. Enjoy.
  
-------------------------------------------------------------------------------

Game Control
------------

Controller #I - to control the player object.
  
  Press
    Left to turn left.
	Right to turn right.
	Up to go forward.
	Down to slow down/go backward.
  Tap
    X continuously to make the player object fly.

  Press
    Select+Start together to quit to the development console.
  
Controller #II - to change various parameters
  
  Tap on Start to set the ShowDebug flag on/off.  This will show some
  debugging info on the screen.
  
  Left Pad, L1/R1 and L2/R2 buttons in conjunction with the right pad
  buttons change various lighting/abbient related variables and the
  position/inclination of the camera wrt the player object.  Please
  look into the view point and the light source handling routines in
  main.c for the details about these control.
  
-------------------------------------------------------------------------------

If the user does not do anything for 30 seconds, a credit screen will come
up.  Please look into DraeDebug() routines in main.c for this code.

-------------------------------------------------------------------------------
  
I will try and provide updates to this sample from time to time.  I hope
this sample will turn out to be useful to you guys and will get you started
on your development process.  Please feel free to contact us on the Net Yaroze
site in US for any further questions.

Also, please do let me know if you have any suggestions.


Pradip K Fatehpuria
Senior Software Development Engineer
Sony Computer Entertainment

-------------------------------------------------------------------------------
