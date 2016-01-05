
.data
ProcessArray:
	.skip 360 /* size of word * 3 process * number of registers r2 - r31 + priority */
	
.text
SwapContext: // arg1 is destination, arg2 is source of new process
	addi sp, sp, -8
	stw r8, 0(sp) // store this register on the stack
	stw r9, 4(sp)
	ldw r8, 8(sp)
	movia r9, ProcessArray
	addi r9, r9, r8
	
	ldw r8, 0(sp)
	
	stw r2, 0(r9)
	stw r3, 4(r9)
	stw r4, 8(r9)
	stw r5, 12(r9)
	stw r6, 16(r9)
	stw r7, 20(r9)
	stw r8, 24(r9)
	stw r10, 32(r9)
	stw r11, 36(r9)
	stw r12, 40(r9)
	stw r13, 44(r9)
	stw r14, 48(r9)
	stw r15, 52(r9)
	stw r16, 56(r9)
	stw r17, 60(r9)
	stw r18, 64(r9)
	stw r19, 68(r9)
	stw r20, 72(r9)
	stw r21, 76(r9)
	stw r22, 80(r9)
	stw r23, 84(r9)
	stw r24, 88(r9)
	stw r25, 92(r9)
	stw r26, 96(r9)
	stw r27, 100(r9)
	stw r28, 104(r9)
	stw r29, 108(r9)
	stw r30, 112(r9)
	stw r31, 116(r9)
	
	ldwio r9, 4(sp)
	addi sp, sp, 8
	stw r9, 28(r9)
	
	movia r1, ProcessArray
	ldw r8, 4(sp)
	add r1, r1, r8
	
	ldw r2, 0(r1)
	ldw r3, 4(r1)
	ldw r4, 8(r1)
	ldw r5, 12(r1)
	ldw r6, 16(r1)
	ldw r7, 20(r1)
	ldw r8, 24(r1)
	ldw r10, 32(r1)
	ldw r11, 36(r1)
	ldw r12, 40(r1)
	ldw r13, 44(r1)
	ldw r14, 48(r1)
	ldw r15, 52(r1)
	ldw r16, 56(r1)
	ldw r17, 60(r1)
	ldw r18, 64(r1)
	ldw r19, 68(r1)
	ldw r20, 72(r1)
	ldw r21, 76(r1)
	ldw r22, 80(r1)
	ldw r23, 84(r1)
	ldw r24, 88(r1)
	ldw r25, 92(r1)
	ldw r26, 96(r1)
	ldw r27, 100(r1)
	ldw r28, 104(r1)
	ldw r29, 108(r1)
	ldw r30, 112(r1)
	ldw r31, 116(r1)
	
	ret