.text

.global kernel_trap_entry

.equ SSTATUS_SIE, 0x00000002 // Supervisor Interrupt Enable
.equ SSTATUS_SPP, 0x00000100 // Supervisor Previous Privilege (1 = S-mode, 0 = U-mode)
.equ FRAME_SIZE, 0x120

// sscratch should have kernel stack pointer IIF in user-mode
// when comming from kernel-space then it should be 0

// the thread pointer should be at the base of the sscratch location if it is not 0

kernel_trap_entry:
	// Swap sscratch with sp IIF we come from user - mode
	csrrw sp, sscratch, sp
	bnez sp, 1f
	csrrw sp, sscratch, sp
1:  // sp is now kernel stack pointer
	addi sp, sp, -FRAME_SIZE
	sd x1, 1*8(sp)
	csrrw x1, sscratch, zero  // restore invariants and bring original sp to x1
	sd x1, 2*8(sp)

	// Save everything, but ra (x1), sp (x2)
	.irp i, 0,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		sd x\i, \i*8(sp)
	.endr

	addi fp, sp, FRAME_SIZE

	csrr t0, sepc
	sd t0, 32*8(sp)

	csrr t1, sstatus
	sd t1, 33*8(sp)
	andi t1, t1, SSTATUS_SPP
	bnez t1, 2f

.option push
.option norelax
	lla gp, __global_pointer$
.option pop

	ld tp, (fp)
2:

	csrr t2, scause
	sd t2, 34*8(sp)

	csrr t3, stval
	sd t3, 35*8(sp)

	mv a0, sp
	call kernel_trap_handler

	// Disable interrupts to prevent corruption during restore
	csrc sstatus, SSTATUS_SIE

	ld t0, 32*8(sp)
	csrw sepc, t0

	ld t1, 33*8(sp)
	csrw sstatus, t1

	// Restore original sscratch
	andi t2, t1, SSTATUS_SPP
	bnez t2, 3f

	csrw sscratch, fp  // arm sscratch for user-mode
3:
	// restore all registers (sp at the end)
	.irp i, 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		ld x\i, \i*8(sp)
	.endr

	ld sp, 2*8(sp)
	sret
