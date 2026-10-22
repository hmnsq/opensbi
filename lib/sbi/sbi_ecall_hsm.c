/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/riscv_asm.h>

अटल पूर्णांक sbi_ecall_hsm_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				 काष्ठा sbi_trap_regs *regs,
				 काष्ठा sbi_ecall_return *out)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	चयन (funcid) {
	हाल SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		अवरोध;
	हाल SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		अवरोध;
	हाल SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		अवरोध;
	हाल SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		अवरोध;
	शेष:
		ret = SBI_ENOTSUPP;
	}

	अगर (ret >= 0) {
		out->मूल्य = ret;
		ret = 0;
	}

	वापस ret;
}

काष्ठा sbi_ecall_extension ecall_hsm;

अटल पूर्णांक sbi_ecall_hsm_register_extensions(व्योम)
{
	वापस sbi_ecall_register_extension(&ecall_hsm);
}

काष्ठा sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
