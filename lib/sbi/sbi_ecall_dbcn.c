/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_trap.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_hart_protection.h>

static ціле sbi_ecall_dbcn_handler(unsigned long extid, unsigned long funcid,
				  struct sbi_trap_regs *regs,
				  struct sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	switch (funcid) {
	case SBI_EXT_DBCN_CONSOLE_WRITE:
	case SBI_EXT_DBCN_CONSOLE_READ:
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
		якщо (regs->a2)
			повернути SBI_ERR_FAILED;

		якщо (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			повернути SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		якщо (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->value = sbi_nputs((const char *)regs->a1, regs->a0);
		інакше
			out->value = sbi_ngets((char *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		повернути 0;
	case SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		повернути 0;
	default:
		break;
	}

	повернути SBI_ENOTSUPP;
}

struct sbi_ecall_extension ecall_dbcn;

static ціле sbi_ecall_dbcn_register_extensions(void)
{
	якщо (!sbi_console_get_device())
		повернути 0;

	повернути sbi_ecall_register_extension(&ecall_dbcn);
}

struct sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
