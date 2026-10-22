// SPDX-License-Identifier: BSD-2-Clause
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_system.h>

static 정수 sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	정수 ret = SBI_ENOTSUPP;

	만약 (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	만약 (ret >= 0) {
		out->value = ret;
		ret = 0;
	}

	반환 ret;
}

static bool susp_available(void)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	위해 (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		만약 (sbi_system_suspend_supported(type))
			반환 true;
	}

	반환 false;
}

struct sbi_ecall_extension ecall_susp;

static 정수 sbi_ecall_susp_register_extensions(void)
{
	만약 (!susp_available())
		반환 0;

	반환 sbi_ecall_register_extension(&ecall_susp);
}

struct sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
