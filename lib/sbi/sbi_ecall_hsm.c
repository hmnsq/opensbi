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

ایستا صحیح sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 ساختار sbi_trap_regs *regs,
				 ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	انتخاب (funcid) {
	حالت SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		بشکن;
	حالت SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		بشکن;
	حالت SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		بشکن;
	حالت SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	اگر (ret >= 0) {
		out->مقدار = ret;
		ret = 0;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_hsm;

ایستا صحیح sbi_ecall_hsm_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_hsm);
}

ساختار sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
