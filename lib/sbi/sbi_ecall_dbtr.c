/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_dbtr.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_dbtr_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				  ਬਣਤਰ sbi_trap_regs *regs,
				  ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_ਖਿਸਕਾਓ;
	ਪੂਰਨ ret = 0;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_DBTR_ਅੰਕ_TRIGGERS:
		ret = sbi_dbtr_ਅੰਕ_trig(regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_ਤਿਆਰੀ_SHMEM:
		ret = sbi_dbtr_ਤਿਆਰੀ_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	};

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_dbtr;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_dbtr_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਜੇ (sbi_dbtr_get_ਕੁੱਲ_triggers() == 0)
		ਵਾਪਸ 0;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_dbtr);
}

ਬਣਤਰ sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_DBTR,
	.extid_ਸਿਰਾ		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_dbtr_ਰਜਿਸਟਰ_extensions,
};
