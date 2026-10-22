/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#અન્તર્ભાવ <sbi/sbi_ecall.h>
#અન્તર્ભાવ <sbi/sbi_ecall_interface.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>

સ્થિર અંતઃરેખ unsigned long sbi_ecall_vendor_id(શૂન્ય)
{
	પાછા SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

સ્થિર પૂર્ણાંક sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    સંરચના sbi_trap_regs *regs,
				    સંરચના sbi_ecall_return *out)
{
	પાછા sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

સંરચના sbi_ecall_extension ecall_vendor;

સ્થિર પૂર્ણાંક sbi_ecall_vendor_register_extensions(શૂન્ય)
{
	unsigned long extid = sbi_ecall_vendor_id();

	જો (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		પાછા 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	પાછા sbi_ecall_register_extension(&ecall_vendor);
}

સંરચના sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
