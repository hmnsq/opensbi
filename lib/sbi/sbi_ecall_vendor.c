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

ساكن مضمن بنانشانی لما sbi_ecall_vendor_id(فراغ)
{
	أرجع SBI_EXT_VENDOR_شروع +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_سرا - SBI_EXT_VENDOR_شروع));
}

ساكن صحيح sbi_ecall_vendor_handler(بنانشانی لما extid, بنانشانی لما funcid,
				    بنية sbi_trap_regs *regs,
				    بنية sbi_ecall_return *باہرلا)
{
	أرجع sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, باہرلا);
}

بنية sbi_ecall_extension ecall_vendor;

ساكن صحيح sbi_ecall_vendor_رجسٹر_extensions(فراغ)
{
	بنانشانی لما extid = sbi_ecall_vendor_id();

	إذا (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		أرجع 0;

	ecall_vendor.extid_شروع = extid;
	ecall_vendor.extid_سرا = extid;

	أرجع sbi_ecall_رجسٹر_extension(&ecall_vendor);
}

بنية sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_شروع		= SBI_EXT_VENDOR_شروع,
	.extid_سرا		= SBI_EXT_VENDOR_سرا,
	.رجسٹر_extensions	= sbi_ecall_vendor_رجسٹر_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
