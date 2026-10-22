/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_system.h>

ساکن صحیح sbi_ecall_srst_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	اگر (funcid == SBI_EXT_SRST_RESET) {
		اگر ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			واپس SBI_EINVAL;

		انتخاب (regs->a0) {
		حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
			توڑو;
		طےشدہ:
			واپس SBI_EINVAL;
		}

		انتخاب (regs->a1) {
		حالت SBI_SRST_RESET_REASON_NONE:
		حالت SBI_SRST_RESET_REASON_SYSFAIL:
			توڑو;
		طےشدہ:
			واپس SBI_EINVAL;
		}

		اگر (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	واپس SBI_ENOTSUPP;
}

ساکن bool srst_دستیاب(خالی)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	برائے (type = 0; type <= SBI_SRST_RESET_TYPE_آخری; type++) {
		اگر (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			واپس true;
	}

	واپس false;
}

ساخت sbi_ecall_extension ecall_srst;

ساکن صحیح sbi_ecall_srst_رجسٹر_extensions(خالی)
{
	اگر (!srst_دستیاب())
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_srst);
}

ساخت sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_آغاز		= SBI_EXT_SRST,
	.extid_انتہا		= SBI_EXT_SRST,
	.رجسٹر_extensions	= sbi_ecall_srst_رجسٹر_extensions,
	.handle			= sbi_ecall_srst_handler,
};
