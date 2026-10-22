/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_cppc.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_cppc_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಪೂರ್ಣಾಂಕ ret = 0;
	uint64_t ತಾತ್ಕಾಲಿಕ;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &ತಾತ್ಕಾಲಿಕ);
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ತಾತ್ಕಾಲಿಕ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_CPPC_READ_HI:
#ಆದರೆ __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &ತಾತ್ಕಾಲಿಕ);
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = ತಾತ್ಕಾಲಿಕ >> 32;
#ಇಲ್ಲದಿದ್ದರೆ
		ಹೊರಹೋಗು->ಮೌಲ್ಯ = 0;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_CPPC_WRITE:
#ಆದರೆ __riscv_xlen == 32
		ತಾತ್ಕಾಲಿಕ = regs->a2;
		ತಾತ್ಕಾಲಿಕ = (ತಾತ್ಕಾಲಿಕ << 32) | regs->a1;
#ಇಲ್ಲದಿದ್ದರೆ
		ತಾತ್ಕಾಲಿಕ = regs->a1;
#ಆದರೆ_ಅಂತ್ಯ
		ret = sbi_cppc_write(regs->a0, ತಾತ್ಕಾಲಿಕ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		ಆದರೆ (ret >= 0) {
			ಹೊರಹೋಗು->ಮೌಲ್ಯ = ret;
			ret = 0;
		}
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_cppc;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_cppc_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (!sbi_cppc_get_device())
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_cppc);
}

ರಚನೆ sbi_ecall_extension ecall_cppc = {
	.name			= "cppc",
	.extid_ಆರಂಭ		= SBI_EXT_CPPC,
	.extid_ಕೊನೆ		= SBI_EXT_CPPC,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_cppc_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_cppc_handler,
};
