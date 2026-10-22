// SPDX-License-Identifier: BSD-2-Clause
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_system.h>

ایستا صحیح sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	صحیح ret = SBI_ENOTSUPP;

	اگر (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	اگر (ret >= 0) {
		out->مقدار = ret;
		ret = 0;
	}

	بازگشت ret;
}

ایستا bool susp_available(تهی)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	برای (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		اگر (sbi_system_suspend_supported(type))
			بازگشت true;
	}

	بازگشت false;
}

ساختار sbi_ecall_extension ecall_susp;

ایستا صحیح sbi_ecall_susp_register_extensions(تهی)
{
	اگر (!susp_available())
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_susp);
}

ساختار sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
