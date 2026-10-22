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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_fwft_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				 ರಚನೆ sbi_trap_regs *regs,
				 ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_FWFT_ಗಣ:
		ret = sbi_fwft_ಗಣ(regs->a0, regs->a1, regs->a2);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_fwft;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_fwft_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_fwft);
}

ರಚನೆ sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_ಆರಂಭ		= SBI_EXT_FWFT,
	.extid_ಕೊನೆ		= SBI_EXT_FWFT,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_fwft_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
