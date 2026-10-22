/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#ضمن <sbi/riscv_encoding.h>
#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/sbi_vector.h>
#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_error.h>

size_t sbi_ويڪٽر_context_سائيز(فراغ)
{
	أرجع حجم(بنية sbi_ويڪٽر_context) + (32 * csr_read(CSR_VLENB));
}

فراغ sbi_ويڪٽر_save(بنية sbi_ويڪٽر_context *dst)
{
	بنانشان ڊگهو vlenb, mstatus_orig;

	إذا (!dst)
		أرجع;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_سيٽ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#وصف_ڪريو SAVE_VREG(i)							\
	({								\
	asm متطاير(							\
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

#وصف_رد SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

فراغ sbi_ويڪٽر_restore(ثابت بنية sbi_ويڪٽر_context *src)
{
	بنانشان ڊگهو vlenb, mstatus_orig;

	إذا (!src)
		أرجع;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_سيٽ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#وصف_ڪريو RESTORE_VREG(i)							\
	({								\
	asm متطاير(							\
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
#وصف_رد RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
