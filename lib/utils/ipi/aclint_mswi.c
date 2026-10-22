/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_atomic.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_ipi.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/ipi/aclint_mswi.h>

স্থির unsigned long mswi_ptr_offset;

#define mswi_get_hart_তথ্য_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), শূন্য *, mswi_ptr_offset)

#define mswi_set_hart_তথ্য_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), শূন্য *, mswi_ptr_offset, (__mswi))

স্থির শূন্য mswi_ipi_send(u32 hart_সূচক)
{
	u32 *msip;
	কাঠামো sbi_scratch *scratch;
	কাঠামো aclint_mswi_তথ্য *mswi;

	scratch = sbi_hartindex_to_scratch(hart_সূচক);
	যদি (!scratch)
		ফেরত;

	mswi = mswi_get_hart_তথ্য_ptr(scratch);
	যদি (!mswi)
		ফেরত;

	/* Set ACLINT IPI */
	msip = (শূন্য *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_সূচক) -
			mswi->first_hartid]);
}

স্থির শূন্য mswi_ipi_clear(শূন্য)
{
	u32 *msip;
	কাঠামো sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	কাঠামো aclint_mswi_তথ্য *mswi;

	mswi = mswi_get_hart_তথ্য_ptr(scratch);
	যদি (!mswi)
		ফেরত;

	/* Clear ACLINT IPI */
	msip = (শূন্য *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

স্থির কাঠামো sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

পূর্ণসংখ্যা aclint_mswi_cold_init(কাঠামো aclint_mswi_তথ্য *mswi)
{
	u32 i;
	পূর্ণসংখ্যা rc;
	কাঠামো sbi_scratch *scratch;

	/* Sanity checks */
	যদি (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->আকার_2 < (mswi->hart_গুনতি * আকার(u32))) ||
	    (!mswi->hart_গুনতি || mswi->hart_গুনতি > ACLINT_MSWI_MAX_HARTS))
		ফেরত SBI_EINVAL;

	/* Allocate scratch space pointer */
	যদি (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(শূন্য *);
		যদি (!mswi_ptr_offset)
			ফেরত SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	জন্য (i = 0; i < mswi->hart_গুনতি; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		যদি (!scratch)
			চালু;
		mswi_set_hart_তথ্য_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_মূল_add_memrange(mswi->addr, mswi->আকার_2, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	যদি (rc)
		ফেরত rc;

	sbi_ipi_add_device(&aclint_mswi);

	ফেরত 0;
}
