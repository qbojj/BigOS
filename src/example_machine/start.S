.section .init

.global _start

_start:
	.option push
	.option norelax
	lla   gp, __global_pointer$
	.option pop
	lla   sp, __stack_top
	j     start
