/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_platform.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned long sbi_ecall_vendor_id(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    ಸಂರಚನಾ sbi_trap_regs *regs,
				    ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪ್ರತ್ಯಾಗಮ sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_vendor;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_vendor_register_extensions(ಶೂನ್ಯ)
{
	unsigned long extid = sbi_ecall_vendor_id();

	ಯದಿ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ಪ್ರತ್ಯಾಗಮ 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_vendor);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
