/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_platform.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned long sbi_ecall_vendor_id(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    ସଂରଚନା sbi_trap_regs *regs,
				    ସଂରଚନା sbi_ecall_return *out)
{
	ଫେରନ୍ତୁ sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

ସଂରଚନା sbi_ecall_extension ecall_vendor;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_vendor_register_extensions(ଶୂନ୍ୟ)
{
	unsigned long extid = sbi_ecall_vendor_id();

	ଯଦି (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ଫେରନ୍ତୁ 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_vendor);
}

ସଂରଚନା sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
