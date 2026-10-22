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

স্থির পূর্ণসংখ্যা sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 কাঠামো sbi_trap_regs *regs,
				 কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		বিরতি;
	ক্ষেত্র SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->মান);
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
		বিরতি;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_fwft;

স্থির পূর্ণসংখ্যা sbi_ecall_fwft_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_fwft);
}

কাঠামো sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
