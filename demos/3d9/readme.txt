3d demo          Mirror surface using dynamic texturing
                and offscreen drawing



outer cube: made out of 16bit dynamically created textures.

The spaceship and the small cubes are simple TMDs.

The mirror is one large polygon, taking its texture
as the 256by256 region whose topleft is 640,256.

Offscreen drawing renders a second view of the world
(the one in the mirror) into that texture area,
hence the mirror.

See DrawMirrorViewOfWorld for mirror mechanics;
NOTE that the present mirror relies on using GsRVIEW2
in an odd way.

Basically, what is seen in a mirror is viewer-dependent,
to find the position and matrix of the mirror-view (for the
render onto the mirror texture area),
we use vector resolution of the main view direction
w.r.t the mirror's plane, thereby find the position of the
mirror-view; the twist in orientation is less easy,
at present it is done by forcing the reference point to
always stay at the centre of the mirror and just moving
the viewpoint in RVIEW2; for more general mirrors,
proper matrix rotation around the view-vector-to-mirror-plane
angle (doubled) is required.





L Evans             May 97


