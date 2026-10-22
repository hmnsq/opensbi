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

ساكن صحيح sbi_ecall_fwft_handler(بنانشانی لما extid, بنانشانی لما funcid,
				 بنية sbi_trap_regs *regs,
				 بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_FWFT_سیٹ:
		ret = sbi_fwft_سیٹ(regs->a0, regs->a1, regs->a2);
		اكسر;
	حالة SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &باہرلا->قدر);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
		اكسر;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_fwft;

ساكن صحيح sbi_ecall_fwft_رجسٹر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٹر_extension(&ecall_fwft);
}

بنية sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_شروع		= SBI_EXT_FWFT,
	.extid_سرا		= SBI_EXT_FWFT,
	.رجسٹر_extensions	= sbi_ecall_fwft_رجسٹر_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
