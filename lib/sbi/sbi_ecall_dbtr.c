/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_dbtr.h>

நிலையான முழு sbi_ecall_dbtr_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	unsigned long smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	முழு ret = 0;

	தேர்வு (funcid) {
	நிலை SBI_EXT_DBTR_இலக்கம்_TRIGGERS:
		ret = sbi_dbtr_இலக்கம்_trig(regs->a0, &out->மதிப்பு);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->மதிப்பு);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	};

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_dbtr;

நிலையான முழு sbi_ecall_dbtr_register_extensions(வெறுமை)
{
	எனில் (sbi_dbtr_get_total_triggers() == 0)
		திரும்பு 0;

	திரும்பு sbi_ecall_register_extension(&ecall_dbtr);
}

கட்டமைப்பு sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
