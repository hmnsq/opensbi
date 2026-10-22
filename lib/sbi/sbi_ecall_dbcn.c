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

स्थिर पूर्णाङ्क sbi_ecall_dbcn_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *बहिर्गमनम्)
{
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_सरणम्;

	विकल्प (funcid) {
	पक्ष SBI_EXT_DBCN_CONSOLE_WRITE:
	पक्ष SBI_EXT_DBCN_CONSOLE_READ:
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
		यदि (regs->a2)
			प्रत्यागम SBI_ERR_FAILED;

		यदि (!sbi_domain_check_addr_range(sbi_domain_thishart_ptr(),
					regs->a1, regs->a0, smode,
					SBI_DOMAIN_READ|SBI_DOMAIN_WRITE))
			प्रत्यागम SBI_ERR_INVALID_PARAM;
		sbi_hart_protection_अस्थायी_map_range(regs->a1, regs->a0);
		यदि (funcid == SBI_EXT_DBCN_CONSOLE_WRITE)
			बहिर्गमनम्->मानम् = sbi_nputs((ध्रुव वर्ण *)regs->a1, regs->a0);
		अन्यथा
			बहिर्गमनम्->मानम् = sbi_ngets((वर्ण *)regs->a1, regs->a0);
		sbi_hart_protection_अस्थायी_unmap_range(regs->a1, regs->a0);
		प्रत्यागम 0;
	पक्ष SBI_EXT_DBCN_CONSOLE_WRITE_BYTE:
		sbi_putc(regs->a0);
		प्रत्यागम 0;
	स्वभाव:
		भङ्ग;
	}

	प्रत्यागम SBI_ENOTSUPP;
}

संरचना sbi_ecall_extension ecall_dbcn;

स्थिर पूर्णाङ्क sbi_ecall_dbcn_पञ्जिकाकोशः_extensions(शून्य)
{
	यदि (!sbi_console_get_device())
		प्रत्यागम 0;

	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_dbcn);
}

संरचना sbi_ecall_extension ecall_dbcn = {
	.name			= "dbcn",
	.extid_start		= SBI_EXT_DBCN,
	.extid_अन्तः		= SBI_EXT_DBCN,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_dbcn_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_dbcn_handler,
};
