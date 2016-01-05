

.text

.section .exceptions, "ax"
IHANDLER:
	rdctl r3,ctl4
	andi r3, r3, 0x1
	bne r3, r0, LightLED 
	br Done
	LightLED:
		movia r8, 0x1
		movia r7, 0xFF200000
		stwio r8, 0(r7)
	Done:
		movia r7, 0xFF202000
		stwio r0, 0(r7)
		subi ea, ea, 4
		eret

.global _start
_start:
	movia r7, 0xFF200000
	stwio r0, 0(r7)

	movia r7, 0xFF202000		# r7 contains the base address for the timer
	movui r2, %hi(200000000)
	movui r3, %lo(200000000)
	stwio r3, 8(r7)				# store into lower bits
	stwio r2, 12(r7)			# store into higher bits

	movi r2, 0x1
	wrctl ctl0, r2				# enable interupts
	wrctl ctl3, r2				# enable timer interupt line0
	
	
	movui r2, 5
	stwio r2, 4(r7)				# Start the timer without continuing with interupt enabled
	



  
