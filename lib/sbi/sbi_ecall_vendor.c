/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_platform.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/riscv_asm.h>

நிலையான உள்வரி unsigned long sbi_ecall_vendor_id(வெறுமை)
{
	திரும்பு SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

நிலையான முழு sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    கட்டமைப்பு sbi_trap_regs *regs,
				    கட்டமைப்பு sbi_ecall_return *out)
{
	திரும்பு sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

கட்டமைப்பு sbi_ecall_extension ecall_vendor;

நிலையான முழு sbi_ecall_vendor_register_extensions(வெறுமை)
{
	unsigned long extid = sbi_ecall_vendor_id();

	எனில் (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		திரும்பு 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	திரும்பு sbi_ecall_register_extension(&ecall_vendor);
}

கட்டமைப்பு sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
