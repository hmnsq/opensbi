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

ساکن صحیح sbi_ecall_dbcn_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_سرکاؤ;

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
			واپس SBI_ERR_FAILED;

		اگر (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			واپس SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_عارضی_map_range(regs->a1, regs->a0);
		اگر (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			برآمد->قدر = sbi_nputs((ثابت حرف *)regs->a1, regs->a0);
		ورنہ
			برآمد->قدر = sbi_ngets((حرف *)regs->a1, regs->a0);
		sbi_hart_protection_عارضی_unmap_range(regs->a1, regs->a0);
		واپس 0;
	حالت SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		واپس 0;
	طےشدہ:
		توڑو;
	}

	واپس SBI_ENOTSUPP;
}

ساخت sbi_ecall_extension ecall_dbcn;

ساکن صحیح sbi_ecall_dbcn_رجسٹر_extensions(خالی)
{
	اگر (!sbi_console_get_device())
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_dbcn);
}

ساخت sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_آغاز		= SBI_EXT_DBCN,
	.extid_انتہا		= SBI_EXT_DBCN,
	.رجسٹر_extensions	= sbi_ecall_dbcn_رجسٹر_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
