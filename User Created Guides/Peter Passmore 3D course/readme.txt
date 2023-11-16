I have designed this introductory tutorial as a first tutorial for a 3rd year games course 
I am running in February 1998. Whilst navigating my way through the PSX documentation, the 
demo programs, and the news groups I often wished (like I suspect many Yaroze newcomers do!) 
that I had a tutorial like this which would just tell me the bits I needed to get me 
programming in 3D from the bottom up rather that struggling with incomprehensible 
functions and sophisticated but large example programs.So I hope this will be of 
use to others of you out there and I only ask that you protect my copyright to this 
document, and if you use it for teaching that you let me know.The document is quite long
 and in a draft state but tells much of what you need to know to get going in 3D. It does 
not deal with sprites at all, which will be the subject of a further tutorial I may make 
available here but note that Ira Rainey has already are some good tutorials on 2D which 
can be found at ~shadow/ftp on the (SCEE) Yaroze server.

--------------------------------
UPDATE 1/12/97#defined functions now bracketed to be ANSI C (& Code warrior compatible) - 
thanks to US Yarozer Matthew Hulett for pointing this out.

NTSC users note that the video mode should be set to NTSC rather than PAL and the screen 
width and height set to an appropriate NTSC mode (eg: 320x240).Step 3 now includes an 
updated main function in the text, and function prototypes.

--------------------------------
UPDATE 9/12/97

Variable matTmp defined as a matrix instead of a pointer to a matrix in functions 
AdvanceModel and RotateModel. Thanks to Craig Graham and Stefano Provenzano for 
catching this one.

Tutorial programs have been updated accordingly.
--------------------------------

I welcome your feedback and (constructive!) criticism so if you wish to contact me, 
email me at p.passmore@mdx.ac.uk.