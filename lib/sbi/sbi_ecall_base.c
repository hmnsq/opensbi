/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাব <sbi/sbi_ecall.h>
#অন্তর্ভাব <sbi/sbi_ecall_interface.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/sbi_version.h>
#অন্তর্ভাব <sbi/riscv_asm.h>

স্থির পূর্ণসংখ্যা sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	কাঠামো sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	যদি (!ext) {
		*out_val = 0;
		ফেরত 0;
	}

	যদি (ext->probe)
		ফেরত ext->probe(extid, out_val);

	*out_val = 1;
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  কাঠামো sbi_trap_regs *regs,
				  কাঠামো sbi_ecall_return *out)
{
	পূর্ণসংখ্যা ret = 0;

	নির্বাচন (funcid) {
	ক্ষেত্র SBI_EXT_BASE_GET_SPEC_VERSION:
		out->মান = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->মান = out->মান | SBI_ECALL_VERSION_MINOR;
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_GET_IMP_ID:
		out->মান = sbi_ecall_get_impid();
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_GET_IMP_VERSION:
		out->মান = OPENSBI_VERSION;
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_GET_MVENDORID:
		out->মান = csr_read(CSR_MVENDORID);
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_GET_MARCHID:
		out->মান = csr_read(CSR_MARCHID);
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_GET_MIMPID:
		out->মান = csr_read(CSR_MIMPID);
		বিরতি;
	ক্ষেত্র SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->মান);
		বিরতি;
	পূর্বনির্ধারিত:
		ret = SBI_ENOTSUPP;
	}

	ফেরত ret;
}

কাঠামো sbi_ecall_extension ecall_base;

স্থির পূর্ণসংখ্যা sbi_ecall_base_register_extensions(শূন্য)
{
	ফেরত sbi_ecall_register_extension(&ecall_base);
}

কাঠামো sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
