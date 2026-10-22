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

ساكن صحيح sbi_ecall_بنياد_probe(بنانشان ڊگهو extid, بنانشان ڊگهو *ٻاهريون_val)
{
	بنية sbi_ecall_extension *ext;

	ext = sbi_ecall_ڳوليو_extension(extid);
	إذا (!ext) {
		*ٻاهريون_val = 0;
		أرجع 0;
	}

	إذا (ext->probe)
		أرجع ext->probe(extid, ٻاهريون_val);

	*ٻاهريون_val = 1;
	أرجع 0;
}

ساكن صحيح sbi_ecall_بنياد_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *ٻاهريون)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_بنياد_GET_SPEC_نسخو:
		ٻاهريون->قدر = (SBI_ECALL_نسخو_MAJOR <<
			      SBI_SPEC_نسخو_MAJOR_وٿي) &
			     (SBI_SPEC_نسخو_MAJOR_نقاب <<
			      SBI_SPEC_نسخو_MAJOR_وٿي);
		ٻاهريون->قدر = ٻاهريون->قدر | SBI_ECALL_نسخو_MINOR;
		اكسر;
	حالة SBI_EXT_بنياد_GET_IMP_ID:
		ٻاهريون->قدر = sbi_ecall_get_impid();
		اكسر;
	حالة SBI_EXT_بنياد_GET_IMP_نسخو:
		ٻاهريون->قدر = OPENSBI_نسخو;
		اكسر;
	حالة SBI_EXT_بنياد_GET_MVENDORID:
		ٻاهريون->قدر = csr_read(CSR_MVENDORID);
		اكسر;
	حالة SBI_EXT_بنياد_GET_MARCHID:
		ٻاهريون->قدر = csr_read(CSR_MARCHID);
		اكسر;
	حالة SBI_EXT_بنياد_GET_MIMPID:
		ٻاهريون->قدر = csr_read(CSR_MIMPID);
		اكسر;
	حالة SBI_EXT_بنياد_PROBE_EXT:
		ret = sbi_ecall_بنياد_probe(regs->a0, &ٻاهريون->قدر);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_بنياد;

ساكن صحيح sbi_ecall_بنياد_رجسٽر_extensions(فراغ)
{
	أرجع sbi_ecall_رجسٽر_extension(&ecall_بنياد);
}

بنية sbi_ecall_extension ecall_بنياد = {
	.name			= "base",
	.extid_شروعات		= SBI_EXT_بنياد,
	.extid_پڇاڙي		= SBI_EXT_بنياد,
	.رجسٽر_extensions	= sbi_ecall_بنياد_رجسٽر_extensions,
	.handle			= sbi_ecall_بنياد_handler,
};
