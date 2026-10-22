/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ecall_interface.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_platform.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_trap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sbi_ecall_vendor_id(ਖਾਲੀ)
{
	ਵਾਪਸ SBI_EXT_VENDOR_ਸ਼ੁਰੂ +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_ਸਿਰਾ - SBI_EXT_VENDOR_ਸ਼ੁਰੂ));
}

ਸਥਿਰ ਪੂਰਨ sbi_ecall_vendor_handler(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ funcid,
				    ਬਣਤਰ sbi_trap_regs *regs,
				    ਬਣਤਰ sbi_ecall_return *ਬਾਹਰਲਾ)
{
	ਵਾਪਸ sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, ਬਾਹਰਲਾ);
}

ਬਣਤਰ sbi_ecall_extension ecall_vendor;

ਸਥਿਰ ਪੂਰਨ sbi_ecall_vendor_ਰਜਿਸਟਰ_extensions(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ extid = sbi_ecall_vendor_id();

	ਜੇ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		ਵਾਪਸ 0;

	ecall_vendor.extid_ਸ਼ੁਰੂ = extid;
	ecall_vendor.extid_ਸਿਰਾ = extid;

	ਵਾਪਸ sbi_ecall_ਰਜਿਸਟਰ_extension(&ecall_vendor);
}

ਬਣਤਰ sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_ਸ਼ੁਰੂ		= SBI_EXT_VENDOR_ਸ਼ੁਰੂ,
	.extid_ਸਿਰਾ		= SBI_EXT_VENDOR_ਸਿਰਾ,
	.ਰਜਿਸਟਰ_extensions	= sbi_ecall_vendor_ਰਜਿਸਟਰ_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
