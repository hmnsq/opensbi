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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ipi_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				 ರಚನೆ sbi_trap_regs *regs,
				 ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆದರೆ (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	ಇಲ್ಲದಿದ್ದರೆ
		ret = SBI_ENOTSUPP;

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_ipi;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ipi_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_ipi);
}

ರಚನೆ sbi_ecall_extension ecall_ipi = {
	.name			= "ipi",
	.extid_ಆರಂಭ		= SBI_EXT_IPI,
	.extid_ಕೊನೆ		= SBI_EXT_IPI,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_ipi_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_ipi_handler,
};
