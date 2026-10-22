/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_version.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ತಳ_probe(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ಹೊರಹೋಗು_val)
{
	ರಚನೆ sbi_ecall_extension *ext;

	ext = sbi_ecall_ಕಂಡುಹಿಡಿ_extension(extid);
	ಆದರೆ (!ext) {
		*ಹೊರಹೋಗು_val = 0;
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆದರೆ (ext->probe)
		ಹಿಂತಿರುಗಿ ext->probe(extid, ಹೊರಹೋಗು_val);

	*ಹೊರಹೋಗು_val = 1;
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ತಳ_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_SPEC_ಆವೃತ್ತಿ:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = (SBI_ECALL_ಆವೃತ್ತಿ_MAJOR <<
			      SBI_SPEC_ಆವೃತ್ತಿ_MAJOR_ಸ್ಥಾನಾಂತರ) &
			     (SBI_SPEC_ಆವೃತ್ತಿ_MAJOR_ಮುಖವಾಡ <<
			      SBI_SPEC_ಆವೃತ್ತಿ_MAJOR_ಸ್ಥಾನಾಂತರ);
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ಹೊರಹೋಗು->ಮೌಲ್ಯ | SBI_ECALL_ಆವೃತ್ತಿ_MINOR;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_IMP_ID:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = sbi_ecall_get_impid();
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_IMP_ಆವೃತ್ತಿ:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = OPENSBI_ಆವೃತ್ತಿ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_MVENDORID:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = csr_read(CSR_MVENDORID);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_MARCHID:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = csr_read(CSR_MARCHID);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_GET_MIMPID:
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = csr_read(CSR_MIMPID);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_ತಳ_PROBE_EXT:
		ret = sbi_ecall_ತಳ_probe(regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_ತಳ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_ತಳ_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_ತಳ);
}

ರಚನೆ sbi_ecall_extension ecall_ತಳ = {
	.name			= "base",
	.extid_ಆರಂಭ		= SBI_EXT_ತಳ,
	.extid_ಕೊನೆ		= SBI_EXT_ತಳ,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_ತಳ_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_ತಳ_handler,
};
