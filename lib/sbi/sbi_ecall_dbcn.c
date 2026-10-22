/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_hart_protection.h>

अटल पूर्णांक sbi_ecall_dbcn_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	चयन (funcid) {
	हाल SBI_EXT_DBCN_CONSOLE_WRITE:
	हाल SBI_EXT_DBCN_CONSOLE_READ:
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
		अगर (regs->a2)
			वापस SBI_ERR_FAILED;

		अगर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			वापस SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_अस्थायी_map_range(regs->a1, regs->a0);
		अगर (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->मूल्य = sbi_nputs((स्थिर अक्षर *)regs->a1, regs->a0);
		अन्यथा
			out->मूल्य = sbi_ngets((अक्षर *)regs->a1, regs->a0);
		sbi_hart_protection_अस्थायी_unmap_range(regs->a1, regs->a0);
		वापस 0;
	हाल SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		वापस 0;
	शेष:
		अवरोध;
	}

	वापस SBI_ENOTSUPP;
}

काष्ठा sbi_ecall_extension ecall_dbcn;

अटल पूर्णांक sbi_ecall_dbcn_register_extensions(व्योम)
{
	अगर (!sbi_console_get_device())
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_dbcn);
}

काष्ठा sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
