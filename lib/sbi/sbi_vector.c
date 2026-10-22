/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#समावेश <sbi/riscv_encoding.h>
#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/sbi_vector.h>
#समावेश <sbi/sbi_types.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_error.h>

माप_प्रकार sbi_vector_context_आकार(व्योम)
{
	वापस माप(काष्ठा sbi_vector_context) + (32 * csr_read(CSR_VLENB));
}

व्योम sbi_vector_save(काष्ठा sbi_vector_context *dst)
{
	अचिन्हित दीर्घ vlenb, mstatus_orig;

	अगर (!dst)
		वापस;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_set(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#घोषणा SAVE_VREG(i)							\
	({								\
	यंत्र अस्थिर(							\
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

#अघोषित SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

व्योम sbi_vector_restore(स्थिर काष्ठा sbi_vector_context *src)
{
	अचिन्हित दीर्घ vlenb, mstatus_orig;

	अगर (!src)
		वापस;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_set(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#घोषणा RESTORE_VREG(i)							\
	({								\
	यंत्र अस्थिर(							\
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
#अघोषित RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
