/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_hart_protection.h>

স্থির পূর্ণাঙ্ক sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	বিকল্প (funcid) {
	পক্ষ SBI_EXT_DBCN_CONSOLE_WRITE:
	পক্ষ SBI_EXT_DBCN_CONSOLE_READ:
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
		যদি (regs->a2)
			প্রত্যাগম SBI_ERR_FAILED;

		যদি (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			প্রত্যাগম SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		যদি (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->মান = sbi_nputs((ধ্রুব বর্ণ *)regs->a1, regs->a0);
		অন্যথা
			out->মান = sbi_ngets((বর্ণ *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		প্রত্যাগম 0;
	পক্ষ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		প্রত্যাগম 0;
	স্বভাব:
		ভঙ্গ;
	}

	প্রত্যাগম SBI_ENOTSUPP;
}

সংরচনা sbi_ecall_extension ecall_dbcn;

স্থির পূর্ণাঙ্ক sbi_ecall_dbcn_register_extensions(শূন্য)
{
	যদি (!sbi_console_get_device())
		প্রত্যাগম 0;

	প্রত্যাগম sbi_ecall_register_extension(&ecall_dbcn);
}

সংরচনা sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
