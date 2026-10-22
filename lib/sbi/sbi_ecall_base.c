/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#شامل <sbi/sbi_ecall.h>
#شامل <sbi/sbi_ecall_interface.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/sbi_version.h>
#شامل <sbi/riscv_asm.h>

ساکن صحیح sbi_ecall_بنیاد_probe(غیرعلامتی طویل extid, غیرعلامتی طویل *برآمد_val)
{
	ساخت sbi_ecall_extension *ext;

	ext = sbi_ecall_ڈھونڈو_extension(extid);
	اگر (!ext) {
		*برآمد_val = 0;
		واپس 0;
	}

	اگر (ext->probe)
		واپس ext->probe(extid, برآمد_val);

	*برآمد_val = 1;
	واپس 0;
}

ساکن صحیح sbi_ecall_بنیاد_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				  ساخت sbi_trap_regs *regs,
				  ساخت sbi_ecall_return *برآمد)
{
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_بنیاد_GET_SPEC_نسخہ:
		برآمد->قدر = (SBI_ECALL_نسخہ_MAJOR <<
			      SBI_SPEC_نسخہ_MAJOR_انحراف) &
			     (SBI_SPEC_نسخہ_MAJOR_نقاب <<
			      SBI_SPEC_نسخہ_MAJOR_انحراف);
		برآمد->قدر = برآمد->قدر | SBI_ECALL_نسخہ_MINOR;
		توڑو;
	حالت SBI_EXT_بنیاد_GET_IMP_ID:
		برآمد->قدر = sbi_ecall_get_impid();
		توڑو;
	حالت SBI_EXT_بنیاد_GET_IMP_نسخہ:
		برآمد->قدر = OPENSBI_نسخہ;
		توڑو;
	حالت SBI_EXT_بنیاد_GET_MVENDORID:
		برآمد->قدر = csr_read(CSR_MVENDORID);
		توڑو;
	حالت SBI_EXT_بنیاد_GET_MARCHID:
		برآمد->قدر = csr_read(CSR_MARCHID);
		توڑو;
	حالت SBI_EXT_بنیاد_GET_MIMPID:
		برآمد->قدر = csr_read(CSR_MIMPID);
		توڑو;
	حالت SBI_EXT_بنیاد_PROBE_EXT:
		ret = sbi_ecall_بنیاد_probe(regs->a0, &برآمد->قدر);
		توڑو;
	طےشدہ:
		ret = SBI_ENOTSUPP;
	}

	واپس ret;
}

ساخت sbi_ecall_extension ecall_بنیاد;

ساکن صحیح sbi_ecall_بنیاد_رجسٹر_extensions(خالی)
{
	واپس sbi_ecall_رجسٹر_extension(&ecall_بنیاد);
}

ساخت sbi_ecall_extension ecall_بنیاد = {
	.name			= "base",
	.extid_آغاز		= SBI_EXT_بنیاد,
	.extid_انتہا		= SBI_EXT_بنیاد,
	.رجسٹر_extensions	= sbi_ecall_بنیاد_رجسٹر_extensions,
	.handle			= sbi_ecall_بنیاد_handler,
};
