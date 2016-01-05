
.data

.equ RED_LEDS, 0xFF200000
.equ TIMER, 0xFF202000
.equ LEGO, 0xFF200060
.equ DIRECTION, 0x07F557FF

.global _start

.text

_start:
movia r8, LEGO
movia r10, DIRECTION
movia r12, RED_LEDS
stwio r10, 4(r8)

loop:
	movia r11, 1
	slli r11, r11, 10
	movia r18, 0xFFFFFFFF
	xor r11, r11, r18
	stwio r11, 0(r8)
	ldwio r5, 0(r8)
	srli r5, r5, 11
	andi r5, r5, 0x1
	bne r0, r5, loop

good:
	ldwio r5, 0(r8)
	srli r5, r5, 27
	andi r5, r5, 0x0f
	
	subi r5, r5, 3
	
	movia r6, 0x1
	sll r6, r6, r5
	stwio r6, 0(r12)

	br loop

done:
	br done