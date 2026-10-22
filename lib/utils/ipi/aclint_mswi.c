/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_ipi.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/ipi/aclint_mswi.h>

સ્થિર unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, mswi_ptr_offset, (__mswi))

સ્થિર શૂન્ય mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	સંરચના sbi_scratch *scratch;
	સંરચના aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	જો (!scratch)
		પાછા;

	mswi = mswi_get_hart_data_ptr(scratch);
	જો (!mswi)
		પાછા;

	/* Set ACLINT IPI */
	msip = (શૂન્ય *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

સ્થિર શૂન્ય mswi_ipi_clear(શૂન્ય)
{
	u32 *msip;
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	જો (!mswi)
		પાછા;

	/* Clear ACLINT IPI */
	msip = (શૂન્ય *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

સ્થિર સંરચના sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

પૂર્ણાંક aclint_mswi_cold_init(સંરચના aclint_mswi_data *mswi)
{
	u32 i;
	પૂર્ણાંક rc;
	સંરચના sbi_scratch *scratch;

	/* Sanity checks */
	જો (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_ગણતરી * કદ(u32))) ||
	    (!mswi->hart_ગણતરી || mswi->hart_ગણતરી > ACLINT_MSWI_MAX_HARTS))
		પાછા SBI_EINVAL;

	/* Allocate scratch space pointer */
	જો (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!mswi_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	માટે (i = 0; i < mswi->hart_ગણતરી; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		જો (!scratch)
			ચાલુ;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_મૂળ_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	જો (rc)
		પાછા rc;

	sbi_ipi_add_device(&aclint_mswi);

	પાછા 0;
}
