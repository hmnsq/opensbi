/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_srst_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಆದರೆ (funcid == SBI_EXT_SRST_RESET) {
		ಆದರೆ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;

		ಆಯ್ಕೆ (regs->a0) {
		ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಸ್ಥಿತಿ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}

		ಆಯ್ಕೆ (regs->a1) {
		ಸ್ಥಿತಿ SBI_SRST_RESET_REASON_NONE:
		ಸ್ಥಿತಿ SBI_SRST_RESET_REASON_SYSFAIL:
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		}

		ಆದರೆ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ bool srst_ಲಭ್ಯ(ಶೂನ್ಯ)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ಗಾಗಿ (type = 0; type <= SBI_SRST_RESET_TYPE_ಕೊನೆಯ; type++) {
		ಆದರೆ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ಹಿಂತಿರುಗಿ true;
	}

	ಹಿಂತಿರುಗಿ false;
}

ರಚನೆ sbi_ecall_extension ecall_srst;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_srst_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (!srst_ಲಭ್ಯ())
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_srst);
}

ರಚನೆ sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_ಆರಂಭ		= SBI_EXT_SRST,
	.extid_ಕೊನೆ		= SBI_EXT_SRST,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_srst_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_srst_handler,
};
