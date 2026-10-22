/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#شامل <sbi/riscv_encoding.h>
#شامل <sbi/riscv_asm.h>
#شامل <sbi/sbi_vector.h>
#شامل <sbi/sbi_types.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_error.h>

size_t sbi_سمتیہ_context_سائز(خالی)
{
	واپس حجم(ساخت sbi_سمتیہ_context) + (32 * csr_read(CSR_VLENB));
}

خالی sbi_سمتیہ_save(ساخت sbi_سمتیہ_context *dst)
{
	غیرعلامتی طویل vlenb, mstatus_orig;

	اگر (!dst)
		واپس;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_سیٹ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#تعریف_کرو SAVE_VREG(i)							\
	({								\
	asm متلون(							\
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

#تعریف_منسوخ SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

خالی sbi_سمتیہ_restore(ثابت ساخت sbi_سمتیہ_context *src)
{
	غیرعلامتی طویل vlenb, mstatus_orig;

	اگر (!src)
		واپس;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_سیٹ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#تعریف_کرو RESTORE_VREG(i)							\
	({								\
	asm متلون(							\
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
#تعریف_منسوخ RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
