/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 ഘടന sbi_trap_regs *regs,
				 ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;

	എങ്കിൽ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	അല്ലെങ്കിൽ
		ret = SBI_ENOTSUPP;

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_ipi;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_ipi_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_ipi);
}

ഘടന sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
