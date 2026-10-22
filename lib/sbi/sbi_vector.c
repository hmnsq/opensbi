/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#అన్తర్భావ <sbi/riscv_encoding.h>
#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/sbi_vector.h>
#అన్తర్భావ <sbi/sbi_types.h>
#అన్తర్భావ <sbi/sbi_hart.h>
#అన్తర్భావ <sbi/sbi_error.h>

size_t sbi_సదిశ_context_కొలత(శూన్య)
{
	తిరిగి పరిమాణం(నిర్మాణం sbi_సదిశ_context) + (32 * csr_read(CSR_VLENB));
}

శూన్య sbi_సదిశ_save(నిర్మాణం sbi_సదిశ_context *dst)
{
	గుర్తులేని దీర్ఘ vlenb, mstatus_orig;

	ఒకవేళ (!dst)
		తిరిగి;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_సమితి(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#నిర్వచించు SAVE_VREG(i)							\
	({								\
	asm అస్థిర(							\
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

#నిర్వచనం_రద్దు SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

శూన్య sbi_సదిశ_restore(స్థిరరాశి నిర్మాణం sbi_సదిశ_context *src)
{
	గుర్తులేని దీర్ఘ vlenb, mstatus_orig;

	ఒకవేళ (!src)
		తిరిగి;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_సమితి(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#నిర్వచించు RESTORE_VREG(i)							\
	({								\
	asm అస్థిర(							\
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
#నిర్వచనం_రద్దు RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
