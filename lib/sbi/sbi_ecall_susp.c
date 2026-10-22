// SPDX-License-Identifier: BSD-2-Clause
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_system.h>

ساكن صحيح sbi_ecall_susp_handler(بنانشانی لما extid, بنانشانی لما funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = SBI_ENOTSUPP;

	إذا (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	إذا (ret >= 0) {
		باہرلا->قدر = ret;
		ret = 0;
	}

	أرجع ret;
}

ساكن bool susp_دستیاب(فراغ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	لكل (type = 0; type <= SBI_SUSP_SLEEP_TYPE_آخری; type++) {
		إذا (sbi_system_suspend_supported(type))
			أرجع true;
	}

	أرجع false;
}

بنية sbi_ecall_extension ecall_susp;

ساكن صحيح sbi_ecall_susp_رجسٹر_extensions(فراغ)
{
	إذا (!susp_دستیاب())
		أرجع 0;

	أرجع sbi_ecall_رجسٹر_extension(&ecall_susp);
}

بنية sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_شروع		= SBI_EXT_SUSP,
	.extid_سرا		= SBI_EXT_SUSP,
	.رجسٹر_extensions	= sbi_ecall_susp_رجسٹر_extensions,
	.handle			= sbi_ecall_susp_handler,
};
