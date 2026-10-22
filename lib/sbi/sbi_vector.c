/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2026 RISCstar Solutions.
 *
 * Authors:
 *	 Dave Patel <dave.patel@riscstar.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_encoding.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_vector.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_types.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hart.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>

size_t sbi_ಸದಿಶ_context_ಗಾತ್ರ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ sbi_ಸದಿಶ_context) + (32 * csr_read(CSR_VLENB));
}

ಶೂನ್ಯ sbi_ಸದಿಶ_save(ರಚನೆ sbi_ಸದಿಶ_context *dst)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vlenb, mstatus_orig;

	ಆದರೆ (!dst)
		ಹಿಂತಿರುಗಿ;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_ಗಣ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Save CSRs */
	dst->vcsr = csr_read(vcsr);
	dst->vstart = csr_read(vstart);

	/* Step 3: Save vector registers */
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_VREG(i)							\
	({								\
	asm ಅಸ್ಥಿರ(							\
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

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SAVE_VREG

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}

ಶೂನ್ಯ sbi_ಸದಿಶ_restore(ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_ಸದಿಶ_context *src)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ vlenb, mstatus_orig;

	ಆದರೆ (!src)
		ಹಿಂತಿರುಗಿ;

	/* Step 1. Save original mstatus and Enable VS */
	mstatus_orig = csr_read_ಗಣ(CSR_MSTATUS, MSTATUS_VS);
	vlenb = csr_read(CSR_VLENB);

	/* Step 2: Restore vector registers */
#ವ್ಯಾಖ್ಯಾನಿಸು RESTORE_VREG(i)							\
	({								\
	asm ಅಸ್ಥಿರ(							\
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
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು RESTORE_VREG

	/* Step 3: Restore CSR's last */
	/* Restore CSRs first */
	csr_write(vcsr,   src->vcsr);
	csr_write(vstart, src->vstart);

	/* Step 4. Restore original mstatus LAST */
	csr_write(CSR_MSTATUS, mstatus_orig);
}
