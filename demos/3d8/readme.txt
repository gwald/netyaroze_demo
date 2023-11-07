3d demo



Multiple camera modes; 2d sprite world
used as textures wrapped onto 3d objects (here, a cube)


Change projection: see the visual effect of this,
also how it affects clipping; use L1 and L2 and Rpad



View distance from world origin: use Lright and Rpad to modify


ViewMoveTime: use Lleft and Rpad



Camera modes:

(1) Nice simple camera mode: six basic moves

(2) Moves are like (1), but not constrained to be full 90 degree
tilts: camera moves to any point and orientation on the
surface of a 3d sphere around the world origin
(change view distance from origin to change sphere's radius)

(3) Camera is local view of (invisible) spaceship;
spaceship moves by Rpad (six rotations) and L1/L2 for
forward and backward. Cockpit view.



Note: the sprite-world as texture for cube is not fully sorted,
need to abstract from logical 2d space to actual texture VRAM space,
but this isn't hard.




L Evans             May 97

