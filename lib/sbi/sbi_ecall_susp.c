// SPDX-License-Identifier: BSD-2-Clause
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_system.h>

స్థిర పూర్ణాంకం sbi_ecall_susp_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = SBI_ENOTSUPP;

	ఒకవేళ (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ఒకవేళ (ret >= 0) {
		బహిర్గతం->విలువ = ret;
		ret = 0;
	}

	తిరిగి ret;
}

స్థిర bool susp_అందుబాటు(శూన్య)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	కొరకు (type = 0; type <= SBI_SUSP_SLEEP_TYPE_చివరి; type++) {
		ఒకవేళ (sbi_system_suspend_supported(type))
			తిరిగి true;
	}

	తిరిగి false;
}

నిర్మాణం sbi_ecall_extension ecall_susp;

స్థిర పూర్ణాంకం sbi_ecall_susp_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (!susp_అందుబాటు())
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_susp);
}

నిర్మాణం sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_ప్రారంభం		= SBI_EXT_SUSP,
	.extid_చివర		= SBI_EXT_SUSP,
	.రిజిస్టరు_extensions	= sbi_ecall_susp_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_susp_handler,
};
