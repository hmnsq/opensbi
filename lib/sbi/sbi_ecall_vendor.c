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

ساكن مضمن بیعلامت دراز sbi_ecall_vendor_id(فراغ)
{
	أرجع SBI_EXT_VENDOR_شروع +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_انتها - SBI_EXT_VENDOR_شروع));
}

ساكن صحيح sbi_ecall_vendor_handler(بیعلامت دراز extid, بیعلامت دراز funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *بیرون)
{
	أرجع sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, بیرون);
}

بنية sbi_ecall_extension ecall_vendor;

ساكن صحيح sbi_ecall_vendor_ثبات_extensions(فراغ)
{
	بیعلامت دراز extid = sbi_ecall_vendor_id();

	إذا (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		أرجع 0;

	ecall_vendor.extid_شروع = extid;
	ecall_vendor.extid_انتها = extid;

	أرجع sbi_ecall_ثبات_extension(&ecall_vendor);
}

بنية sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_شروع		= SBI_EXT_VENDOR_شروع,
	.extid_انتها		= SBI_EXT_VENDOR_انتها,
	.ثبات_extensions	= sbi_ecall_vendor_ثبات_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
