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

ساکن صحیح sbi_ecall_dbtr_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	غیرعلامتی طویل smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_سرکاؤ;
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_DBTR_عدد_TRIGGERS:
		ret = sbi_dbtr_عدد_trig(regs->a0, &برآمد->قدر);
		توڑو;
	حالت SBI_EXT_DBTR_تیاری_SHMEM:
		ret = sbi_dbtr_تیاری_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &برآمد->قدر);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		توڑو;
	حالت SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	};

	واپس ret;
}

ساخت sbi_ecall_extension ecall_dbtr;

ساکن صحیح sbi_ecall_dbtr_رجسٹر_extensions(خالی)
{
	اگر (sbi_dbtr_get_کل_triggers() == 0)
		واپس 0;

	واپس sbi_ecall_رجسٹر_extension(&ecall_dbtr);
}

ساخت sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_آغاز		= SBI_EXT_DBTR,
	.extid_انتہا		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.رجسٹر_extensions	= sbi_ecall_dbtr_رجسٹر_extensions,
};
