/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_fwft.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 ഘടന sbi_trap_regs *regs,
				 ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
		നിർത്തുക;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_fwft;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_fwft_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_fwft);
}

ഘടന sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
