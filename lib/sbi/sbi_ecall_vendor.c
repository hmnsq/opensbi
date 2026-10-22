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
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_trap.h>
#ضمن <sbi/riscv_asm.h>

ساكن مضمن unsigned long sbi_ecall_vendor_id(فراغ)
{
	إرجاع SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

ساكن صحيح sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *out)
{
	إرجاع sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

بنية sbi_ecall_extension ecall_vendor;

ساكن صحيح sbi_ecall_vendor_register_extensions(فراغ)
{
	unsigned long extid = sbi_ecall_vendor_id();

	إذا (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		إرجاع 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	إرجاع sbi_ecall_register_extension(&ecall_vendor);
}

بنية sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
