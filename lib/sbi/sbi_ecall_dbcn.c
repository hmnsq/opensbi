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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_DBCN_CONSOLE_WRITE:
	ಪಕ್ಷ SBI_EXT_DBCN_CONSOLE_READ:
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
		ಯದಿ (regs->a2)
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_FAILED;

		ಯದಿ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			ಪ್ರತ್ಯಾಗಮ SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		ಯದಿ (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->ಮೌಲ್ಯ = sbi_nputs((ಧ್ರುವ ವರ್ಣ *)regs->a1, regs->a0);
		ಅನ್ಯಥಾ
			out->ಮೌಲ್ಯ = sbi_ngets((ವರ್ಣ *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		ಪ್ರತ್ಯಾಗಮ 0;
	ಪಕ್ಷ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		ಪ್ರತ್ಯಾಗಮ 0;
	ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_dbcn;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_dbcn_register_extensions(ಶೂನ್ಯ)
{
	ಯದಿ (!sbi_console_get_device())
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_dbcn);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
