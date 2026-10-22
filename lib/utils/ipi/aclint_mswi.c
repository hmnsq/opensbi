/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_ipi.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/ipi/aclint_mswi.h>

நிலையான unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), வெறுமை *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, mswi_ptr_offset, (__mswi))

நிலையான வெறுமை mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	கட்டமைப்பு sbi_scratch *scratch;
	கட்டமைப்பு aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	எனில் (!scratch)
		திரும்பு;

	mswi = mswi_get_hart_data_ptr(scratch);
	எனில் (!mswi)
		திரும்பு;

	/* Set ACLINT IPI */
	msip = (வெறுமை *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

நிலையான வெறுமை mswi_ipi_clear(வெறுமை)
{
	u32 *msip;
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	எனில் (!mswi)
		திரும்பு;

	/* Clear ACLINT IPI */
	msip = (வெறுமை *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

நிலையான கட்டமைப்பு sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

முழு aclint_mswi_cold_init(கட்டமைப்பு aclint_mswi_data *mswi)
{
	u32 i;
	முழு rc;
	கட்டமைப்பு sbi_scratch *scratch;

	/* Sanity checks */
	எனில் (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_மொத்தம் * அளவு(u32))) ||
	    (!mswi->hart_மொத்தம் || mswi->hart_மொத்தம் > ACLINT_MSWI_MAX_HARTS))
		திரும்பு SBI_EINVAL;

	/* Allocate scratch space pointer */
	எனில் (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!mswi_ptr_offset)
			திரும்பு SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	க்கு (i = 0; i < mswi->hart_மொத்தம்; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		எனில் (!scratch)
			தொடர்;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_வேர்_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	எனில் (rc)
		திரும்பு rc;

	sbi_ipi_add_device(&aclint_mswi);

	திரும்பு 0;
}
