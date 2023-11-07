/*******************************************************
*   Copyright (C) 1997 by Sony Computer Entertainment  *
*                  All Rights Reserved                 *
*               Stuart Ashley 8th May '97              *
*******************************************************/

/**************************
*** example of morphing ***
**************************/

Controls
--------

Quit - Select + Start

Aim
---

To illustrate run time manipulation of TMDs.

Overview Of Construction
------------------------

A TMD is initially created using one of the predefined meshes. From this the
vertices are altered to another mesh.
        In order to keep down calculation time down a TMD is created with
multiple polygons, rather than a number of single poly TMDs being used.
