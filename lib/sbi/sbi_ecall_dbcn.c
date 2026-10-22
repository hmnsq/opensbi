/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/sbi_hart_protection.h>

நிலையான முழு sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	தேர்வு (funcid) {
	நிலை SBI_EXT_DBCN_CONSOLE_WRITE:
	நிலை SBI_EXT_DBCN_CONSOLE_READ:
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
		எனில் (regs->a2)
			திரும்பு SBI_ERR_FAILED;

		எனில் (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			திரும்பு SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		எனில் (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->மதிப்பு = sbi_nputs((மாறிலி எழுத்து *)regs->a1, regs->a0);
		இல்லையெனில்
			out->மதிப்பு = sbi_ngets((எழுத்து *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		திரும்பு 0;
	நிலை SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		திரும்பு 0;
	இயல்பு:
		நிறுத்து;
	}

	திரும்பு SBI_ENOTSUPP;
}

கட்டமைப்பு sbi_ecall_extension ecall_dbcn;

நிலையான முழு sbi_ecall_dbcn_register_extensions(வெறுமை)
{
	எனில் (!sbi_console_get_device())
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_dbcn);
}

கட்டமைப்பு sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
