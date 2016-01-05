.text

.section .exceptions, "ax"
IHANDLER:
	addi	sp,	sp, -16
 	stw		r2,	0(sp)
 	stw 	r3,	4(sp) /* save registers for main routine */
 	stw 	r4,	8(sp)
	stw 	r5,	12(sp)
	
	rdctl 	r3,	ctl4
	andi 	r3, r3, 0x1
	
	bne 	r3,	r0,	LightLED
	br exit_exception
	
LightLED:
	movia 	r8,	0x1
	movia 	r7, 0xFF200000
	stwio 	r8, 0(r7)


exit_exception:
	subi 	ea, ea, 4		
	eret