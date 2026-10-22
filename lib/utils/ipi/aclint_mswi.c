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

ساکن غیرعلامتی طویل mswi_ptr_انحراف;

#تعریف_کرو mswi_get_hart_ڈیٹا_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, mswi_ptr_انحراف)

#تعریف_کرو mswi_سیٹ_hart_ڈیٹا_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), خالی *, mswi_ptr_انحراف, (__mswi))

ساکن خالی mswi_ipi_send(u32 hart_اشاریہ)
{
	u32 *msip;
	ساخت sbi_scratch *scratch;
	ساخت aclint_mswi_ڈیٹا *mswi;

	scratch = sbi_hartindex_مقصد_scratch(hart_اشاریہ);
	اگر (!scratch)
		واپس;

	mswi = mswi_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mswi)
		واپس;

	/* Set ACLINT IPI */
	msip = (خالی *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_مقصد_hartid(hart_اشاریہ) -
			mswi->پہلا_hartid]);
}

ساکن خالی mswi_ipi_clear(خالی)
{
	u32 *msip;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت aclint_mswi_ڈیٹا *mswi;

	mswi = mswi_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mswi)
		واپس;

	/* Clear ACLINT IPI */
	msip = (خالی *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->پہلا_hartid]);
}

ساکن ساخت sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

صحیح aclint_mswi_cold_init(ساخت aclint_mswi_ڈیٹا *mswi)
{
	u32 i;
	صحیح rc;
	ساخت sbi_scratch *scratch;

	/* Sanity checks */
	اگر (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->سائز < (mswi->hart_گنتی * حجم(u32))) ||
	    (!mswi->hart_گنتی || mswi->hart_گنتی > ACLINT_MSWI_MAX_HARTS))
		واپس SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!mswi_ptr_انحراف) {
		mswi_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!mswi_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	برائے (i = 0; i < mswi->hart_گنتی; i++) {
		scratch = sbi_hartid_مقصد_scratch(mswi->پہلا_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		اگر (!scratch)
			جاری;
		mswi_سیٹ_hart_ڈیٹا_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_جڑ_جوڑو_memrange(mswi->addr, mswi->سائز, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	اگر (rc)
		واپس rc;

	sbi_ipi_جوڑو_device(&aclint_mswi);

	واپس 0;
}
