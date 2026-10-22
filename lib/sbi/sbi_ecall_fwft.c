/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_fwft.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>

સ્થિર પૂર્ણાંક sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 સંરચના sbi_trap_regs *regs,
				 સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		તોડો;
	સ્થિતિ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->કિંમત);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
		તોડો;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_fwft;

સ્થિર પૂર્ણાંક sbi_ecall_fwft_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_fwft);
}

સંરચના sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
