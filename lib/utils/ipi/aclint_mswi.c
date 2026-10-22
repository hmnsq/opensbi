/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/ipi/aclint_mswi.h>

ساكن unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), فراغ *, mswi_ptr_offset, (__mswi))

ساكن فراغ mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	بنية sbi_scratch *scratch;
	بنية aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	إذا (!scratch)
		أرجع;

	mswi = mswi_get_hart_data_ptr(scratch);
	إذا (!mswi)
		أرجع;

	/* Set ACLINT IPI */
	msip = (فراغ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

ساكن فراغ mswi_ipi_clear(فراغ)
{
	u32 *msip;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	إذا (!mswi)
		أرجع;

	/* Clear ACLINT IPI */
	msip = (فراغ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

ساكن بنية sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

صحيح aclint_mswi_cold_init(بنية aclint_mswi_data *mswi)
{
	u32 i;
	صحيح rc;
	بنية sbi_scratch *scratch;

	/* Sanity checks */
	إذا (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_كدي * حجم(u32))) ||
	    (!mswi->hart_كدي || mswi->hart_كدي > ACLINT_MSWI_MAX_HARTS))
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		إذا (!mswi_ptr_offset)
			أرجع SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	لكل (i = 0; i < mswi->hart_كدي; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_سيو_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_add_device(&aclint_mswi);

	أرجع 0;
}
