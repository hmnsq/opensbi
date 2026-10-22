/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_dbtr.h>

સ્થિર પૂર્ણાંક sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	પૂર્ણાંક ret = 0;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_DBTR_અંક_TRIGGERS:
		ret = sbi_dbtr_અંક_trig(regs->a0, &out->કિંમત);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->કિંમત);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	};

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_dbtr;

સ્થિર પૂર્ણાંક sbi_ecall_dbtr_register_extensions(શૂન્ય)
{
	જો (sbi_dbtr_get_total_triggers() == 0)
		પાછા 0;

	પાછા sbi_ecall_register_extension(&ecall_dbtr);
}

સંરચના sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
