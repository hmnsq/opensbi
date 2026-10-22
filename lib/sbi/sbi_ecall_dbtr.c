/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_dbtr.h>

ایستا صحیح sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_DBTR_رقم_TRIGGERS:
		ret = sbi_dbtr_رقم_trig(regs->a0, &out->مقدار);
		بشکن;
	حالت SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->مقدار);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		بشکن;
	حالت SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	};

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_dbtr;

ایستا صحیح sbi_ecall_dbtr_register_extensions(تهی)
{
	اگر (sbi_dbtr_get_total_triggers() == 0)
		بازگشت 0;

	بازگشت sbi_ecall_register_extension(&ecall_dbtr);
}

ساختار sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
