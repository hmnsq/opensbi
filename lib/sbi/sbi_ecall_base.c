/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ecall_interface.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_trap.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_version.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_base_probe(unsigned long extid, unsigned long *out_val)
{
	ಸಂರಚನಾ sbi_ecall_extension *ext;

	ext = sbi_ecall_find_extension(extid);
	ಯದಿ (!ext) {
		*out_val = 0;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಯದಿ (ext->probe)
		ಪ್ರತ್ಯಾಗಮ ext->probe(extid, out_val);

	*out_val = 1;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_base_handler(unsigned long extid, unsigned long funcid,
				  ಸಂರಚನಾ sbi_trap_regs *regs,
				  ಸಂರಚನಾ sbi_ecall_return *out)
{
	ಪೂರ್ಣಾಙ್ಕ ret = 0;

	ವಿಕಲ್ಪ (funcid) {
	ಪಕ್ಷ SBI_EXT_BASE_GET_SPEC_VERSION:
		out->ಮೌಲ್ಯ = (SBI_ECALL_VERSION_MAJOR <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET) &
			     (SBI_SPEC_VERSION_MAJOR_MASK <<
			      SBI_SPEC_VERSION_MAJOR_OFFSET);
		out->ಮೌಲ್ಯ = out->ಮೌಲ್ಯ | SBI_ECALL_VERSION_MINOR;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_GET_IMP_ID:
		out->ಮೌಲ್ಯ = sbi_ecall_get_impid();
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_GET_IMP_VERSION:
		out->ಮೌಲ್ಯ = OPENSBI_VERSION;
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_GET_MVENDORID:
		out->ಮೌಲ್ಯ = csr_read(CSR_MVENDORID);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_GET_MARCHID:
		out->ಮೌಲ್ಯ = csr_read(CSR_MARCHID);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_GET_MIMPID:
		out->ಮೌಲ್ಯ = csr_read(CSR_MIMPID);
		ಭಙ್ಗ;
	ಪಕ್ಷ SBI_EXT_BASE_PROBE_EXT:
		ret = sbi_ecall_base_probe(regs->a0, &out->ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಸ್ವಭಾವ:
		ret = SBI_ENOTSUPP;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ sbi_ecall_extension ecall_base;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sbi_ecall_base_register_extensions(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ sbi_ecall_register_extension(&ecall_base);
}

ಸಂರಚನಾ sbi_ecall_extension ecall_base = {
	.name			= "base",
	.extid_start		= SBI_EXT_BASE,
	.extid_end		= SBI_EXT_BASE,
	.register_extensions	= sbi_ecall_base_register_extensions,
	.handle			= sbi_ecall_base_handler,
};
