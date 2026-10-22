/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_fwft.h>
#ضمن <sbi/sbi_trap.h>

ساكن صحيح sbi_ecall_fwft_handler(unsigned long extid, unsigned long funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *out)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_FWFT_SET:
		ret = sbi_fwft_set(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &out->value);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	ܦܢܐ ret;
}

بنية sbi_ecall_extension ecall_fwft;

ساكن صحيح sbi_ecall_fwft_register_extensions(فراغ)
{
	ܦܢܐ sbi_ecall_register_extension(&ecall_fwft);
}

بنية sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_start		= SBI_EXT_FWFT,
	.extid_end		= SBI_EXT_FWFT,
	.register_extensions	= sbi_ecall_fwft_register_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
