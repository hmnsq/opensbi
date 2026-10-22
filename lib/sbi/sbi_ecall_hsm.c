/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/riscv_asm.h>

ساکن صحیح sbi_ecall_hsm_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				 ساخت sbi_trap_regs *regs,
				 ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_سرکاؤ;

	انتخاب (funcid) {
	حالت SBI_EXT_HSM_HART_آغاز:
		ret = sbi_hsm_hart_آغاز(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		توڑو;
	حالت SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		توڑو;
	حالت SBI_EXT_HSM_HART_GET_حالت:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		توڑو;
	حالت SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	اگر (ret >= 0) {
		برآمد->قدر = ret;
		ret = 0;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_hsm;

ساکن صحیح sbi_ecall_hsm_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_hsm);
}

ساخت sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_آغاز		= SBI_EXT_HSM,
	.extid_انتہا		= SBI_EXT_HSM,
	.رجسٹر_extensions	= sbi_ecall_hsm_رجسٹر_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
