/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 ସଂରଚନା sbi_trap_regs *regs,
				 ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଯଦି (ret >= 0) {
		out->ମୂଲ୍ୟ = ret;
		ret = 0;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_hsm;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_hsm_register_extensions(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_hsm);
}

ସଂରଚନା sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
