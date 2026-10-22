/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hart_protection.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_DBCN_CONSOLE_WRITE:
	ସ୍ଥିତି SBI_EXT_DBCN_CONSOLE_READ:
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
		ଯଦି (regs->a2)
			ଫେରନ୍ତୁ SBI_ERR_FAILED;

		ଯଦି (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			ଫେରନ୍ତୁ SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		ଯଦି (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->ମୂଲ୍ୟ = sbi_nputs((ଅଚଳ ଅକ୍ଷର *)regs->a1, regs->a0);
		ଅନ୍ୟଥା
			out->ମୂଲ୍ୟ = sbi_ngets((ଅକ୍ଷର *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		ଫେରନ୍ତୁ 0;
	ସ୍ଥିତି SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		ଫେରନ୍ତୁ 0;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସଂରଚନା sbi_ecall_extension ecall_dbcn;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_dbcn_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (!sbi_console_get_device())
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_dbcn);
}

ସଂରଚନା sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
