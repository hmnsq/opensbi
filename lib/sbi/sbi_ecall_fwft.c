/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_fwft.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 ಸಂರಚನಾ sbi_trap_regs *regs,
				 ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_fwft;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_fwft_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_fwft);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
