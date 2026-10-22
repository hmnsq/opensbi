/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাৱ <sbi/riscv_asm.h>
#অন্তর্ভাৱ <sbi/riscv_atomic.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_domain.h>
#অন্তর্ভাৱ <sbi/sbi_error.h>
#অন্তর্ভাৱ <sbi/sbi_ipi.h>
#অন্তর্ভাৱ <sbi/sbi_scratch.h>
#অন্তর্ভাৱ <sbi/sbi_timer.h>
#অন্তর্ভাৱ <sbi_utils/ipi/aclint_mswi.h>

স্থির unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), শূন্য *, mswi_ptr_offset, (__mswi))

স্থির শূন্য mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	সংরচনা sbi_scratch *scratch;
	সংরচনা aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	যদি (!scratch)
		ঘূৰাই;

	mswi = mswi_get_hart_data_ptr(scratch);
	যদি (!mswi)
		ঘূৰাই;

	/* Set ACLINT IPI */
	msip = (শূন্য *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

স্থির শূন্য mswi_ipi_clear(শূন্য)
{
	u32 *msip;
	সংরচনা sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	সংরচনা aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	যদি (!mswi)
		ঘূৰাই;

	/* Clear ACLINT IPI */
	msip = (শূন্য *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

স্থির সংরচনা sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

পূর্ণাঙ্ক aclint_mswi_cold_init(সংরচনা aclint_mswi_data *mswi)
{
	u32 i;
	পূর্ণাঙ্ক rc;
	সংরচনা sbi_scratch *scratch;

	/* Sanity checks */
	যদি (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_গুনতি * পরিমাণ(u32))) ||
	    (!mswi->hart_গুনতি || mswi->hart_গুনতি > ACLINT_MSWI_MAX_HARTS))
		ঘূৰাই SBI_EINVAL;

	/* Allocate scratch space pointer */
	যদি (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!mswi_ptr_offset)
			ঘূৰাই SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	বাবে (i = 0; i < mswi->hart_গুনতি; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		যদি (!scratch)
			অনুৱর্ত;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_মূল_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	যদি (rc)
		ঘূৰাই rc;

	sbi_ipi_add_device(&aclint_mswi);

	ঘূৰাই 0;
}
