/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_system.h>

நிலையான முழு sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	எனில் (funcid == SBI_EXT_SRST_RESET) {
		எனில் ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			திரும்பு SBI_EINVAL;

		தேர்வு (regs->a0) {
		நிலை SBI_SRST_RESET_TYPE_SHUTDOWN:
		நிலை SBI_SRST_RESET_TYPE_COLD_REBOOT:
		நிலை SBI_SRST_RESET_TYPE_WARM_REBOOT:
			நிறுத்து;
		இயல்பு:
			திரும்பு SBI_EINVAL;
		}

		தேர்வு (regs->a1) {
		நிலை SBI_SRST_RESET_REASON_NONE:
		நிலை SBI_SRST_RESET_REASON_SYSFAIL:
			நிறுத்து;
		இயல்பு:
			திரும்பு SBI_EINVAL;
		}

		எனில் (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	திரும்பு SBI_ENOTSUPP;
}

நிலையான bool srst_available(வெறுமை)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	க்கு (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		எனில் (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			திரும்பு true;
	}

	திரும்பு false;
}

கட்டமைப்பு sbi_ecall_extension ecall_srst;

நிலையான முழு sbi_ecall_srst_register_extensions(வெறுமை)
{
	எனில் (!srst_available())
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_srst);
}

கட்டமைப்பு sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
