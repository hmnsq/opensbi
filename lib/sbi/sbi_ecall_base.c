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

ساكن صحيح sbi_ecall_پایه_probe(بیعلامت دراز extid, بیعلامت دراز *بیرون_val)
{
	بنية sbi_ecall_extension *ext;

	ext = sbi_ecall_پیدا_کن_extension(extid);
	إذا (!ext) {
		*بیرون_val = 0;
		أرجع 0;
	}

	إذا (ext->probe)
		أرجع ext->probe(extid, بیرون_val);

	*بیرون_val = 1;
	أرجع 0;
}

ساكن صحيح sbi_ecall_پایه_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بیرون)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_پایه_GET_SPEC_نسخه:
		بیرون->مقدار = (SBI_ECALL_نسخه_MAJOR <<
			      SBI_SPEC_نسخه_MAJOR_جابه_جایی) &
			     (SBI_SPEC_نسخه_MAJOR_ماسک <<
			      SBI_SPEC_نسخه_MAJOR_جابه_جایی);
		بیرون->مقدار = بیرون->مقدار | SBI_ECALL_نسخه_MINOR;
		اكسر;
	حالة SBI_EXT_پایه_GET_IMP_ID:
		بیرون->مقدار = sbi_ecall_get_impid();
		اكسر;
	حالة SBI_EXT_پایه_GET_IMP_نسخه:
		بیرون->مقدار = OPENSBI_نسخه;
		اكسر;
	حالة SBI_EXT_پایه_GET_MVENDORID:
		بیرون->مقدار = csr_read(CSR_MVENDORID);
		اكسر;
	حالة SBI_EXT_پایه_GET_MARCHID:
		بیرون->مقدار = csr_read(CSR_MARCHID);
		اكسر;
	حالة SBI_EXT_پایه_GET_MIMPID:
		بیرون->مقدار = csr_read(CSR_MIMPID);
		اكسر;
	حالة SBI_EXT_پایه_PROBE_EXT:
		ret = sbi_ecall_پایه_probe(regs->a0, &بیرون->مقدار);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_پایه;

ساكن صحيح sbi_ecall_پایه_ثبات_extensions(فراغ)
{
	أرجع sbi_ecall_ثبات_extension(&ecall_پایه);
}

بنية sbi_ecall_extension ecall_پایه = {
	.name			= "base",
	.extid_شروع		= SBI_EXT_پایه,
	.extid_انتها		= SBI_EXT_پایه,
	.ثبات_extensions	= sbi_ecall_پایه_ثبات_extensions,
	.handle			= sbi_ecall_پایه_handler,
};
