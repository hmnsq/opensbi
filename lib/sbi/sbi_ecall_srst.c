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

static цео sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	ако (funcid == SBI_EXT_SRST_RESET) {
		ако ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			врати SBI_EINVAL;

		switch (regs->a0) {
		case SBI_SRST_RESET_TYPE_SHUTDOWN:
		case SBI_SRST_RESET_TYPE_COLD_REBOOT:
		case SBI_SRST_RESET_TYPE_WARM_REBOOT:
			break;
		default:
			врати SBI_EINVAL;
		}

		switch (regs->a1) {
		case SBI_SRST_RESET_REASON_NONE:
		case SBI_SRST_RESET_REASON_SYSFAIL:
			break;
		default:
			врати SBI_EINVAL;
		}

		ако (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	врати SBI_ENOTSUPP;
}

static bool srst_available(void)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	за (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		ако (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			врати true;
	}

	врати false;
}

struct sbi_ecall_extension ecall_srst;

static цео sbi_ecall_srst_register_extensions(void)
{
	ако (!srst_available())
		врати 0;

	врати sbi_ecall_register_extension(&ecall_srst);
}

struct sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
