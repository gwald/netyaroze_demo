/*
	Full assembly plasma thingy
	Based originally on code by Jim	Pitts
*/


#include <asm.h>		

#define table 0x8009	// High part of cos/sine table address

.text
.align	2
.globl	plasma_asm

.set noreorder
.set noat

// a0 is theta value..

plasma_asm:

	la t2,buffer
	or	v0,zero,238*4	// Y count

$outer:
	lui AT,table
	or AT,AT,v0
	lw a2,0(AT)			// Cossine[Y]

	lui a3,table
	sll AT,a0,2
	or a3,a3,AT			// &Cossine[theta]

	lw t0,0(a3)			// cossine[theta] (x=0)	

	srl a1,a2,16
	add a1,a1,a0
	add a1,a1,a0		// 2theta+cos[y]

	sll a2,a2,16
	srl a2,a2,16		// sin[y]

	or v1,zero,40		// X count

$inner:

	sll AT,t0,16		// sin[thetax]
	srl AT,AT,16
	sub AT,AT,a1		// - c1
	and AT,AT,256
	srl t1,AT,7			// First bit

	srl AT,t0,16		// cos[thetax]

	lw	t0,8(a3)		// Next cossine
	add AT,AT,a2		// + c2
	and AT,AT,256
	srl AT,AT,8			// 2nd bit
	or t1,t1,AT			// First pixel

	add	a1,a1,1			// Increase X for constants
	add a2,a2,1

	sll AT,t0,16		// sin[thetax]
	srl AT,AT,16
	sub AT,AT,a1		// - c1
	and AT,AT,256
	sll AT,AT,1			// First bit
	or t1,t1,AT

	srl AT,t0,16		// cos[thetax]

	lw	t0,16(a3)		// Next cossine
	add AT,AT,a2		// + c2
	and AT,AT,256		// 2nd bit
	or t1,t1,AT			// Second pixel

	add	a1,a1,1			// Increase X for constants
	add a2,a2,1

	sll AT,t0,16		// sin[thetax]
	srl AT,AT,16
	sub AT,AT,a1		// - c1
	and AT,AT,256
	sll AT,AT,9			// First bit
	or t1,t1,AT

	srl AT,t0,16		// cos[thetax]

	lw	t0,24(a3)		// Next cossine
	add AT,AT,a2		// + c2
	and AT,AT,256
	sll AT,AT,8			// 2nd bit
	or t1,t1,AT			// Third pixel

	add	a1,a1,1			// Increase X for constants
	add a2,a2,1

	sll AT,t0,16		// sin[thetax]
	srl AT,AT,16
	sub AT,AT,a1		// - c1
	and AT,AT,256
	sll AT,AT,17		// First bit
	or t1,t1,AT

	srl AT,t0,16		// cos[thetax]

	lw	t0,32(a3)		// Next cossine (actually for next loop)
	add AT,AT,a2		// + c2
	and AT,AT,256
	sll AT,AT,16		// 2nd bit
	or t1,t1,AT			// Fourth pixel

	sw t1,(t2)			// Store in buffer
	add t2,t2,4

	add	a1,a1,1			// Increase X for constants
	add a2,a2,1

	sub	v1,v1,1
	bne	v1,zero,$inner
	add	a3,a3,32		// DELAY slot..executes before jump

	bne v0,zero,$outer
	sub v0,v0,8			// DELAY slot..executes before jump BUT after test!!

	jr	ra
	nop