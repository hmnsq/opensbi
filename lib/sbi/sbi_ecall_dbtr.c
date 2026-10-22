/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_dbtr.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_dbtr_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				  ರಚನೆ sbi_trap_regs *regs,
				  ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_ಸರಿಸು;
	ಪೂರ್ಣಾಂಕ ret = 0;

	ಆಯ್ಕೆ (funcid) {
	ಸ್ಥಿತಿ SBI_EXT_DBTR_ಅಂಕೆ_TRIGGERS:
		ret = sbi_dbtr_ಅಂಕೆ_trig(regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_ಸಿದ್ಧತೆ_SHMEM:
		ret = sbi_dbtr_ಸಿದ್ಧತೆ_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &ಹೊರಹೋಗು->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
		ret = SBI_ENOTSUPP;
	};

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ sbi_ecall_extension ecall_dbtr;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_dbtr_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಆದರೆ (sbi_dbtr_get_ಒಟ್ಟು_triggers() == 0)
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_dbtr);
}

ರಚನೆ sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_ಆರಂಭ		= SBI_EXT_DBTR,
	.extid_ಕೊನೆ		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_dbtr_ರಿಜಿಸ್ಟರ್_extensions,
};
