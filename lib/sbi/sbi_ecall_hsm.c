/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/riscv_asm.h>

ساكن صحيح sbi_ecall_hsm_handler(بنانشانی لما extid, بنانشانی لما funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_کھسکاؤ;

	بدل (funcid) {
	حالة SBI_EXT_HSM_HART_شروع:
		ret = sbi_hsm_hart_شروع(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		اكسر;
	حالة SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		اكسر;
	حالة SBI_EXT_HSM_HART_GET_حالت:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		اكسر;
	حالة SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	إذا (ret >= 0) {
		باہرلا->قدر = ret;
		ret = 0;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_hsm;

ساكن صحيح sbi_ecall_hsm_رجسٹر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٹر_extension(&ecall_hsm);
}

بنية sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_شروع		= SBI_EXT_HSM,
	.extid_سرا		= SBI_EXT_HSM,
	.رجسٹر_extensions	= sbi_ecall_hsm_رجسٹر_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
