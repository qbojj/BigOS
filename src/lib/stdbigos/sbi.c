#include "sbi.h"

sbiret sbi_ecall(SbiExtensionId ext, ireg_t fid, ireg_t arg0, ireg_t arg1,
				 ireg_t arg2, ireg_t arg3, ireg_t arg4, ireg_t arg5) {
	register long a0 __asm__("a0") = arg0;
	register long a1 __asm__("a1") = arg1;
	register long a2 __asm__("a2") = arg2;
	register long a3 __asm__("a3") = arg3;
	register long a4 __asm__("a4") = arg4;
	register long a5 __asm__("a5") = arg5;
	register long a6 __asm__("a6") = fid;
	register long a7 __asm__("a7") = ext;
	__asm__ volatile("ecall"
					 : "+r"(a0), "+r"(a1)
					 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
					 : "memory");
	sbiret ret;
	ret.error = a0;
	ret.value = a1;
	return ret;
}

bool guest_sbi_probe_extension(SbiExtensionId extid, ireg_t* out_val) {
	sbiret ret = sbi_ecall(SbiExtBase, SbiProbeExtension, extid, 0, 0, 0, 0, 0);

	if(out_val) { *out_val = ret.value; }

	return ret.error == SbiSuccess && ret.value;
}
