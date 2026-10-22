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

ساكن مضمن بنانشان ڊگهو sbi_ecall_vendor_id(فراغ)
{
	أرجع SBI_EXT_VENDOR_شروعات +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_پڇاڙي - SBI_EXT_VENDOR_شروعات));
}

ساكن صحيح sbi_ecall_vendor_handler(بنانشان ڊگهو extid, بنانشان ڊگهو funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *ٻاهريون)
{
	أرجع sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, ٻاهريون);
}

بنية sbi_ecall_extension ecall_vendor;

ساكن صحيح sbi_ecall_vendor_رجسٽر_extensions(فراغ)
{
	بنانشان ڊگهو extid = sbi_ecall_vendor_id();

	إذا (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		أرجع 0;

	ecall_vendor.extid_شروعات = extid;
	ecall_vendor.extid_پڇاڙي = extid;

	أرجع sbi_ecall_رجسٽر_extension(&ecall_vendor);
}

بنية sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_شروعات		= SBI_EXT_VENDOR_شروعات,
	.extid_پڇاڙي		= SBI_EXT_VENDOR_پڇاڙي,
	.رجسٽر_extensions	= sbi_ecall_vendor_رجسٽر_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
