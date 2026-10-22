// SPDX-License-Identifier: BSD-2-Clause
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_system.h>

ساكن صحيح sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *out)
{
	صحيح ret = SBI_ENOTSUPP;

	إذا (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	إذا (ret >= 0) {
		out->قيمة = ret;
		ret = 0;
	}

	إرجاع ret;
}

ساكن bool susp_available(فراغ)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	لكل (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		إذا (sbi_system_suspend_supported(type))
			إرجاع true;
	}

	إرجاع false;
}

بنية sbi_ecall_extension ecall_susp;

ساكن صحيح sbi_ecall_susp_register_extensions(فراغ)
{
	إذا (!susp_available())
		إرجاع 0;

	إرجاع sbi_ecall_register_extension(&ecall_susp);
}

بنية sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
