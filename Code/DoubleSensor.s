
.data

.equ RED_LEDS, 0xFF200000
.equ TIMER, 0xFF202000
.equ LEGO, 0xFF200060
.equ DIRECTION, 0x07F557FF


/* Sensor Mappings go (enable, ready) 
 * Sensor 1 - (10, 11) r6
 * Sensor 2 - (12, 13) r7
 * Sensor 3 - (14, 15)
 * Sensor 4 - (16, 17)
 * Sensor 5 - (18, 19)
 */
.global _start

.text

_start:
	mov r6, r0 /* Sensor 1 */
	mov r7, r0 /* Sensor 2 */
	movia r8, LEGO
	movia r10, DIRECTION
	movia r12, RED_LEDS
	
	stwio r10, 4(r8)	/* Store direction register into lego to initiate */

	loop:
		movia r18, 0xFFFFFFFF /* r18 will be used to flip the bits of any register with an xor */
		
		CheckSensor1:
			movia r11, 1 
			slli r11, r11, 10 /* r11 now has the bit set for sensor 1 */
			xor r11, r11, r18 /* invert the bits since that's how the lego controller likes it */
			stwio r11, 0(r8)
			ldwio r5, 0(r8)
			srli r5, r5, 11
			andi r5, r5, 0x1 /* Read from ready to make sure sensor has valid data */
			beq r0, r5, CheckSensor1
			
			ldwio r6, 0(r8)
			srli r6, r6, 27
			andi r6, r6, 0x0f	/* Get only the first byte of sensor 1 data */
			subi r6, r6, 3 /* Subtract 3 to normalize hand over top of sensor to 0 */
		
		CheckSensor2:
			movia r11, 1 /* Same deal for sensor 2 as was done with sensor 1 */
			slli r11, r11, 12
			xor r11, r11, r18
			stwio r11, 0(r8)
			ldwio r5, 0(r8)
			srli r5, r5, 13
			andi r5, r5, 0x1
			beq r0, r5, CheckSensor2
			
			ldwio r7, 0(r8)
			srli r7, r7, 27
			andi r7, r7, 0x0f
			subi r7, r7, 3

		DoneCheckingSensors:

	writeLED:
		movia r16, 0x1
		sll r16, r16, r6 /* Take sensor 1 and shift a 1 by that many bytes
		                  * This will result in the distance of the hand over the sensor
                          * displayed as powers of 2 to show the LED's moving						  
						  */
		
		movia r17, 0x1 /* do the same for the second sensor */
		sll r17, r17, r7
		
		or r16, r16, r17 /* or the two sensors together so we can observe both of them in action */
		stwio r16, 0(r12) /* write it all to LED's so we can see the changes in height correspond with LED light*/
		
		br loop

done:
	br done