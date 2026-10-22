/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_trap.h>
#include <sbi/riscv_asm.h>

static anta_hstha unsigned long sbi_ecall_vendor_id(void)
{
	ཕྱིར SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

static ཧྲིལ sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    sa_nracanaa sbi_trap_regs *regs,
				    sa_nracanaa sbi_ecall_return *out)
{
	ཕྱིར sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

sa_nracanaa sbi_ecall_extension ecall_vendor;

static ཧྲིལ sbi_ecall_vendor_register_extensions(void)
{
	unsigned long extid = sbi_ecall_vendor_id();

	གལ་ཏེ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ཕྱིར 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	ཕྱིར sbi_ecall_register_extension(&ecall_vendor);
}

sa_nracanaa sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
