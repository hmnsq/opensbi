/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_hart_protection.h>

ایستا صحیح sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	انتخاب (funcid) {
	حالت SBI_EXT_DBCN_CONSOLE_WRITE:
	حالت SBI_EXT_DBCN_CONSOLE_READ:
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
		اگر (regs->a2)
			بازگشت SBI_ERR_FAILED;

		اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			بازگشت SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		اگر (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->مقدار = sbi_nputs((ثابت نویسه *)regs->a1, regs->a0);
		وگرنه
			out->مقدار = sbi_ngets((نویسه *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		بازگشت 0;
	حالت SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		بازگشت 0;
	پیشفرض:
		بشکن;
	}

	بازگشت SBI_ENOTSUPP;
}

ساختار sbi_ecall_extension ecall_dbcn;

ایستا صحیح sbi_ecall_dbcn_register_extensions(تهی)
{
	اگر (!sbi_console_get_device())
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_dbcn);
}

ساختار sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
