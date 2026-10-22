/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_dbtr.h>

স্থির পূর্ণসংখ্যা sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	পূর্ণসংখ্যা ret = 0;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_DBTR_অঙ্ক_TRIGGERS:
		ret = sbi_dbtr_অঙ্ক_trig(regs->a0, &out->মান);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->মান);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	};

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_dbtr;

স্থির পূর্ণসংখ্যা sbi_ecall_dbtr_register_extensions(শূন্য)
{
	যদি (sbi_dbtr_get_যোগ_triggers() == 0)
		ফেরত 0;

	ফেরত sbi_ecall_register_extension(&ecall_dbtr);
}

কাঠামো sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
