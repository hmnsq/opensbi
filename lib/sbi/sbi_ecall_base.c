/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_version.h>
#include <sbi/riscv_asm.h>

static puur_naa_nka sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	sa_nracanaa sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	ꦪꦼꦤ꧀ (!ext) {
		*out_val = 0;
		ꦮꦭꦶ 0;
	}

	ꦪꦼꦤ꧀ (ext->probe)
		ꦮꦭꦶ ext->probe(extid, out_val);

	*out_val = 1;
	ꦮꦭꦶ 0;
}

static puur_naa_nka sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  sa_nracanaa sbi_trap_regs *regs,
				  sa_nracanaa sbi_ecall_return *out)
{
	puur_naa_nka ret = 0;

	switch (funcid) {
	pak_sa SBI_EXT_BASE_GET_SPEC_VERSION:
		out->value = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->value = out->value | SBI_ECALL_VERSION_MINOR;
		bha_nga;
	pak_sa SBI_EXT_BASE_GET_IMP_ID:
		out->value = sbi_ecall_get_impid();
		bha_nga;
	pak_sa SBI_EXT_BASE_GET_IMP_VERSION:
		out->value = OPENSBI_VERSION;
		bha_nga;
	pak_sa SBI_EXT_BASE_GET_MVENDORID:
		out->value = csr_read(CSR_MVENDORID);
		bha_nga;
	pak_sa SBI_EXT_BASE_GET_MARCHID:
		out->value = csr_read(CSR_MARCHID);
		bha_nga;
	pak_sa SBI_EXT_BASE_GET_MIMPID:
		out->value = csr_read(CSR_MIMPID);
		bha_nga;
	pak_sa SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->value);
		bha_nga;
	default:
		ret = SBI_ENOTSUPP;
	}

	ꦮꦭꦶ ret;
}

sa_nracanaa sbi_ecall_extension ecall_base;

static puur_naa_nka sbi_ecall_base_register_extensions(void)
{
	ꦮꦭꦶ sbi_ecall_register_extension(&ecall_base);
}

sa_nracanaa sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
