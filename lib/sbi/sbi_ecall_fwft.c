/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_fwft.h>
#شامل <sbi/sbi_trap.h>

ساکن صحیح sbi_ecall_fwft_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				 ساخت sbi_trap_regs *regs,
				 ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_FWFT_سیٹ:
		ret = sbi_fwft_سیٹ(regs->a0, regs->a1, regs->a2);
		توڑو;
	حالت SBI_EXT_FWFT_GET:
		ret = sbi_fwft_get(regs->a0, &برآمد->قدر);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
		توڑو;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_fwft;

ساکن صحیح sbi_ecall_fwft_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_fwft);
}

ساخت sbi_ecall_extension ecall_fwft = {
	.name			= "fwft",
	.extid_آغاز		= SBI_EXT_FWFT,
	.extid_انتہا		= SBI_EXT_FWFT,
	.رجسٹر_extensions	= sbi_ecall_fwft_رجسٹر_extensions,
	.handle			= sbi_ecall_fwft_handler,
};
