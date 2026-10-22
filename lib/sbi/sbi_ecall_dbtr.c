/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_dbtr.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	പൂർണ്ണസംഖ്യ ret = 0;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_DBTR_അക്കം_TRIGGERS:
		ret = sbi_dbtr_അക്കം_trig(regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	};

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_dbtr;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_dbtr_register_extensions(ശൂന്യം)
{
	എങ്കിൽ (sbi_dbtr_get_total_triggers() == 0)
		തിരികെ 0;

	തിരികെ sbi_ecall_register_extension(&ecall_dbtr);
}

ഘടന sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
