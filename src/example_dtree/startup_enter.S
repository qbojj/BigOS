.section .init.enter, "ax"
.global _enter
_enter:
    .option push
    .option norelax
    la    gp, __global_pointer$
    .option pop
    la    sp, _sp
    jal   zero, _start
