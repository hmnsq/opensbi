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

ساكن بیعلامت دراز mswi_ptr_جابه_جایی;

#تعریف_کن mswi_get_hart_داده_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mswi_ptr_جابه_جایی)

#تعریف_کن mswi_مجموعه_hart_داده_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), فراغ *, mswi_ptr_جابه_جایی, (__mswi))

ساكن فراغ mswi_ipi_send(u32 hart_شاخص)
{
	u32 *msip;
	بنية sbi_scratch *scratch;
	بنية aclint_mswi_داده *mswi;

	scratch = sbi_hartindex_مقصد_scratch(hart_شاخص);
	إذا (!scratch)
		أرجع;

	mswi = mswi_get_hart_داده_ptr(scratch);
	إذا (!mswi)
		أرجع;

	/* Set ACLINT IPI */
	msip = (فراغ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_مقصد_hartid(hart_شاخص) -
			mswi->اولی_hartid]);
}

ساكن فراغ mswi_ipi_clear(فراغ)
{
	u32 *msip;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mswi_داده *mswi;

	mswi = mswi_get_hart_داده_ptr(scratch);
	إذا (!mswi)
		أرجع;

	/* Clear ACLINT IPI */
	msip = (فراغ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->اولی_hartid]);
}

ساكن بنية sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

صحيح aclint_mswi_cold_init(بنية aclint_mswi_داده *mswi)
{
	u32 i;
	صحيح rc;
	بنية sbi_scratch *scratch;

	/* Sanity checks */
	إذا (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->اندازه < (mswi->hart_شمار * حجم(u32))) ||
	    (!mswi->hart_شمار || mswi->hart_شمار > ACLINT_MSWI_MAX_HARTS))
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mswi_ptr_جابه_جایی) {
		mswi_ptr_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(فراغ *);
		إذا (!mswi_ptr_جابه_جایی)
			أرجع SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	لكل (i = 0; i < mswi->hart_شمار; i++) {
		scratch = sbi_hartid_مقصد_scratch(mswi->اولی_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mswi_مجموعه_hart_داده_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ریشه_جمع_کن_memrange(mswi->addr, mswi->اندازه, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	sbi_ipi_جمع_کن_device(&aclint_mswi);

	أرجع 0;
}
