#ifndef _STDBIGOS_SBI_H_
#define _STDBIGOS_SBI_H_

#include "types.h"

#define SBI_SPEC_VERSION_DEFAULT	 0x1
#define SBI_SPEC_VERSION_MAJOR_SHIFT 24
#define SBI_SPEC_VERSION_MAJOR_MASK	 0x7f
#define SBI_SPEC_VERSION_MINOR_MASK	 0xffffff

typedef enum : ireg_t {
	SbiSuccess = 0,
	SbiErrFailure = -1,
	SbiErrNotSupported = -2,
	SbiErrInvalidParam = -3,
	SbiErrDenied = -4,
	SbiErrInvalidAddress = -5,
	SbiErrAlreadyAvailable = -6,
	SbiErrAlreadyStarted = -7,
	SbiErrAlreadyStopped = -8,
} SbiError;

typedef enum : ireg_t {
	SbiExtBase = 0x10,
	SbiExtSystemReset = 0x53525354,	 // 'SRST' in ASCII
	SbiExtTime = 0x54494D45,		 // 'TIME' in ASCII
	SbiExtIpi = 0x495049,			 // 'IPI' in ASCII
	SbiExtShutdown = 0x53554844,	 // 'SHUT' in ASCII
	SbiExtHsm = 0x48534D,			 // 'HSM' in ASCII
	SbiExtPmu = 0x504D55,			 // 'PMU' in ASCII
	SbiExtFp = 0x46500000,			 // 'FP' in ASCII
	SbiExtMessageProxy = 0x4D505859, // 'MPXY' in ASCII
} SbiExtensionId;

typedef struct {
	SbiError error;
	reg_t value;
} sbiret;

sbiret sbi_ecall(SbiExtensionId ext, ireg_t fid, reg_t arg0, reg_t arg1,
				 reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5);

// Base Extension

sbiret sbi_get_spec_version();
sbiret sbi_get_impl_id();
sbiret sbi_get_impl_version();
sbiret sbi_probe_extension(SbiExtensionId);
sbiret sbi_get_mvendorid();
sbiret sbi_get_marchid();
sbiret sbi_get_mimpid();

// ...

#endif
