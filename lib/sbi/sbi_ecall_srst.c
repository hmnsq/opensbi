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

static 整数 sbi_ecall_srst_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	もし (funcid == SBI_EXT_SRST_RESET) {
		もし ((((u32)-1U) <= ((u64)regs->a0)) ||
		    (((u32)-1U) <= ((u64)regs->a1)))
			戻る SBI_EINVAL;

		switch (regs->a0) {
		case SBI_SRST_RESET_TYPE_SHUTDOWN:
		case SBI_SRST_RESET_TYPE_COLD_REBOOT:
		case SBI_SRST_RESET_TYPE_WARM_REBOOT:
			break;
		default:
			戻る SBI_EINVAL;
		}

		switch (regs->a1) {
		case SBI_SRST_RESET_REASON_NONE:
		case SBI_SRST_RESET_REASON_SYSFAIL:
			break;
		default:
			戻る SBI_EINVAL;
		}

		もし (sbi_system_reset_supported(regs->a0, regs->a1))
			sbi_system_reset(regs->a0, regs->a1);
	}

	戻る SBI_ENOTSUPP;
}

static bool srst_available(void)
{
	u32 type;

	/*
	 * At least one standard reset types should be supported by
	 * the platform for SBI SRST extension to be usable.
	 */
	ために (type = 0; type <= SBI_SRST_RESET_TYPE_LAST; type++) {
		もし (sbi_system_reset_supported(type,
					SBI_SRST_RESET_REASON_NONE))
			戻る true;
	}

	戻る false;
}

struct sbi_ecall_extension ecall_srst;

static 整数 sbi_ecall_srst_register_extensions(void)
{
	もし (!srst_available())
		戻る 0;

	戻る sbi_ecall_register_extension(&ecall_srst);
}

struct sbi_ecall_extension ecall_srst = {
	.name			= "srst",
	.extid_start		= SBI_EXT_SRST,
	.extid_end		= SBI_EXT_SRST,
	.register_extensions	= sbi_ecall_srst_register_extensions,
	.handle			= sbi_ecall_srst_handler,
};
