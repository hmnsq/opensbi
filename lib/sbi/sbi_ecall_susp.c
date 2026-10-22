// SPDX-License-Identifier: BSD-2-Clause
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_system.h>

ساكن صحيح sbi_ecall_susp_handler(بینښه اوږد extid, بینښه اوږد funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بهر)
{
	صحيح ret = SBI_ENOTSUPP;

	إذا (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	إذا (ret >= 0) {
		بهر->ارزښت = ret;
		ret = 0;
	}

	أرجع ret;
}

ساكن bool susp_شته(فراغ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	لكل (type = 0; type <= SBI_SUSP_SLEEP_TYPE_وروستی; type++) {
		إذا (sbi_system_suspend_supported(type))
			أرجع true;
	}

	أرجع false;
}

بنية sbi_ecall_extension ecall_susp;

ساكن صحيح sbi_ecall_susp_ثبتوونکی_extensions(فراغ)
{
	إذا (!susp_شته())
		أرجع 0;

	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_susp);
}

بنية sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_پیل		= SBI_EXT_SUSP,
	.extid_پای_ټکی		= SBI_EXT_SUSP,
	.ثبتوونکی_extensions	= sbi_ecall_susp_ثبتوونکی_extensions,
	.handle			= sbi_ecall_susp_handler,
};
