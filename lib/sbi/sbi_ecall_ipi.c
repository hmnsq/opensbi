/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_ipi.h>

ساكن صحيح sbi_ecall_ipi_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;

	إذا (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	وإلا
		ret = SBI_ENOTSUPP;

	أرجع ret;
}

بنية sbi_ecall_extension ecall_ipi;

ساكن صحيح sbi_ecall_ipi_ثبات_extensions(فراغ)
{
	أرجع sbi_ecall_ثبات_extension(&ecall_ipi);
}

بنية sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_شروع		= SBI_EXT_IPI,
	.extid_انتها		= SBI_EXT_IPI,
	.ثبات_extensions	= sbi_ecall_ipi_ثبات_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
