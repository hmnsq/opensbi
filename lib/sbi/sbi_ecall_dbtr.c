/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_dbtr.h>

ساكن صحيح sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_DBTR_لمب_TRIGGERS:
		ret = sbi_dbtr_لمب_trig(regs->a0, &out->درج);
		اكسر;
	حالة SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->درج);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		اكسر;
	حالة SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	};

	أرجع ret;
}

بنية sbi_ecall_extension ecall_dbtr;

ساكن صحيح sbi_ecall_dbtr_register_extensions(فراغ)
{
	إذا (sbi_dbtr_get_total_triggers() == 0)
		أرجع 0;

	أرجع sbi_ecall_register_extension(&ecall_dbtr);
}

بنية sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
