/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/ipi/aclint_mswi.h>

ایستا unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), تهی *, mswi_ptr_offset, (__mswi))

ایستا تهی mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	ساختار sbi_scratch *scratch;
	ساختار aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	اگر (!scratch)
		بازگشت;

	mswi = mswi_get_hart_data_ptr(scratch);
	اگر (!mswi)
		بازگشت;

	/* Set ACLINT IPI */
	msip = (تهی *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

ایستا تهی mswi_ipi_clear(تهی)
{
	u32 *msip;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	اگر (!mswi)
		بازگشت;

	/* Clear ACLINT IPI */
	msip = (تهی *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

ایستا ساختار sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

صحیح aclint_mswi_cold_init(ساختار aclint_mswi_data *mswi)
{
	u32 i;
	صحیح rc;
	ساختار sbi_scratch *scratch;

	/* Sanity checks */
	اگر (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_شمار * اندازه(u32))) ||
	    (!mswi->hart_شمار || mswi->hart_شمار > ACLINT_MSWI_MAX_HARTS))
		بازگشت SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!mswi_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	برای (i = 0; i < mswi->hart_شمار; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		اگر (!scratch)
			ادامه;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ریشه_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	اگر (rc)
		بازگشت rc;

	sbi_ipi_add_device(&aclint_mswi);

	بازگشت 0;
}
