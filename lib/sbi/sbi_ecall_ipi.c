/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 ಸಂರಚನಾ sbi_trap_regs *regs,
				 ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	ಯದಿ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	ಅನ್ಯಥಾ
		ret = SBI_ENOTSUPP;

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_ipi;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_ipi_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_ipi);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_start		= SBI_EXT_IPI,
	.extid_end		= SBI_EXT_IPI,
	.register_extensions	= sbi_ecall_ipi_register_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
