/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/riscv_asm.h>

స్థిర పూర్ణాంకం sbi_ecall_hsm_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				 నిర్మాణం sbi_trap_regs *regs,
				 నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	పూర్ణాంకం ret = 0;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_జరుపు;

	ఎంపిక (funcid) {
	స్థితి SBI_EXT_HSM_HART_ప్రారంభం:
		ret = sbi_hsm_hart_ప్రారంభం(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		ఆపు;
	స్థితి SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		ఆపు;
	స్థితి SBI_EXT_HSM_HART_GET_స్థితి:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		ఆపు;
	స్థితి SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		ఆపు;
	అప్రమేయ:
		ret = SBI_ENOTSUPP;
	}

	ఒకవేళ (ret >= 0) {
		బహిర్గతం->విలువ = ret;
		ret = 0;
	}

	తిరిగి ret;
}

నిర్మాణం sbi_ecall_extension ecall_hsm;

స్థిర పూర్ణాంకం sbi_ecall_hsm_రిజిస్టరు_extensions(శూన్య)
{
	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_hsm);
}

నిర్మాణం sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_ప్రారంభం		= SBI_EXT_HSM,
	.extid_చివర		= SBI_EXT_HSM,
	.రిజిస్టరు_extensions	= sbi_ecall_hsm_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
