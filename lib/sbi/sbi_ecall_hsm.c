/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_hsm_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				 ਬਣਤਰ sbi_trap_regs *regs,
				 ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_ਖਿਸਕਾਓ;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_HSM_HART_ਸ਼ੁਰੂ:
		ret = sbi_hsm_hart_ਸ਼ੁਰੂ(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_HSM_HART_GET_ਸਥਿਤੀ:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
	}

	ਜੇ (ret >= 0) {
		ਬਾਹਰਲਾ->ਮੁੱਲ = ret;
		ret = 0;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_hsm;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_hsm_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_hsm);
}

ਬਣਤਰ sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_HSM,
	.extid_ਸਿਰਾ		= SBI_EXT_HSM,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_hsm_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
