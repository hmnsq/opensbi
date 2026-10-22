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
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_trap.h>
#شامل <sbi/riscv_asm.h>

ایستا درونخطی unsigned long sbi_ecall_vendor_id(تهی)
{
	بازگشت SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

ایستا صحیح sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    ساختار sbi_trap_regs *regs,
				    ساختار sbi_ecall_return *out)
{
	بازگشت sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

ساختار sbi_ecall_extension ecall_vendor;

ایستا صحیح sbi_ecall_vendor_register_extensions(تهی)
{
	unsigned long extid = sbi_ecall_vendor_id();

	اگر (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		بازگشت 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	بازگشت sbi_ecall_register_extension(&ecall_vendor);
}

ساختار sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
