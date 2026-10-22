/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_system.h>

ساكن صحيح sbi_ecall_srst_handler(بنانشانی لما extid, بنانشانی لما funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *باہرلا)
{
	إذا (funcid == SBI_EXT_SRST_RESET) {
		إذا ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			أرجع SBI_EINVAL;

		بدل (regs->a0) {
		حالة SBI_SRST_RESET_TYPE_SHUTDOWN:
		حالة SBI_SRST_RESET_TYPE_COLD_REBOOT:
		حالة SBI_SRST_RESET_TYPE_WARM_REBOOT:
			اكسر;
		افتراضي:
			أرجع SBI_EINVAL;
		}

		بدل (regs->a1) {
		حالة SBI_SRST_RESET_REASON_NONE:
		حالة SBI_SRST_RESET_REASON_SYSFAIL:
			اكسر;
		افتراضي:
			أرجع SBI_EINVAL;
		}

		إذا (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	أرجع SBI_ENOTSUPP;
}

ساكن bool srst_دستیاب(فراغ)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	لكل (type = 0; type <= SBI_SRST_RESET_TYPE_آخری; type++) {
		إذا (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			أرجع true;
	}

	أرجع false;
}

بنية sbi_ecall_extension ecall_srst;

ساكن صحيح sbi_ecall_srst_رجسٹر_extensions(فراغ)
{
	إذا (!srst_دستیاب())
		أرجع 0;

	أرجع sbi_ecall_رجسٹر_extension(&ecall_srst);
}

بنية sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_شروع		= SBI_EXT_SRST,
	.extid_سرا		= SBI_EXT_SRST,
	.رجسٹر_extensions	= sbi_ecall_srst_رجسٹر_extensions,
	.handle			= sbi_ecall_srst_handler,
};
