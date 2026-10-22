/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_platform.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>

স্থির অন্তঃস্থ unsigned long sbi_ecall_vendor_id(শূন্য)
{
	ঘূৰাই SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

স্থির পূর্ণাঙ্ক sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    সংরচনা sbi_trap_regs *regs,
				    সংরচনা sbi_ecall_return *out)
{
	ঘূৰাই sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

সংরচনা sbi_ecall_extension ecall_vendor;

স্থির পূর্ণাঙ্ক sbi_ecall_vendor_register_extensions(শূন্য)
{
	unsigned long extid = sbi_ecall_vendor_id();

	যদি (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ঘূৰাই 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	ঘূৰাই sbi_ecall_register_extension(&ecall_vendor);
}

সংরচনা sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
