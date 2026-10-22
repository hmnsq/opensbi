/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart_protection.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_dbcn_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_ಸರಿಸು;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_DBCN_CONSOLE_WRITE:
	ಸ್ಥಿತಿ SBI_EXT_DBCN_CONSOLE_READ:
		/*
		 * On RV32, the M-mode can only access the first 4GB of
		 * the physical address space because M-mode does not have
		 * MMU to access full 34-bit physical address space.
		 *
		 * Based on above, we simply fail if the upper 32bits of
		 * the physical address (i.e. a2 register) is non-zero on
		 * RV32.
		 *
		 * Analogously, we fail if the upper 64bit of the
		 * physical address (i.e. a2 register) is non-zero on
		 * RV64.
		 */
		ಆದರೆ (regs->a2)
			ಹಿಂತಿರುಗಿ SBI_ERR_FAILED;

		ಆದರೆ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			ಹಿಂತಿರುಗಿ SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_map_range(regs->a1, regs->a0);
		ಆದರೆ (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			ಹೊರಹೋಗು->ಮೌಲ್ಯ = sbi_nputs((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)regs->a1, regs->a0);
		ಇಲ್ಲದಿದ್ದರೆ
			ಹೊರಹೋಗು->ಮೌಲ್ಯ = sbi_ngets((ಅಕ್ಷರ *)regs->a1, regs->a0);
		sbi_hart_protection_ತಾತ್ಕಾಲಿಕ_unmap_range(regs->a1, regs->a0);
		ಹಿಂತಿರುಗಿ 0;
	ಸ್ಥಿತಿ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		ಹಿಂತಿರುಗಿ 0;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ರಚನೆ sbi_ecall_extension ecall_dbcn;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_dbcn_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (!sbi_console_get_device())
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_dbcn);
}

ರಚನೆ sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_ಆರಂಭ		= SBI_EXT_DBCN,
	.extid_ಕೊನೆ		= SBI_EXT_DBCN,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_dbcn_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
