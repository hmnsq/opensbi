/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#অন্তর্ভাব <sbi/riscv_encoding.h>
#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/sbi_vector.h>
#অন্তর্ভাব <sbi/sbi_types.h>
#অন্তর্ভাব <sbi/sbi_hart.h>
#অন্তর্ভাব <sbi/sbi_error.h>

size_t sbi_vector_context_আকার(শূন্য)
{
	ফেরত আকার(কাঠামো sbi_vector_context) + (32 * csr_read(CSR_VLENB));
}

শূন্য sbi_vector_save(কাঠামো sbi_vector_context *dst)
{
	unsigned long vlenb, mstatus_orig;

	যদি (!dst)
		ফেরত;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_set(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#define SAVE_VREG(i)							\
	({								\
	asm অস্থির(							\
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

#undef SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

শূন্য sbi_vector_restore(ধ্রুবক কাঠামো sbi_vector_context *src)
{
	unsigned long vlenb, mstatus_orig;

	যদি (!src)
		ফেরত;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_set(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#define RESTORE_VREG(i)							\
	({								\
	asm অস্থির(							\
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
#undef RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
