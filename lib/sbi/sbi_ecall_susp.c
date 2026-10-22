// SPDX-License-Identifier: BSD-2-Clause
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_system.h>

স্থির পূর্ণসংখ্যা sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = SBI_ENOTSUPP;

	যদি (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	যদি (ret >= 0) {
		out->মান = ret;
		ret = 0;
	}

	ফেরত ret;
}

স্থির bool susp_available(শূন্য)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	জন্য (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		যদি (sbi_system_suspend_supported(type))
			ফেরত true;
	}

	ফেরত false;
}

কাঠামো sbi_ecall_extension ecall_susp;

স্থির পূর্ণসংখ্যা sbi_ecall_susp_register_extensions(শূন্য)
{
	যদি (!susp_available())
		ফেরত 0;

	ফেরত sbi_ecall_register_extension(&ecall_susp);
}

কাঠামো sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
