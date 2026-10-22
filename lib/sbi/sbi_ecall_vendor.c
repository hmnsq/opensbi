/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#अन्तर्भाव <sbi/sbi_ecall.h>
#अन्तर्भाव <sbi/sbi_ecall_interface.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_platform.h>
#अन्तर्भाव <sbi/sbi_trap.h>
#अन्तर्भाव <sbi/riscv_asm.h>

स्थिर अन्तःस्थ unsigned long sbi_ecall_vendor_id(शून्य)
{
	प्रत्यागम SBI_EXT_VENDOR_START +
		(csr_read(CSR_MVENDORID) &
		 (SBI_EXT_VENDOR_END - SBI_EXT_VENDOR_START));
}

स्थिर पूर्णाङ्क sbi_ecall_vendor_handler(unsigned long extid, unsigned long funcid,
				    संरचना sbi_trap_regs *regs,
				    संरचना sbi_ecall_return *out)
{
	प्रत्यागम sbi_platform_vendor_ext_provider(sbi_platform_thishart_ptr(),
						funcid, regs, out);
}

संरचना sbi_ecall_extension ecall_vendor;

स्थिर पूर्णाङ्क sbi_ecall_vendor_register_extensions(शून्य)
{
	unsigned long extid = sbi_ecall_vendor_id();

	यदि (!sbi_platform_vendor_ext_check(sbi_platform_thishart_ptr()))
		प्रत्यागम 0;

	ecall_vendor.extid_start = extid;
	ecall_vendor.extid_end = extid;

	प्रत्यागम sbi_ecall_register_extension(&ecall_vendor);
}

संरचना sbi_ecall_extension ecall_vendor = {
	.name			= "vendor",
	.extid_start		= SBI_EXT_VENDOR_START,
	.extid_end		= SBI_EXT_VENDOR_END,
	.register_extensions	= sbi_ecall_vendor_register_extensions,
	.handle			= sbi_ecall_vendor_handler,
};
