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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಯದಿ (funcid == SBI_EXT_SRST_RESET) {
		ಯದಿ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

		ವಿಕಲ್ಪ (regs->a0) {
		ಪಕ್ಷ SBI_SRST_RESET_TYPE_SHUTDOWN:
		ಪಕ್ಷ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ಪಕ್ಷ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		}

		ವಿಕಲ್ಪ (regs->a1) {
		ಪಕ್ಷ SBI_SRST_RESET_REASON_NONE:
		ಪಕ್ಷ SBI_SRST_RESET_REASON_SYSFAIL:
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		}

		ಯದಿ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ bool srst_available(ಶೂನ್ಯ)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ಪ್ರತಿ (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		ಯದಿ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ಪ್ರತ್ಯಾಗಮ true;
	}

	ಪ್ರತ್ಯಾಗಮ false;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_srst;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_srst_register_extensions(ಶೂನ್ಯ)
{
	ಯದಿ (!srst_available())
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_srst);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
