// SPDX-License-Identifier: BSD-2-Clause
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_system.h>

static ཧྲིལ sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	ཧྲིལ ret = SBI_ENOTSUPP;

	གལ་སྲིད (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	གལ་སྲིད (ret >= 0) {
		out->value = ret;
		ret = 0;
	}

	ལོག ret;
}

static bool susp_available(void)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	དོན་ལུ (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		གལ་སྲིད (sbi_system_suspend_supported(type))
			ལོག true;
	}

	ལོག false;
}

sa_nracanaa sbi_ecall_extension ecall_susp;

static ཧྲིལ sbi_ecall_susp_register_extensions(void)
{
	གལ་སྲིད (!susp_available())
		ལོག 0;

	ལོག sbi_ecall_register_extension(&ecall_susp);
}

sa_nracanaa sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
