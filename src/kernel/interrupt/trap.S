.text

.global kernel_trap_entry
.global kernel_restore_with_cleanup
.global jump_with_stack

.equ SSTATUS_SIE, 0x00000002 // Supervisor Interrupt Enable
.equ SSTATUS_SPP, 0x00000100 // Supervisor Previous Privilege (1 = S-mode, 0 = U-mode)
.equ FRAME_SIZE, 0x120

// sscratch should have kernel stack pointer IIF in user-mode
// when comming from kernel-space then it should be 0

// the thread pointer should be at the base of the sscratch location if it is not 0

kernel_trap_entry:
	// Swap sscratch with sp IIF we come from user - mode
	csrrw sp, sscratch, sp
	bnez sp, from_umode

from_smode:
	csrrw sp, sscratch, zero
	add sp, sp, -FRAME_SIZE
	sd x1, 1*8(sp)
	add x1, sp, FRAME_SIZE
	sd x1, 2*8(sp)
	j save_regs

from_umode:
	add sp, sp, -FRAME_SIZE
	sd x1, 1*8(sp)
	csrrw x1, sscratch, zero
	sd x1, 2*8(sp)

save_regs:
	// Save everything, but ra (x1), sp (x2)
	.irp i, 0,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		sd x\i, \i*8(sp)
	.endr

	csrr t0, sepc
	csrr t1, sstatus
	csrr t2, scause
	csrr t3, stval

	sd t0, 32*8(sp)
	sd t1, 33*8(sp)
	sd t2, 34*8(sp)
	sd t3, 35*8(sp)

	andi t1, t1, SSTATUS_SPP
	bnez t1, run_handler

.option push
.option norelax
	lla gp, __global_pointer$
.option pop

	ld tp, FRAME_SIZE(sp)

run_handler:
    mv a0, sp

    // fixup the stack alignment (16-bytes)
    mv s0, sp
    and sp, sp, ~15
	call kernel_trap_handler
	mv sp, s0

kernel_trap_restore:
	.irp i, 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
		ld x\i, \i*8(sp)
	.endr

	ld x1, 33*8(sp)
	and x1, x1, ~SSTATUS_SIE // whatever was specified, disable interrupts
	csrw sstatus, x1

	and x1, x1, SSTATUS_SPP
	bnez x1, restore_smode

restore_umode:
	add x1, sp, FRAME_SIZE
	csrw sscratch, x1
	j restore_finalize

restore_smode:
	csrw sscratch, zero

restore_finalize:
	ld x1, 32*8(sp)
	csrw sepc, x1

	ld x1, 1*8(sp)
	ld sp, 2*8(sp)
	sret

kernel_restore_with_cleanup:
	mv sp, a0
	beqz a2, kernel_trap_restore
	lla ra, kernel_trap_restore
	mv a0, a1
	jr a2

jump_with_stack:
	mv sp, a0
	mv a0, a1
	jr a2
