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
#include <sbi/sbi_ipi.h>

static จำนวนเต็ม sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 sa_nracanaa sbi_trap_regs *regs,
				 sa_nracanaa sbi_ecall_return *out)
{
	จำนวนเต็ม ret = 0;

	ถ้า (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	มิฉะนั้น
		ret = SBI_ENOTSUPP;

	คืน ret;
}

sa_nracanaa sbi_ecall_extension ecall_ipi;

static จำนวนเต็ม sbi_ecall_ipi_register_extensions(void)
{
	คืน sbi_ecall_register_extension(&ecall_ipi);
}

sa_nracanaa sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
