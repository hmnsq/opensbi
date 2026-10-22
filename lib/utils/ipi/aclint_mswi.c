/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#అన్తర్భావ <sbi/riscv_asm.h>
#అన్తర్భావ <sbi/riscv_atomic.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_ipi.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/ipi/aclint_mswi.h>

స్థిర గుర్తులేని దీర్ఘ mswi_ptr_స్థానభ్రంశం;

#నిర్వచించు mswi_get_hart_దత్తాంశం_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, mswi_ptr_స్థానభ్రంశం)

#నిర్వచించు mswi_సమితి_hart_దత్తాంశం_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), శూన్య *, mswi_ptr_స్థానభ్రంశం, (__mswi))

స్థిర శూన్య mswi_ipi_send(u32 hart_సూచీ)
{
	u32 *msip;
	నిర్మాణం sbi_scratch *scratch;
	నిర్మాణం aclint_mswi_దత్తాంశం *mswi;

	scratch = sbi_hartindex_గమ్యం_scratch(hart_సూచీ);
	ఒకవేళ (!scratch)
		తిరిగి;

	mswi = mswi_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mswi)
		తిరిగి;

	/* Set ACLINT IPI */
	msip = (శూన్య *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_గమ్యం_hartid(hart_సూచీ) -
			mswi->మొదటి_hartid]);
}

స్థిర శూన్య mswi_ipi_clear(శూన్య)
{
	u32 *msip;
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం aclint_mswi_దత్తాంశం *mswi;

	mswi = mswi_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mswi)
		తిరిగి;

	/* Clear ACLINT IPI */
	msip = (శూన్య *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->మొదటి_hartid]);
}

స్థిర నిర్మాణం sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

పూర్ణాంకం aclint_mswi_cold_init(నిర్మాణం aclint_mswi_దత్తాంశం *mswi)
{
	u32 i;
	పూర్ణాంకం rc;
	నిర్మాణం sbi_scratch *scratch;

	/* Sanity checks */
	ఒకవేళ (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->కొలత < (mswi->hart_లెక్క * పరిమాణం(u32))) ||
	    (!mswi->hart_లెక్క || mswi->hart_లెక్క > ACLINT_MSWI_MAX_HARTS))
		తిరిగి SBI_EINVAL;

	/* Allocate scratch space pointer */
	ఒకవేళ (!mswi_ptr_స్థానభ్రంశం) {
		mswi_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!mswi_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	కొరకు (i = 0; i < mswi->hart_లెక్క; i++) {
		scratch = sbi_hartid_గమ్యం_scratch(mswi->మొదటి_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ఒకవేళ (!scratch)
			కొనసాగు;
		mswi_సమితి_hart_దత్తాంశం_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_మూలం_కూడు_memrange(mswi->addr, mswi->కొలత, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_ipi_కూడు_device(&aclint_mswi);

	తిరిగి 0;
}
