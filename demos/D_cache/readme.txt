


Blur demo

Author: Colin Hughes, based on Jim Pitts' demo


Shows
(1) how to use assembler with GNU
(2) how to set the stack to succesfully use the D-Cache.



(1)	Using assembler:
	
	<plasma.s> is assembly language, the <makefile> shows how
	GNU compiles it into an object file for linking with others.

    Assembly can be invoked directly in C functions as follows:

	__asm__ volatile ("sw $29,(savesp)");	  // save stack pointer

(2) Data cache usage:

	the R3000 chip (PlayStation CPU chip) has a 1K scratchpad buffer
	for super-fast access; it's called the D-cache, and perhaps
	its most common use is in optimisation, for speeding up
	stack reads and writes.

	To put your program's stack onto the D-cache :-

	(i) ensure that it is 1K at most

	(ii) rename the function <main> as <submain>,
		create a new <main> function: this will 
		set the new stack pointer, invoke <submain>, reset the stack
		and quit.

	(iii) actually setting the stack is done via assembly
		instructions. 
		
