// SPDX-License-Identifier: BSD-2-Clause
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = SBI_ENOTSUPP;

	ଯଦି (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ଯଦି (ret >= 0) {
		out->ମୂଲ୍ୟ = ret;
		ret = 0;
	}

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର bool susp_available(ଶୂନ୍ୟ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	ପାଇଁ (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		ଯଦି (sbi_system_suspend_supported(type))
			ଫେରନ୍ତୁ true;
	}

	ଫେରନ୍ତୁ false;
}

ସଂରଚନା sbi_ecall_extension ecall_susp;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_susp_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (!susp_available())
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_susp);
}

ସଂରଚନା sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
