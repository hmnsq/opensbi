/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_dbtr.h>

స్థిర పూర్ణాంకం sbi_ecall_dbtr_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				  నిర్మాణం sbi_trap_regs *regs,
				  నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	గుర్తులేని దీర్ఘ smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_జరుపు;
	పూర్ణాంకం ret = 0;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_DBTR_అంకె_TRIGGERS:
		ret = sbi_dbtr_అంకె_trig(regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	స్థితి SBI_EXT_DBTR_అమరిక_SHMEM:
		ret = sbi_dbtr_అమరిక_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &బహిర్గతం->విలువ);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		ఆపు;
	స్థితి SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	};

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_dbtr;

స్థిర పూర్ణాంకం sbi_ecall_dbtr_రిజిస్టరు_extensions(శూన్య)
{
	ఒకవేళ (sbi_dbtr_get_మొత్తం_triggers() == 0)
		తిరిగి 0;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_dbtr);
}

నిర్మాణం sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_ప్రారంభం		= SBI_EXT_DBTR,
	.extid_చివర		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.రిజిస్టరు_extensions	= sbi_ecall_dbtr_రిజిస్టరు_extensions,
};
