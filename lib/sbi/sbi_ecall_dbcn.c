/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart_protection.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_dbcn_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_ਖਿਸਕਾਓ;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_DBCN_CONSOLE_WRITE:
	ਹਾਲਤ SBI_EXT_DBCN_CONSOLE_READ:
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
		ਜੇ (regs->a2)
			ਵਾਪਸ SBI_ERR_FAILED;

		ਜੇ (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			ਵਾਪਸ SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_ਆਰਜ਼ੀ_map_range(regs->a1, regs->a0);
		ਜੇ (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			ਬਾਹਰਲਾ->ਮੁੱਲ = sbi_nputs((ਅਚਲ ਅੱਖਰ *)regs->a1, regs->a0);
		ਨਹੀਂਤਾਂ
			ਬਾਹਰਲਾ->ਮੁੱਲ = sbi_ngets((ਅੱਖਰ *)regs->a1, regs->a0);
		sbi_hart_protection_ਆਰਜ਼ੀ_unmap_range(regs->a1, regs->a0);
		ਵਾਪਸ 0;
	ਹਾਲਤ SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		ਵਾਪਸ 0;
	ਮੂਲ:
		ਰੋਕੋ;
	}

	ਵਾਪਸ SBI_ENOTSUPP;
}

ਬਣਤਰ sbi_ecall_extension ecall_dbcn;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_dbcn_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (!sbi_console_get_device())
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_dbcn);
}

ਬਣਤਰ sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_DBCN,
	.extid_ਸਿਰਾ		= SBI_EXT_DBCN,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_dbcn_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
