/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ضمن <sbi/sbi_ecall.h>
#ضمن <sbi/sbi_ecall_interface.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/sbi_version.h>
#ضمن <sbi/riscv_asm.h>

ساكن صحيح sbi_ecall_ادھار_probe(بنانشانی لما extid, بنانشانی لما *باہرلا_val)
{
	بنية sbi_ecall_extension *ext;

	ext = sbi_ecall_لبھو_extension(extid);
	إذا (!ext) {
		*باہرلا_val = 0;
		أرجع 0;
	}

	إذا (ext->probe)
		أرجع ext->probe(extid, باہرلا_val);

	*باہرلا_val = 1;
	أرجع 0;
}

ساكن صحيح sbi_ecall_ادھار_handler(بنانشانی لما extid, بنانشانی لما funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *باہرلا)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_ادھار_GET_SPEC_نسخہ:
		باہرلا->قدر = (SBI_ECALL_نسخہ_MAJOR <<
			      SBI_SPEC_نسخہ_MAJOR_وتھ) &
			     (SBI_SPEC_نسخہ_MAJOR_نقاب <<
			      SBI_SPEC_نسخہ_MAJOR_وتھ);
		باہرلا->قدر = باہرلا->قدر | SBI_ECALL_نسخہ_MINOR;
		اكسر;
	حالة SBI_EXT_ادھار_GET_IMP_ID:
		باہرلا->قدر = sbi_ecall_get_impid();
		اكسر;
	حالة SBI_EXT_ادھار_GET_IMP_نسخہ:
		باہرلا->قدر = OPENSBI_نسخہ;
		اكسر;
	حالة SBI_EXT_ادھار_GET_MVENDORID:
		باہرلا->قدر = csr_read(CSR_MVENDORID);
		اكسر;
	حالة SBI_EXT_ادھار_GET_MARCHID:
		باہرلا->قدر = csr_read(CSR_MARCHID);
		اكسر;
	حالة SBI_EXT_ادھار_GET_MIMPID:
		باہرلا->قدر = csr_read(CSR_MIMPID);
		اكسر;
	حالة SBI_EXT_ادھار_PROBE_EXT:
		ret = sbi_ecall_ادھار_probe(regs->a0, &باہرلا->قدر);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_ادھار;

ساكن صحيح sbi_ecall_ادھار_رجسٹر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٹر_extension(&ecall_ادھار);
}

بنية sbi_ecall_extension ecall_ادھار = {
	.name			= "base",
	.extid_شروع		= SBI_EXT_ادھار,
	.extid_سرا		= SBI_EXT_ادھار,
	.رجسٹر_extensions	= sbi_ecall_ادھار_رجسٹر_extensions,
	.handle			= sbi_ecall_ادھار_handler,
};
