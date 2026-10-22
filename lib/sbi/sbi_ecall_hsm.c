/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/riscv_asm.h>

நிலையான முழு sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 கட்டமைப்பு sbi_trap_regs *regs,
				 கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	தேர்வு (funcid) {
	நிலை SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		நிறுத்து;
	நிலை SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		நிறுத்து;
	நிலை SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}

	எனில் (ret >= 0) {
		out->மதிப்பு = ret;
		ret = 0;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_hsm;

நிலையான முழு sbi_ecall_hsm_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_hsm);
}

கட்டமைப்பு sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
