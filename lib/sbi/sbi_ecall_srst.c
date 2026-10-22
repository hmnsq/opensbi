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

ایستا صحیح sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	اگر (funcid == SBI_EXT_SRST_RESET) {
		اگر ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			بازگشت SBI_EINVAL;

		انتخاب (regs->a0) {
		حالت SBI_SRST_RESET_TYPE_SHUTDOWN:
		حالت SBI_SRST_RESET_TYPE_COLD_REBOOT:
		حالت SBI_SRST_RESET_TYPE_WARM_REBOOT:
			بشکن;
		پیشفرض:
			بازگشت SBI_EINVAL;
		}

		انتخاب (regs->a1) {
		حالت SBI_SRST_RESET_REASON_NONE:
		حالت SBI_SRST_RESET_REASON_SYSFAIL:
			بشکن;
		پیشفرض:
			بازگشت SBI_EINVAL;
		}

		اگر (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	بازگشت SBI_ENOTSUPP;
}

ایستا bool srst_available(تهی)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	برای (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		اگر (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			بازگشت true;
	}

	بازگشت false;
}

ساختار sbi_ecall_extension ecall_srst;

ایستا صحیح sbi_ecall_srst_register_extensions(تهی)
{
	اگر (!srst_available())
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_srst);
}

ساختار sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
