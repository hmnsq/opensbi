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
#અન્તર્ભાવ <sbi/sbi_trap.h>
#અન્તર્ભાવ <sbi/sbi_version.h>
#અન્તર્ભાવ <sbi/riscv_asm.h>

સ્થિર પૂર્ણાંક sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	સંરચના sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	જો (!ext) {
		*out_val = 0;
		પાછા 0;
	}

	જો (ext->probe)
		પાછા ext->probe(extid, out_val);

	*out_val = 1;
	પાછા 0;
}

સ્થિર પૂર્ણાંક sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  સંરચના sbi_trap_regs *regs,
				  સંરચના sbi_ecall_return *out)
{
	પૂર્ણાંક ret = 0;

	પસંદગી (funcid) {
	સ્થિતિ SBI_EXT_BASE_GET_SPEC_VERSION:
		out->કિંમત = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->કિંમત = out->કિંમત | SBI_ECALL_VERSION_MINOR;
		તોડો;
	સ્થિતિ SBI_EXT_BASE_GET_IMP_ID:
		out->કિંમત = sbi_ecall_get_impid();
		તોડો;
	સ્થિતિ SBI_EXT_BASE_GET_IMP_VERSION:
		out->કિંમત = OPENSBI_VERSION;
		તોડો;
	સ્થિતિ SBI_EXT_BASE_GET_MVENDORID:
		out->કિંમત = csr_read(CSR_MVENDORID);
		તોડો;
	સ્થિતિ SBI_EXT_BASE_GET_MARCHID:
		out->કિંમત = csr_read(CSR_MARCHID);
		તોડો;
	સ્થિતિ SBI_EXT_BASE_GET_MIMPID:
		out->કિંમત = csr_read(CSR_MIMPID);
		તોડો;
	સ્થિતિ SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->કિંમત);
		તોડો;
	મૂળભૂત:
		ret = SBI_ENOTSUPP;
	}

	પાછા ret;
}

સંરચના sbi_ecall_extension ecall_base;

સ્થિર પૂર્ણાંક sbi_ecall_base_register_extensions(શૂન્ય)
{
	પાછા sbi_ecall_register_extension(&ecall_base);
}

સંરચના sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
