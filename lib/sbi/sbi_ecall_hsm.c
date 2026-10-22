/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_hsm.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 ഘടന sbi_trap_regs *regs,
				 ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ulong smode = (csr_read(CSR_MSTATUS) & MSTATUS_MPP) >>
			MSTATUS_MPP_SHIFT;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, sbi_domain_thishart_ptr(),
					 regs->a0, regs->a1, smode, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_HSM_HART_GET_STATUS:
		ret = sbi_hsm_hart_get_state(sbi_domain_thishart_ptr(),
					     regs->a0);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_HSM_HART_SUSPEND:
		ret = sbi_hsm_hart_suspend(scratch, regs->a0, regs->a1,
					   smode, regs->a2);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	എങ്കിൽ (ret >= 0) {
		out->മൂല്യം = ret;
		ret = 0;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_hsm;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_hsm_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_hsm);
}

ഘടന sbi_ecall_extension ecall_hsm = {
	.name			= "hsm",
	.extid_start		= SBI_EXT_HSM,
	.extid_end		= SBI_EXT_HSM,
	.register_extensions	= sbi_ecall_hsm_register_extensions,
	.handle			= sbi_ecall_hsm_handler,
};
