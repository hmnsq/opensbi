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

স্থির পূর্ণাঙ্ক sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 সংরচনা sbi_trap_regs *regs,
				 সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	বিকল্প (funcid) {
	পক্ষ SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		ভঙ্গ;
	পক্ষ SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		ভঙ্গ;
	পক্ষ SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		ভঙ্গ;
	স্বভাব:
		ret = SBI_ENOTSUPP;
	}

	যদি (ret >= 0) {
		out->মান = ret;
		ret = 0;
	}

	প্রত্যাগম ret;
}

সংরচনা sbi_ecall_extension ecall_hsm;

স্থির পূর্ণাঙ্ক sbi_ecall_hsm_register_extensions(শূন্য)
{
	প্রত্যাগম sbi_ecall_register_extension(&ecall_hsm);
}

সংরচনা sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
