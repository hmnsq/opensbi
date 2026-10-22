/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_fwft.h>
#অন্তর্ভাব <sbi/sbi_trap.h>

স্থির পূর্ণাঙ্ক sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 সংরচনা sbi_trap_regs *regs,
				 সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;

	বিকল্প (funcid) {
	পক্ষ SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		ভঙ্গ;
	পক্ষ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->মান);
		ভঙ্গ;
	স্বভাব:
		ret = SBI_ENOTSUPP;
		ভঙ্গ;
	}

	প্রত্যাগম ret;
}

সংরচনা sbi_ecall_extension ecall_fwft;

স্থির পূর্ণাঙ্ক sbi_ecall_fwft_register_extensions(শূন্য)
{
	প্রত্যাগম sbi_ecall_register_extension(&ecall_fwft);
}

সংরচনা sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
