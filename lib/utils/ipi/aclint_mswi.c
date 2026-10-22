/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_ipi.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/ipi/aclint_mswi.h>

ସ୍ଥିର unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, mswi_ptr_offset, (__mswi))

ସ୍ଥିର ଶୂନ୍ୟ mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	ସଂରଚନା sbi_scratch *scratch;
	ସଂରଚନା aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	ଯଦି (!scratch)
		ଫେରନ୍ତୁ;

	mswi = mswi_get_hart_data_ptr(scratch);
	ଯଦି (!mswi)
		ଫେରନ୍ତୁ;

	/* Set ACLINT IPI */
	msip = (ଶୂନ୍ୟ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

ସ୍ଥିର ଶୂନ୍ୟ mswi_ipi_clear(ଶୂନ୍ୟ)
{
	u32 *msip;
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	ଯଦି (!mswi)
		ଫେରନ୍ତୁ;

	/* Clear ACLINT IPI */
	msip = (ଶୂନ୍ୟ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

ସ୍ଥିର ସଂରଚନା sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା aclint_mswi_cold_init(ସଂରଚନା aclint_mswi_data *mswi)
{
	u32 i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା sbi_scratch *scratch;

	/* Sanity checks */
	ଯଦି (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_ଗଣନା * ଆକାର(u32))) ||
	    (!mswi->hart_ଗଣନା || mswi->hart_ଗଣନା > ACLINT_MSWI_MAX_HARTS))
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ଯଦି (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!mswi_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	ପାଇଁ (i = 0; i < mswi->hart_ଗଣନା; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ଯଦି (!scratch)
			ଜାରି;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ମୂଳ_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sbi_ipi_add_device(&aclint_mswi);

	ଫେରନ୍ତୁ 0;
}
