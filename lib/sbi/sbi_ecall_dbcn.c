/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_hart_protection.h>

స్థిర పూర్ణాంకం sbi_ecall_dbcn_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_జరుపు;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_DBCN_CONSOLE_WRITE:
	స్థితి SBI_EXT_DBCN_CONSOLE_READ:
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
		ఒకవేళ (regs->a2)
			తిరిగి SBI_ERR_FAILED;

		ఒకవేళ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			తిరిగి SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_తాత్కాలిక_map_range(regs->a1, regs->a0);
		ఒకవేళ (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			బహిర్గతం->విలువ = sbi_nputs((స్థిరరాశి అక్షరం *)regs->a1, regs->a0);
		లేకపోతే
			బహిర్గతం->విలువ = sbi_ngets((అక్షరం *)regs->a1, regs->a0);
		sbi_hart_protection_తాత్కాలిక_unmap_range(regs->a1, regs->a0);
		తిరిగి 0;
	స్థితి SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		తిరిగి 0;
	అప్రమేయ:
		ఆపు;
	}

	తిరిగి SBI_ENOTSUPP;
}

నిర్మాణం sbi_ecall_extension ecall_dbcn;

స్థిర పూర్ణాంకం sbi_ecall_dbcn_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (!sbi_console_get_device())
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_dbcn);
}

నిర్మాణం sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_ప్రారంభం		= SBI_EXT_DBCN,
	.extid_చివర		= SBI_EXT_DBCN,
	.రిజిస్టరు_extensions	= sbi_ecall_dbcn_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
