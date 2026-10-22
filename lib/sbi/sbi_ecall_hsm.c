/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>

સ્થિર પૂર્ણાંક sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 સંરચના sbi_trap_regs *regs,
				 સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		તોડો;
	સ્થિતિ SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		તોડો;
	સ્થિતિ SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	જો (ret >= 0) {
		out->કિંમત = ret;
		ret = 0;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_hsm;

સ્થિર પૂર્ણાંક sbi_ecall_hsm_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_hsm);
}

સંરચના sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
