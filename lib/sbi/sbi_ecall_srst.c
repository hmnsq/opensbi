/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_system.h>

স্থির পূর্ণাঙ্ক sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	যদি (funcid == SBI_EXT_SRST_RESET) {
		যদি ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ঘূৰাই SBI_EINVAL;

		ৱিকল্প (regs->a0) {
		পক্ষ SBI_SRST_RESET_TYPE_SHUTDOWN:
		পক্ষ SBI_SRST_RESET_TYPE_COLD_REBOOT:
		পক্ষ SBI_SRST_RESET_TYPE_WARM_REBOOT:
			ভঙ্গ;
		স্ৱভাৱ:
			ঘূৰাই SBI_EINVAL;
		}

		ৱিকল্প (regs->a1) {
		পক্ষ SBI_SRST_RESET_REASON_NONE:
		পক্ষ SBI_SRST_RESET_REASON_SYSFAIL:
			ভঙ্গ;
		স্ৱভাৱ:
			ঘূৰাই SBI_EINVAL;
		}

		যদি (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ঘূৰাই SBI_ENOTSUPP;
}

স্থির bool srst_available(শূন্য)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	বাবে (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		যদি (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ঘূৰাই true;
	}

	ঘূৰাই false;
}

সংরচনা sbi_ecall_extension ecall_srst;

স্থির পূর্ণাঙ্ক sbi_ecall_srst_register_extensions(শূন্য)
{
	যদি (!srst_available())
		ঘূৰাই 0;

	ঘূৰাই sbi_ecall_register_extension(&ecall_srst);
}

সংরচনা sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
