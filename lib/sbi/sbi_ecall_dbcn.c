/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/sbi_hart_protection.h>

स्थिर पूर्णांक sbi_ecall_dbcn_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	निवड (funcid) {
	स्थिती SBI_EXT_DBCN_CONSOLE_WRITE:
	स्थिती SBI_EXT_DBCN_CONSOLE_READ:
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
		जर (regs->a2)
			परत SBI_ERR_FAILED;

		जर (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			परत SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_temp_map_range(regs->a1, regs->a0);
		जर (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			out->मूल्य = sbi_nputs((स्थिरांक अक्षर *)regs->a1, regs->a0);
		अन्यथा
			out->मूल्य = sbi_ngets((अक्षर *)regs->a1, regs->a0);
		sbi_hart_protection_temp_unmap_range(regs->a1, regs->a0);
		परत 0;
	स्थिती SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		परत 0;
	पूर्वनिर्धारित:
		थांबा;
	}

	परत SBI_ENOTSUPP;
}

संरचना sbi_ecall_extension ecall_dbcn;

स्थिर पूर्णांक sbi_ecall_dbcn_register_extensions(शून्य)
{
	जर (!sbi_console_get_device())
		परत 0;

	परत sbi_ecall_register_extension(&ecall_dbcn);
}

संरचना sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_end		= SBI_EXT_DBCN,
	.register_extensions	= sbi_ecall_dbcn_register_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
