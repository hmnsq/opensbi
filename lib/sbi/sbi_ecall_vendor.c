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
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_trap.h>
#অন্তর্ভাব <sbi/riscv_asm.h>

স্থির অন্তঃরেখ unsigned long sbi_ecall_vendor_id(শূন্য)
{
	ফেরত SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

স্থির পূর্ণসংখ্যা sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    কাঠামো sbi_trap_regs *regs,
				    কাঠামো sbi_ecall_return *out)
{
	ফেরত sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

কাঠামো sbi_ecall_extension ecall_vendor;

স্থির পূর্ণসংখ্যা sbi_ecall_vendor_register_extensions(শূন্য)
{
	unsigned long extid = sbi_ecall_vendor_id();

	যদি (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ফেরত 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	ফেরত sbi_ecall_register_extension(&ecall_vendor);
}

কাঠামো sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
