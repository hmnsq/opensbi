/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_ipi.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/ipi/aclint_mswi.h>

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mswi_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು mswi_get_hart_ದತ್ತಾಂಶ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, mswi_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು mswi_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, mswi_ptr_ಸ್ಥಾನಾಂತರ, (__mswi))

ಸ್ಥಿರ ಶೂನ್ಯ mswi_ipi_send(u32 hart_ಸೂಚ್ಯಂಕ)
{
	u32 *msip;
	ರಚನೆ sbi_scratch *scratch;
	ರಚನೆ aclint_mswi_ದತ್ತಾಂಶ *mswi;

	scratch = sbi_hartindex_ಗಮ್ಯ_scratch(hart_ಸೂಚ್ಯಂಕ);
	ಆದರೆ (!scratch)
		ಹಿಂತಿರುಗಿ;

	mswi = mswi_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mswi)
		ಹಿಂತಿರುಗಿ;

	/* Set ACLINT IPI */
	msip = (ಶೂನ್ಯ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_ಗಮ್ಯ_hartid(hart_ಸೂಚ್ಯಂಕ) -
			mswi->ಮೊದಲ_hartid]);
}

ಸ್ಥಿರ ಶೂನ್ಯ mswi_ipi_clear(ಶೂನ್ಯ)
{
	u32 *msip;
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ aclint_mswi_ದತ್ತಾಂಶ *mswi;

	mswi = mswi_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mswi)
		ಹಿಂತಿರುಗಿ;

	/* Clear ACLINT IPI */
	msip = (ಶೂನ್ಯ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->ಮೊದಲ_hartid]);
}

ಸ್ಥಿರ ರಚನೆ sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

ಪೂರ್ಣಾಂಕ aclint_mswi_cold_init(ರಚನೆ aclint_mswi_ದತ್ತಾಂಶ *mswi)
{
	u32 i;
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ sbi_scratch *scratch;

	/* Sanity checks */
	ಆದರೆ (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->ಗಾತ್ರ_2 < (mswi->hart_ಎಣಿಕೆ * ಗಾತ್ರ(u32))) ||
	    (!mswi->hart_ಎಣಿಕೆ || mswi->hart_ಎಣಿಕೆ > ACLINT_MSWI_MAX_HARTS))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಆದರೆ (!mswi_ptr_ಸ್ಥಾನಾಂತರ) {
		mswi_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!mswi_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	ಗಾಗಿ (i = 0; i < mswi->hart_ಎಣಿಕೆ; i++) {
		scratch = sbi_hartid_ಗಮ್ಯ_scratch(mswi->ಮೊದಲ_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ಆದರೆ (!scratch)
			ಮುಂದುವರಿ;
		mswi_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mswi->addr, mswi->ಗಾತ್ರ_2, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_ipi_ಕೂಡಿಸು_device(&aclint_mswi);

	ಹಿಂತಿರುಗಿ 0;
}
