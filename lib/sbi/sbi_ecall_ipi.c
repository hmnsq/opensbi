/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>

સ્થિર પૂર્ણાંક sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 સંરચના sbi_trap_regs *regs,
				 સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;

	જો (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	નહીંતર
		ret = SBI_ENOTSUPP;

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_ipi;

સ્થિર પૂર્ણાંક sbi_ecall_ipi_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_ipi);
}

સંરચના sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
