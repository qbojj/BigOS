.section .init

.global _start
.extern _start_c

_start:
	.option push
	.option norelax
	lla   gp, __global_pointer$
	.option pop
	lla   sp, __stack_start
	j     _start_c
