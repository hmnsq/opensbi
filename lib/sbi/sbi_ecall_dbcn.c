/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/sbi_hart_protection.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_DBCN_CONSOLE_WRITE:
	അവസ്ഥ SBI_EXT_DBCN_CONSOLE_READ:
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
		എങ്കിൽ (regs->a2)
			തിരികെ SBI_ERR_FAILED;

		എങ്കിൽ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			തിരികെ SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		എങ്കിൽ (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->മൂല്യം = sbi_nputs((സ്ഥിരം അക്ഷരം *)regs->a1, regs->a0);
		അല്ലെങ്കിൽ
			out->മൂല്യം = sbi_ngets((അക്ഷരം *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		തിരികെ 0;
	അവസ്ഥ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		തിരികെ 0;
	സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}

	തിരികെ SBI_ENOTSUPP;
}

ഘടന sbi_ecall_extension ecall_dbcn;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_dbcn_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (!sbi_console_get_device())
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_dbcn);
}

ഘടന sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
