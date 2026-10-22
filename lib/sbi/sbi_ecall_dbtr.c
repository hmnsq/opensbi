/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_dbtr.h>

अटल पूर्णांक sbi_ecall_dbtr_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				  काष्ठा sbi_trap_regs *regs,
				  काष्ठा sbi_ecall_return *out)
{
	अचिन्हित दीर्घ smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;
	पूर्णांक ret = 0;

	चयन (funcid) {
	हाल SBI_EXT_DBTR_अंक_TRIGGERS:
		ret = sbi_dbtr_अंक_trig(regs->a0, &out->मूल्य);
		अवरोध;
	हाल SBI_EXT_DBTR_SETUP_SHMEM:
		ret = sbi_dbtr_setup_shmem(sbi_domain_thishart_ptr(), smode,
					   regs->a0, regs->a1);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_READ:
		ret = sbi_dbtr_read_trig(smode, regs->a0, regs->a1);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_INSTALL:
		ret = sbi_dbtr_install_trig(smode, regs->a0, &out->मूल्य);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_UNINSTALL:
		ret = sbi_dbtr_uninstall_trig(regs->a0, regs->a1);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_ENABLE:
		ret = sbi_dbtr_enable_trig(regs->a0, regs->a1);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_UPDATE:
		ret = sbi_dbtr_update_trig(smode, regs->a0);
		अवरोध;
	हाल SBI_EXT_DBTR_TRIGGER_DISABLE:
		ret = sbi_dbtr_disable_trig(regs->a0, regs->a1);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	};

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_dbtr;

अटल पूर्णांक sbi_ecall_dbtr_register_extensions(व्योम)
{
	अगर (sbi_dbtr_get_योग_triggers() == 0)
		वापस 0;

	वापस sbi_ecall_register_extension(&ecall_dbtr);
}

काष्ठा sbi_ecall_extension ecall_dbtr = {
	.name			= "dbtr",
	.extid_start		= SBI_EXT_DBTR,
	.extid_end		= SBI_EXT_DBTR,
	.handle			= sbi_ecall_dbtr_handler,
	.register_extensions	= sbi_ecall_dbtr_register_extensions,
};
