/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_hart_protection.h>

ساكن صحيح sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	بدل (funcid) {
	حالة SBI_EXT_DBCN_CONSOLE_WRITE:
	حالة SBI_EXT_DBCN_CONSOLE_READ:
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
		إذا (regs->a2)
			أرجع SBI_ERR_FAILED;

		إذا (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			أرجع SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		إذا (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->قدر = sbi_nputs((ثابت حرف *)regs->a1, regs->a0);
		وإلا
			out->قدر = sbi_ngets((حرف *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		أرجع 0;
	حالة SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		أرجع 0;
	افتراضي:
		اكسر;
	}

	أرجع SBI_ENOTSUPP;
}

بنية sbi_ecall_extension ecall_dbcn;

ساكن صحيح sbi_ecall_dbcn_register_extensions(فراغ)
{
	إذا (!sbi_console_get_device())
		أرجع 0;

	أرجع sbi_ecall_register_extension(&ecall_dbcn);
}

بنية sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
