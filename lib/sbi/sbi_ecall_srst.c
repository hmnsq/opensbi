/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_system.h>

static ཧྲིལ sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	གལ་ཏེ (funcid == SBI_EXT_SRST_RESET) {
		གལ་ཏེ ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			ཕྱིར SBI_EINVAL;

		switch (regs->a0) {
		pak_sa SBI_SRST_RESET_TYPE_SHUTDOWN:
		pak_sa SBI_SRST_RESET_TYPE_COLD_REBOOT:
		pak_sa SBI_SRST_RESET_TYPE_WARM_REBOOT:
			bha_nga;
		default:
			ཕྱིར SBI_EINVAL;
		}

		switch (regs->a1) {
		pak_sa SBI_SRST_RESET_REASON_NONE:
		pak_sa SBI_SRST_RESET_REASON_SYSFAIL:
			bha_nga;
		default:
			ཕྱིར SBI_EINVAL;
		}

		གལ་ཏེ (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	ཕྱིར SBI_ENOTSUPP;
}

static bool srst_available(void)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ཆེད་དུ (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		གལ་ཏེ (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			ཕྱིར true;
	}

	ཕྱིར false;
}

sa_nracanaa sbi_ecall_extension ecall_srst;

static ཧྲིལ sbi_ecall_srst_register_extensions(void)
{
	གལ་ཏེ (!srst_available())
		ཕྱིར 0;

	ཕྱིར sbi_ecall_register_extension(&ecall_srst);
}

sa_nracanaa sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
