/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#అన్తర్భావ <sbi/sbi_ecall.h>
#అన్తర్భావ <sbi/sbi_ecall_interface.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_platform.h>
#అన్తర్భావ <sbi/sbi_trap.h>
#అన్తర్భావ <sbi/riscv_asm.h>

స్థిర అంతఃవరుస గుర్తులేని దీర్ఘ sbi_ecall_vendor_id(శూన్య)
{
	తిరిగి SBI_EXT_VENDOR_ప్రారంభం +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_చివర - SBI_EXT_VENDOR_ప్రారంభం));
}

స్థిర పూర్ణాంకం sbi_ecall_vendor_handler(గుర్తులేని దీర్ఘ extid, గుర్తులేని దీర్ఘ funcid,
				    నిర్మాణం sbi_trap_regs *regs,
				    నిర్మాణం sbi_ecall_return *బహిర్గతం)
{
	తిరిగి sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, బహిర్గతం);
}

నిర్మాణం sbi_ecall_extension ecall_vendor;

స్థిర పూర్ణాంకం sbi_ecall_vendor_రిజిస్టరు_extensions(శూన్య)
{
	గుర్తులేని దీర్ఘ extid = sbi_ecall_vendor_id();

	ఒకవేళ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		తిరిగి 0;

	ecall_vendor.extid_ప్రారంభం = extid;
	ecall_vendor.extid_చివర = extid;

	తిరిగి sbi_ecall_రిజిస్టరు_extension(&ecall_vendor);
}

నిర్మాణం sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_ప్రారంభం		= SBI_EXT_VENDOR_ప్రారంభం,
	.extid_చివర		= SBI_EXT_VENDOR_చివర,
	.రిజిస్టరు_extensions	= sbi_ecall_vendor_రిజిస్టరు_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
