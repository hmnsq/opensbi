/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#অন্তর্ভাৱ <sbi/sbi_ecall.h>
#অন্তর্ভাৱ <sbi/sbi_ecall_interface.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_trap.h>
#অন্তর্ভাৱ <sbi/sbi_version.h>
#অন্তর্ভাৱ <sbi/riscv_asm.h>

স্থির পূর্ণাঙ্ক sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	সংরচনা sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	যদি (!ext) {
		*out_val = 0;
		ঘূৰাই 0;
	}

	যদি (ext->probe)
		ঘূৰাই ext->probe(extid, out_val);

	*out_val = 1;
	ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  সংরচনা sbi_trap_regs *regs,
				  সংরচনা sbi_ecall_return *out)
{
	পূর্ণাঙ্ক ret = 0;

	ৱিকল্প (funcid) {
	পক্ষ SBI_EXT_BASE_GET_SPEC_VERSION:
		out->মান = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->মান = out->মান | SBI_ECALL_VERSION_MINOR;
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_GET_IMP_ID:
		out->মান = sbi_ecall_get_impid();
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_GET_IMP_VERSION:
		out->মান = OPENSBI_VERSION;
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_GET_MVENDORID:
		out->মান = csr_read(CSR_MVENDORID);
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_GET_MARCHID:
		out->মান = csr_read(CSR_MARCHID);
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_GET_MIMPID:
		out->মান = csr_read(CSR_MIMPID);
		ভঙ্গ;
	পক্ষ SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->মান);
		ভঙ্গ;
	স্ৱভাৱ:
		ret = SBI_ENOTSUPP;
	}

	ঘূৰাই ret;
}

সংরচনা sbi_ecall_extension ecall_base;

স্থির পূর্ণাঙ্ক sbi_ecall_base_register_extensions(শূন্য)
{
	ঘূৰাই sbi_ecall_register_extension(&ecall_base);
}

সংরচনা sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
