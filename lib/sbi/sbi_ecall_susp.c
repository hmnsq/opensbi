// SPDX-License-Identifier: BSD-2-Clause
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_system.h>

static цяло sbi_ecall_susp_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	цяло ret = SBI_ENOTSUPP;

	ако (funcid == SBI_EXT_SUSP_SUSPEND)
		ret = sbi_system_suspend(regs->a0, regs->a1, regs->a2);

	ако (ret >= 0) {
		out->value = ret;
		ret = 0;
	}

	върни ret;
}

static bool susp_available(void)
{
	u32 type;

	/*
	 * At least one suspend type should be supported by the
	 * platform for the SBI SUSP extension to be usable.
	 */
	за (type = 0; type <= SBI_SUSP_SLEEP_TYPE_LAST; type++) {
		ако (sbi_system_suspend_supported(type))
			върни true;
	}

	върни false;
}

struct sbi_ecall_extension ecall_susp;

static цяло sbi_ecall_susp_register_extensions(void)
{
	ако (!susp_available())
		върни 0;

	върни sbi_ecall_register_extension(&ecall_susp);
}

struct sbi_ecall_extension ecall_susp = {
	.name			= "susp",
	.extid_start		= SBI_EXT_SUSP,
	.extid_end		= SBI_EXT_SUSP,
	.register_extensions	= sbi_ecall_susp_register_extensions,
	.handle			= sbi_ecall_susp_handler,
};
