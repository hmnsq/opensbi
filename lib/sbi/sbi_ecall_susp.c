// SPDX-License-Identifier: BSD-2-Clause
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_system.h>

ساکن صحیح sbi_ecall_susp_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = SBI_ENOTSUPP;

	اگر (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	اگر (ret >= 0) {
		برآمد->قدر = ret;
		ret = 0;
	}

	واپس ret;
}

ساکن bool susp_دستیاب(خالی)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	برائے (type = 0; type <= SBI_SUSP_SLEEP_TYPE_آخری; type++) {
		اگر (sbi_system_suspend_supported(type))
			واپس true;
	}

	واپس false;
}

ساخت sbi_ecall_extension ecall_susp;

ساکن صحیح sbi_ecall_susp_رجسٹر_extensions(خالی)
{
	اگر (!susp_دستیاب())
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_susp);
}

ساخت sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_آغاز		= SBI_EXT_SUSP,
	.extid_انتہا		= SBI_EXT_SUSP,
	.رجسٹر_extensions	= sbi_ecall_susp_رجسٹر_extensions,
	.handle			= sbi_ecall_susp_handler,
};
