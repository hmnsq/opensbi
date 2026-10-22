/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#समावेश <sbi/sbi_ecall.h>
#समावेश <sbi/sbi_ecall_interface.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_trap.h>
#समावेश <sbi/riscv_asm.h>

अटल अंतरभूत अचिन्हित दीर्घ sbi_ecall_vendor_id(व्योम)
{
	वापस SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

अटल पूर्णांक sbi_ecall_vendor_handler(अचिन्हित दीर्घ extid, अचिन्हित दीर्घ funcid,
				    काष्ठा sbi_trap_regs *regs,
				    काष्ठा sbi_ecall_return *out)
{
	वापस sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

काष्ठा sbi_ecall_extension ecall_vendor;

अटल पूर्णांक sbi_ecall_vendor_register_extensions(व्योम)
{
	अचिन्हित दीर्घ extid = sbi_ecall_vendor_id();

	अगर (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		वापस 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	वापस sbi_ecall_register_extension(&ecall_vendor);
}

काष्ठा sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
