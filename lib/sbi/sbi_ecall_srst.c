/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_system.h>

સ્થિર પૂર્ણાંક sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	જો (funcid == SBI_EXT_SRST_RESET) {
		જો ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			પાછા SBI_EINVAL;

		પસંદગી (regs->a0) {
		સ્થિતિ SBI_SRST_RESET_TYPE_SHUTDOWN:
		સ્થિતિ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		સ્થિતિ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			તોડો;
		મૂળભૂત:
			પાછા SBI_EINVAL;
		}

		પસંદગી (regs->a1) {
		સ્થિતિ SBI_SRST_RESET_REASON_NONE:
		સ્થિતિ SBI_SRST_RESET_REASON_SYSFAIL:
			તોડો;
		મૂળભૂત:
			પાછા SBI_EINVAL;
		}

		જો (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	પાછા SBI_ENOTSUPP;
}

સ્થિર bool srst_available(શૂન્ય)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	માટે (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		જો (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			પાછા true;
	}

	પાછા false;
}

સંરચના sbi_ecall_extension ecall_srst;

સ્થિર પૂર્ણાંક sbi_ecall_srst_register_extensions(શૂન્ય)
{
	જો (!srst_available())
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_srst);
}

સંરચના sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
