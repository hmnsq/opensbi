/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_dbtr.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_DBTR_ଅଙ୍କ_TRIGGERS:
		ret = sbi_dbtr_ଅଙ୍କ_trig(regs->a0, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	};

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_dbtr;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_dbtr_register_extensions(ଶୂନ୍ୟ)
{
	ଯଦି (sbi_dbtr_get_total_triggers() == 0)
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_dbtr);
}

ସଂରଚନା sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
