/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/sbi_hsm.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर पूर्णाङ्क sbi_ecall_hsm_handler(अचिह्नित दीर्घ extid, अचिह्नित दीर्घ funcid,
				 संरचना sbi_trap_regs *regs,
				 संरचना sbi_ecall_return *बहिर्गमनम्)
{
	पूर्णाङ्क ret = 0;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_सरणम्;

	विकल्प (funcid) {
	पक्ष SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		भङ्ग;
	पक्ष SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		भङ्ग;
	पक्ष SBI_EXT_HSM_HART_GET_स्थितिः:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		भङ्ग;
	पक्ष SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		भङ्ग;
	स्वभाव:
		ret = SBI_ENOTSUPP;
	}

	यदि (ret >= 0) {
		बहिर्गमनम्->मानम् = ret;
		ret = 0;
	}

	प्रत्यागम ret;
}

संरचना sbi_ecall_extension ecall_hsm;

स्थिर पूर्णाङ्क sbi_ecall_hsm_पञ्जिकाकोशः_extensions(शून्य)
{
	प्रत्यागम sbi_ecall_पञ्जिकाकोशः_extension(&ecall_hsm);
}

संरचना sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_अन्तः		= SBI_EXT_HSM,
	.पञ्जिकाकोशः_extensions	= sbi_ecall_hsm_पञ्जिकाकोशः_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
