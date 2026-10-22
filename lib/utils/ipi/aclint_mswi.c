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

ಸ್ಥಿರ unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, mswi_ptr_offset, (__mswi))

ಸ್ಥಿರ ಶೂನ್ಯ mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	ಸಂರಚನಾ sbi_scratch *scratch;
	ಸಂರಚನಾ aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ಯದಿ (!scratch)
		ಪ್ರತ್ಯಾಗಮ;

	mswi = mswi_get_hart_data_ptr(scratch);
	ಯದಿ (!mswi)
		ಪ್ರತ್ಯಾಗಮ;

	/* Set ACLINT IPI */
	msip = (ಶೂನ್ಯ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

ಸ್ಥಿರ ಶೂನ್ಯ mswi_ipi_clear(ಶೂನ್ಯ)
{
	u32 *msip;
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	ಯದಿ (!mswi)
		ಪ್ರತ್ಯಾಗಮ;

	/* Clear ACLINT IPI */
	msip = (ಶೂನ್ಯ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

ಪೂರ್ಣಾಙ್ಕ aclint_mswi_cold_init(ಸಂರಚನಾ aclint_mswi_data *mswi)
{
	u32 i;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ sbi_scratch *scratch;

	/* Sanity checks */
	ಯದಿ (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_ಎಣಿಕೆ * ಪರಿಮಾಣ(u32))) ||
	    (!mswi->hart_ಎಣಿಕೆ || mswi->hart_ಎಣಿಕೆ > ACLINT_MSWI_MAX_HARTS))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಯದಿ (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!mswi_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	ಪ್ರತಿ (i = 0; i < mswi->hart_ಎಣಿಕೆ; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ಯದಿ (!scratch)
			ಅನುವರ್ತ;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ಬೇರು_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sbi_ipi_add_device(&aclint_mswi);

	ಪ್ರತ್ಯಾಗಮ 0;
}
