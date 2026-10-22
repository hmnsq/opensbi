/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#அந்தர்பாவ <sbi/sbi_ecall.h>
#அந்தர்பாவ <sbi/sbi_ecall_interface.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_trap.h>
#அந்தர்பாவ <sbi/sbi_version.h>
#அந்தர்பாவ <sbi/riscv_asm.h>

நிலையான முழு sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	கட்டமைப்பு sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	எனில் (!ext) {
		*out_val = 0;
		திரும்பு 0;
	}

	எனில் (ext->probe)
		திரும்பு ext->probe(extid, out_val);

	*out_val = 1;
	திரும்பு 0;
}

நிலையான முழு sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  கட்டமைப்பு sbi_trap_regs *regs,
				  கட்டமைப்பு sbi_ecall_return *out)
{
	முழு ret = 0;

	தேர்வு (funcid) {
	நிலை SBI_EXT_BASE_GET_SPEC_VERSION:
		out->மதிப்பு = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->மதிப்பு = out->மதிப்பு | SBI_ECALL_VERSION_MINOR;
		நிறுத்து;
	நிலை SBI_EXT_BASE_GET_IMP_ID:
		out->மதிப்பு = sbi_ecall_get_impid();
		நிறுத்து;
	நிலை SBI_EXT_BASE_GET_IMP_VERSION:
		out->மதிப்பு = OPENSBI_VERSION;
		நிறுத்து;
	நிலை SBI_EXT_BASE_GET_MVENDORID:
		out->மதிப்பு = csr_read(CSR_MVENDORID);
		நிறுத்து;
	நிலை SBI_EXT_BASE_GET_MARCHID:
		out->மதிப்பு = csr_read(CSR_MARCHID);
		நிறுத்து;
	நிலை SBI_EXT_BASE_GET_MIMPID:
		out->மதிப்பு = csr_read(CSR_MIMPID);
		நிறுத்து;
	நிலை SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->மதிப்பு);
		நிறுத்து;
	இயல்பு:
		ret = SBI_ENOTSUPP;
	}

	திரும்பு ret;
}

கட்டமைப்பு sbi_ecall_extension ecall_base;

நிலையான முழு sbi_ecall_base_register_extensions(வெறுமை)
{
	திரும்பு sbi_ecall_register_extension(&ecall_base);
}

கட்டமைப்பு sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
