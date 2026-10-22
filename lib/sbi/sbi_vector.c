/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_encoding.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_vector.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hart.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>

size_t sbi_ਸਦਿਸ਼_context_ਮਾਪ(ਖਾਲੀ)
{
	ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ sbi_ਸਦਿਸ਼_context) + (32 * csr_read(CSR_VLENB));
}

ਖਾਲੀ sbi_ਸਦਿਸ਼_save(ਬਣਤਰ sbi_ਸਦਿਸ਼_context *dst)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vlenb, mstatus_orig;

	ਜੇ (!dst)
		ਵਾਪਸ;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_ਸੈੱਟ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_VREG(i)							\
	({								\
	asm ਅਸਥਿਰ(							\
		"	.option push\n\t"				\
		"	.option arch, +v\n\t"				\
		"	vs8r.v v" #i ", (%0)\n\t"			\
		"	.option pop\n\t"				\
		::	"r"(dst->vregs + (i) * vlenb)	: "memory");	\
	})								\

	SAVE_VREG(0);
	SAVE_VREG(8);
	SAVE_VREG(16);
	SAVE_VREG(24);

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

ਖਾਲੀ sbi_ਸਦਿਸ਼_restore(ਅਚਲ ਬਣਤਰ sbi_ਸਦਿਸ਼_context *src)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ vlenb, mstatus_orig;

	ਜੇ (!src)
		ਵਾਪਸ;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_ਸੈੱਟ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESTORE_VREG(i)							\
	({								\
	asm ਅਸਥਿਰ(							\
		"	.option push\n\t"				\
		"	.option arch, +v\n\t"				\
		"	vl8r.v v" #i ", (%0)\n\t"			\
		"	.option pop\n\t"				\
		::	"r"(src->vregs + (i) * vlenb) : "memory");	\
	 })								\

	RESTORE_VREG(0);
	RESTORE_VREG(8);
	RESTORE_VREG(16);
	RESTORE_VREG(24);
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
