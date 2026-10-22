/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/riscv_asm.h>

স্থির পূর্ণসংখ্যা sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 কাঠামো sbi_trap_regs *regs,
				 কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		বিরতি;
	ক্ষেত্র SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		বিরতি;
	ক্ষেত্র SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}

	যদি (ret >= 0) {
		out->মান = ret;
		ret = 0;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_hsm;

স্থির পূর্ণসংখ্যা sbi_ecall_hsm_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_hsm);
}

কাঠামো sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
