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
#അന്തര്ഭാവ <sbi/sbi_trap.h>
#അന്തര്ഭാവ <sbi/sbi_version.h>
#അന്തര്ഭാവ <sbi/riscv_asm.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	ഘടന sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	എങ്കിൽ (!ext) {
		*out_val = 0;
		തിരികെ 0;
	}

	എങ്കിൽ (ext->probe)
		തിരികെ ext->probe(extid, out_val);

	*out_val = 1;
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  ഘടന sbi_trap_regs *regs,
				  ഘടന sbi_ecall_return *out)
{
	പൂർണ്ണസംഖ്യ ret = 0;

	തിരഞ്ഞെടുപ്പ് (funcid) {
	അവസ്ഥ SBI_EXT_BASE_GET_SPEC_VERSION:
		out->മൂല്യം = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->മൂല്യം = out->മൂല്യം | SBI_ECALL_VERSION_MINOR;
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_GET_IMP_ID:
		out->മൂല്യം = sbi_ecall_get_impid();
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_GET_IMP_VERSION:
		out->മൂല്യം = OPENSBI_VERSION;
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_GET_MVENDORID:
		out->മൂല്യം = csr_read(CSR_MVENDORID);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_GET_MARCHID:
		out->മൂല്യം = csr_read(CSR_MARCHID);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_GET_MIMPID:
		out->മൂല്യം = csr_read(CSR_MIMPID);
		നിർത്തുക;
	അവസ്ഥ SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->മൂല്യം);
		നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
		ret = SBI_ENOTSUPP;
	}

	തിരികെ ret;
}

ഘടന sbi_ecall_extension ecall_base;

സ്ഥിതം പൂർണ്ണസംഖ്യ sbi_ecall_base_register_extensions(ശൂന്യം)
{
	തിരികെ sbi_ecall_register_extension(&ecall_base);
}

ഘടന sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
