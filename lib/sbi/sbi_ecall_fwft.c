/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_fwft.h>
#அந்தர்பாவ <sbi/sbi_trap.h>

நிலையான முழு sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 கட்டமைப்பு sbi_trap_regs *regs,
				 கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;

	தேர்வு (funcid) {
	நிலை SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		நிறுத்து;
	நிலை SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->மதிப்பு);
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
		நிறுத்து;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_fwft;

நிலையான முழு sbi_ecall_fwft_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_fwft);
}

கட்டமைப்பு sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
