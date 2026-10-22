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

ساكن صحيح sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	بنية sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	𐤀𐤌 (!ext) {
		*out_val = 0;
		أرجع 0;
	}

	𐤀𐤌 (ext->probe)
		أرجع ext->probe(extid, out_val);

	*out_val = 1;
	أرجع 0;
}

ساكن صحيح sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  بنية sbi_trap_regs *regs,
				  بنية sbi_ecall_return *out)
{
	صحيح ret = 0;

	بدل (funcid) {
	حالة SBI_EXT_BASE_GET_SPEC_VERSION:
		out->value = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->value = out->value | SBI_ECALL_VERSION_MINOR;
		اكسر;
	حالة SBI_EXT_BASE_GET_IMP_ID:
		out->value = sbi_ecall_get_impid();
		اكسر;
	حالة SBI_EXT_BASE_GET_IMP_VERSION:
		out->value = OPENSBI_VERSION;
		اكسر;
	حالة SBI_EXT_BASE_GET_MVENDORID:
		out->value = csr_read(CSR_MVENDORID);
		اكسر;
	حالة SBI_EXT_BASE_GET_MARCHID:
		out->value = csr_read(CSR_MARCHID);
		اكسر;
	حالة SBI_EXT_BASE_GET_MIMPID:
		out->value = csr_read(CSR_MIMPID);
		اكسر;
	حالة SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->value);
		اكسر;
	افتراضي:
		ret = SBI_ENOTSUPP;
	}

	أرجع ret;
}

بنية sbi_ecall_extension ecall_base;

ساكن صحيح sbi_ecall_base_register_extensions(فراغ)
{
	أرجع sbi_ecall_register_extension(&ecall_base);
}

بنية sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
