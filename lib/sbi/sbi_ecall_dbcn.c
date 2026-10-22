/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/sbi_hart_protection.h>

સ્થિર પૂર્ણાંક sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_DBCN_CONSOLE_WRITE:
	સ્થિતિ SBI_EXT_DBCN_CONSOLE_READ:
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
		જો (regs->a2)
			પાછા SBI_ERR_FAILED;

		જો (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			પાછા SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		જો (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->કિંમત = sbi_nputs((અચલ અક્ષર *)regs->a1, regs->a0);
		નહીંતર
			out->કિંમત = sbi_ngets((અક્ષર *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		પાછા 0;
	સ્થિતિ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		પાછા 0;
	મૂળભૂત:
		તોડો;
	}

	પાછા SBI_ENOTSUPP;
}

સંરચના sbi_ecall_extension ecall_dbcn;

સ્થિર પૂર્ણાંક sbi_ecall_dbcn_register_extensions(શૂન્ય)
{
	જો (!sbi_console_get_device())
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_dbcn);
}

સંરચના sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
