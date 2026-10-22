// SPDX-License-Identifier: BSD-2-Clause
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = SBI_ENOTSUPP;

	എങ്കിൽ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	എങ്കിൽ (ret >= 0) {
		out->മൂല്യം = ret;
		ret = 0;
	}

	തിരികെ ret;
}

സ്ഥിതം bool susp_available(ശൂന്യം)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	വേണ്ടി (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		എങ്കിൽ (sbi_system_suspend_supported(type))
			തിരികെ true;
	}

	തിരികെ false;
}

ഘടന sbi_ecall_extension ecall_susp;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_susp_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (!susp_available())
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_susp);
}

ഘടന sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
