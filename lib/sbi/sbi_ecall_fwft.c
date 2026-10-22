/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_fwft.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>

ਸਥਿਰ ਪੂਰਨ sbi_ecall_fwft_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				 ਬਣਤਰ sbi_trap_regs *regs,
				 ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਪੂਰਨ ret = 0;

	ਚੋਣ (funcid) {
	ਹਾਲਤ SBI_EXT_FWFT_ਸੈੱਟ:
		ret = sbi_fwft_ਸੈੱਟ(regs->a0, regs->a1, regs->a2);
		ਰੋਕੋ;
	ਹਾਲਤ SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &ਬਾਹਰਲਾ->ਮੁੱਲ);
		ਰੋਕੋ;
	ਮੂਲ:
		ret = SBI_ENOTSUPP;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}

ਬਣਤਰ sbi_ecall_extension ecall_fwft;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_fwft_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_fwft);
}

ਬਣਤਰ sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_FWFT,
	.extid_ਸਿਰਾ		= SBI_EXT_FWFT,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_fwft_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
