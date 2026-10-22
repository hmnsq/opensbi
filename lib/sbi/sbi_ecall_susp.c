// SPDX-License-Identifier: BSD-2-Clause
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_system.h>

સ્થિર પૂર્ણાંક sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = SBI_ENOTSUPP;

	જો (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	જો (ret >= 0) {
		out->કિંમત = ret;
		ret = 0;
	}

	પાછા ret;
}

સ્થિર bool susp_available(શૂન્ય)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	માટે (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		જો (sbi_system_suspend_supported(type))
			પાછા true;
	}

	પાછા false;
}

સંરચના sbi_ecall_extension ecall_susp;

સ્થિર પૂર્ણાંક sbi_ecall_susp_register_extensions(શૂન્ય)
{
	જો (!susp_available())
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_susp);
}

સંરચના sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
