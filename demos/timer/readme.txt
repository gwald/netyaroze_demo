Timing demo


Basic ploy:


space shuttle on screen, by pressing Rup and Rdown, user
controls the number of them. More than three will cause the PSX
to start dropping frames.

The fire sprite in the foreground is non-interactive:
its purpose is to show that a process can be made to run
at fairly constant real-time rate, although the game fps is variable.

Program measures its own hsyncs, stores them and once in a while
finds the average; hence it predicts its own frame rate and
hence can make (eg sprites) move in a manner that should
preserve real-time constant speed across changes in game frame rate.

Note that this applies to all game processes (game logic, AI processing,
object processes, etc) as well as to sprite-movement.

Note: initially I thought that adding VSync(0)s to a main loop
that was already taking a long time might help re-synch and improve
its frame rate, but this is untrue.


L Evans
