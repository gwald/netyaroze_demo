                Follow My Camera

3d game: more dynamic texturing on larger scale,
using breakout as child 2d-drawing process.


                    Pad Controls:-



Menu screen:
    Rup/Rdown to go up and down,
    Rleft/Rright to choose option items


During demo:
        any key returns to menu


During game:
    <start> & <select> to return to menu

    <start> to pause

    When player controls camera:
        Lpad, L1 and L2 for six camera moves
        (Rup and Rdown for movement when in third basic camera mode)

    <select and R1 and R2> for screen snapshot

    <Rright, Rleft>: move the player's bat to keep ball in play





The breakout game draws onto off-screen buffer, used as texture
for polygons of inner cube/outer cuce/both.


Variable camera modes:-
    (1) five basic modes:
        simple swivelling
            (four moves, two twists, all 6 are discrete 90 degrees);
        proper swivelling (four moves, two twists, small amounts);
        local / cockpit view (camera attached to movable object
                        i.e. like spaceship-pilot view);
        diagonal: camera flips among the 8 diagonal vectors emanating
            from the corners of the cubes; also two twist of 120 degrees;
        circling: can circle in 9 planes (3 basic: xy, xz, yz, 6 diagonal
            planes), can flip direction of circling (clockwise - anticlock).

    (2) six auto camera modes:
        first four are fixed, one in each of the basic camera modes
        (except basic mode three);
        fifth perodically flips between four of five basic modes;
        sixth perodically flips between three of five basic modes.



L Evans                 May 97
