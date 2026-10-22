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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sbi_ecall_vendor_id(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ SBI_EXT_VENDOR_ಆರಂಭ +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_ಕೊನೆ - SBI_EXT_VENDOR_ಆರಂಭ));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_vendor_handler(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ funcid,
				    ರಚನೆ sbi_trap_regs *regs,
				    ರಚನೆ sbi_ecall_return *ಹೊರಹೋಗು)
{
	ಹಿಂತಿರುಗಿ sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, ಹೊರಹೋಗು);
}

ರಚನೆ sbi_ecall_extension ecall_vendor;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sbi_ecall_vendor_ರಿಜಿಸ್ಟರ್_extensions(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ extid = sbi_ecall_vendor_id();

	ಆದರೆ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ಹಿಂತಿರುಗಿ 0;

	ecall_vendor.extid_ಆರಂಭ = extid;
	ecall_vendor.extid_ಕೊನೆ = extid;

	ಹಿಂತಿರುಗಿ sbi_ecall_ರಿಜಿಸ್ಟರ್_extension(&ecall_vendor);
}

ರಚನೆ sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_ಆರಂಭ		= SBI_EXT_VENDOR_ಆರಂಭ,
	.extid_ಕೊನೆ		= SBI_EXT_VENDOR_ಕೊನೆ,
	.ರಿಜಿಸ್ಟರ್_extensions	= sbi_ecall_vendor_ರಿಜಿಸ್ಟರ್_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
