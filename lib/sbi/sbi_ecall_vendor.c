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

ساکن اندرونی غیرعلامتی طویل sbi_ecall_vendor_id(خالی)
{
	واپس SBI_EXT_VENDOR_آغاز +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_انتہا - SBI_EXT_VENDOR_آغاز));
}

ساکن صحیح sbi_ecall_vendor_handler(غیرعلامتی طویل extid, غیرعلامتی طویل funcid,
				    ساخت sbi_trap_regs *regs,
				    ساخت sbi_ecall_return *برآمد)
{
	واپس sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, برآمد);
}

ساخت sbi_ecall_extension ecall_vendor;

ساکن صحیح sbi_ecall_vendor_رجسٹر_extensions(خالی)
{
	غیرعلامتی طویل extid = sbi_ecall_vendor_id();

	اگر (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		واپس 0;

	ecall_vendor.extid_آغاز = extid;
	ecall_vendor.extid_انتہا = extid;

	واپس sbi_ecall_رجسٹر_extension(&ecall_vendor);
}

ساخت sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_آغاز		= SBI_EXT_VENDOR_آغاز,
	.extid_انتہا		= SBI_EXT_VENDOR_انتہا,
	.رجسٹر_extensions	= sbi_ecall_vendor_رجسٹر_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
