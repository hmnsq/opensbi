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

ایستا صحیح sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	ساختار sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	اگر (!ext) {
		*out_val = 0;
		بازگشت 0;
	}

	اگر (ext->probe)
		بازگشت ext->probe(extid, out_val);

	*out_val = 1;
	بازگشت 0;
}

ایستا صحیح sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  ساختار sbi_trap_regs *regs,
				  ساختار sbi_ecall_return *out)
{
	صحیح ret = 0;

	انتخاب (funcid) {
	حالت SBI_EXT_BASE_GET_SPEC_VERSION:
		out->مقدار = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->مقدار = out->مقدار | SBI_ECALL_VERSION_MINOR;
		بشکن;
	حالت SBI_EXT_BASE_GET_IMP_ID:
		out->مقدار = sbi_ecall_get_impid();
		بشکن;
	حالت SBI_EXT_BASE_GET_IMP_VERSION:
		out->مقدار = OPENSBI_VERSION;
		بشکن;
	حالت SBI_EXT_BASE_GET_MVENDORID:
		out->مقدار = csr_read(CSR_MVENDORID);
		بشکن;
	حالت SBI_EXT_BASE_GET_MARCHID:
		out->مقدار = csr_read(CSR_MARCHID);
		بشکن;
	حالت SBI_EXT_BASE_GET_MIMPID:
		out->مقدار = csr_read(CSR_MIMPID);
		بشکن;
	حالت SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->مقدار);
		بشکن;
	پیشفرض:
		ret = SBI_ENOTSUPP;
	}

	بازگشت ret;
}

ساختار sbi_ecall_extension ecall_base;

ایستا صحیح sbi_ecall_base_register_extensions(تهی)
{
	بازگشت sbi_ecall_register_extension(&ecall_base);
}

ساختار sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
