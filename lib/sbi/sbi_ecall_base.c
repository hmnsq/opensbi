/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ecall_interface.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_trap.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_version.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	ସଂରଚନା sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	ଯଦି (!ext) {
		*out_val = 0;
		ଫେରନ୍ତୁ 0;
	}

	ଯଦି (ext->probe)
		ଫେରନ୍ତୁ ext->probe(extid, out_val);

	*out_val = 1;
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  ସଂରଚନା sbi_trap_regs *regs,
				  ସଂରଚନା sbi_ecall_return *out)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

	ବାଛ (funcid) {
	ସ୍ଥିତି SBI_EXT_BASE_GET_SPEC_VERSION:
		out->ମୂଲ୍ୟ = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->ମୂଲ୍ୟ = out->ମୂଲ୍ୟ | SBI_ECALL_VERSION_MINOR;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_GET_IMP_ID:
		out->ମୂଲ୍ୟ = sbi_ecall_get_impid();
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_GET_IMP_VERSION:
		out->ମୂଲ୍ୟ = OPENSBI_VERSION;
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_GET_MVENDORID:
		out->ମୂଲ୍ୟ = csr_read(CSR_MVENDORID);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_GET_MARCHID:
		out->ମୂଲ୍ୟ = csr_read(CSR_MARCHID);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_GET_MIMPID:
		out->ମୂଲ୍ୟ = csr_read(CSR_MIMPID);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = SBI_ENOTSUPP;
	}

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା sbi_ecall_extension ecall_base;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbi_ecall_base_register_extensions(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ sbi_ecall_register_extension(&ecall_base);
}

ସଂରଚନା sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
