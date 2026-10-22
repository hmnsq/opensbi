/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_system.h>

স্থির পূর্ণসংখ্যা sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	যদি (funcid == SBI_EXT_SRST_RESET) {
		যদি ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ফেরত SBI_EINVAL;

		নির্বাচন (regs->a0) {
		ক্ষেত্র SBI_SRST_RESET_TYPE_SHUTDOWN:
		ক্ষেত্র SBI_SRST_RESET_TYPE_COLD_REBOOT:
		ক্ষেত্র SBI_SRST_RESET_TYPE_WARM_REBOOT:
			বিরতি;
		পূর্বনির্ধারিত:
			ফেরত SBI_EINVAL;
		}

		নির্বাচন (regs->a1) {
		ক্ষেত্র SBI_SRST_RESET_REASON_NONE:
		ক্ষেত্র SBI_SRST_RESET_REASON_SYSFAIL:
			বিরতি;
		পূর্বনির্ধারিত:
			ফেরত SBI_EINVAL;
		}

		যদি (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ফেরত SBI_ENOTSUPP;
}

স্থির bool srst_available(শূন্য)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	জন্য (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		যদি (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ফেরত true;
	}

	ফেরত false;
}

কাঠামো sbi_ecall_extension ecall_srst;

স্থির পূর্ণসংখ্যা sbi_ecall_srst_register_extensions(শূন্য)
{
	যদি (!srst_available())
		ফেরত 0;

	ফেরত sbi_ecall_register_extension(&ecall_srst);
}

কাঠামো sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
