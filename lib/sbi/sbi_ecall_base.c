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

ساكن صحيح sbi_ecall_بنسټ_probe(بینښه اوږد extid, بینښه اوږد *بهر_val)
{
	بنية sbi_ecall_extension *ext;

	ext = sbi_ecall_پیدا_کړه_extension(extid);
	إذا (!ext) {
		*بهر_val = 0;
		أرجع 0;
	}

	إذا (ext->probe)
		أرجع ext->probe(extid, بهر_val);

	*بهر_val = 1;
	أرجع 0;
}

ساكن صحيح sbi_ecall_بنسټ_handler(بینښه اوږد extid, بینښه اوږد funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *بهر)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_بنسټ_GET_SPEC_نسخه:
		بهر->ارزښت = (SBI_ECALL_نسخه_MAJOR <<
			      SBI_SPEC_نسخه_MAJOR_دننه_واټن) &
			     (SBI_SPEC_نسخه_MAJOR_ماسک <<
			      SBI_SPEC_نسخه_MAJOR_دننه_واټن);
		بهر->ارزښت = بهر->ارزښت | SBI_ECALL_نسخه_MINOR;
		اكسر;
	حالة SBI_EXT_بنسټ_GET_IMP_ID:
		بهر->ارزښت = sbi_ecall_get_impid();
		اكسر;
	حالة SBI_EXT_بنسټ_GET_IMP_نسخه:
		بهر->ارزښت = OPENSBI_نسخه;
		اكسر;
	حالة SBI_EXT_بنسټ_GET_MVENDORID:
		بهر->ارزښت = csr_read(CSR_MVENDORID);
		اكسر;
	حالة SBI_EXT_بنسټ_GET_MARCHID:
		بهر->ارزښت = csr_read(CSR_MARCHID);
		اكسر;
	حالة SBI_EXT_بنسټ_GET_MIMPID:
		بهر->ارزښت = csr_read(CSR_MIMPID);
		اكسر;
	حالة SBI_EXT_بنسټ_PROBE_EXT:
		ret = sbi_ecall_بنسټ_probe(regs->a0, &بهر->ارزښت);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_بنسټ;

ساكن صحيح sbi_ecall_بنسټ_ثبتوونکی_extensions(فراغ)
{
	أرجع sbi_ecall_ثبتوونکی_extension(&ecall_بنسټ);
}

بنية sbi_ecall_extension ecall_بنسټ = {
	.name			= "base",
	.extid_پیل		= SBI_EXT_بنسټ,
	.extid_پای_ټکی		= SBI_EXT_بنسټ,
	.ثبتوونکی_extensions	= sbi_ecall_بنسټ_ثبتوونکی_extensions,
	.handle			= sbi_ecall_بنسټ_handler,
};
