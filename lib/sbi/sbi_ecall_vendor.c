/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#അന്തര്ഭാവ <sbi/sbi_ecall.h>
#അന്തര്ഭാവ <sbi/sbi_ecall_interface.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_platform.h>
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>

സ്ഥിതം വരിയിൽ unsigned long sbi_ecall_vendor_id(ശൂന്യം)
{
	തിരികെ SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    ഘടന sbi_trap_regs *regs,
				    ഘടന sbi_ecall_return *out)
{
	തിരികെ sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

ഘടന sbi_ecall_extension ecall_vendor;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_vendor_register_extensions(ശൂന്യം)
{
	unsigned long extid = sbi_ecall_vendor_id();

	എങ്കിൽ (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		തിരികെ 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	തിരികെ sbi_ecall_register_extension(&ecall_vendor);
}

ഘടന sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
