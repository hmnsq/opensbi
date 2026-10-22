/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_dbtr.h>

स्थिर पूर्णाङ्क sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  संरचना sbi_trap_regs *regs,
				  संरचना sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	पूर्णाङ्क ret = 0;

	विकल्प (funcid) {
	पक्ष SBI_EXT_DBTR_अङ्कः_TRIGGERS:
		ret = sbi_dbtr_अङ्कः_trig(regs->a0, &out->मानम्);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->मानम्);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	};

	पटिनिवत्त ret;
}

संरचना sbi_ecall_extension ecall_dbtr;

स्थिर पूर्णाङ्क sbi_ecall_dbtr_register_extensions(शून्य)
{
	सचे (sbi_dbtr_get_total_triggers() == 0)
		पटिनिवत्त 0;

	पटिनिवत्त sbi_ecall_register_extension(&ecall_dbtr);
}

संरचना sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
