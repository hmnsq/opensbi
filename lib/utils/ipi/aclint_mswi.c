/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_ipi.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/ipi/aclint_mswi.h>

സ്ഥിതം unsigned long mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, mswi_ptr_offset, (__mswi))

സ്ഥിതം ശൂന്യം mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	ഘടന sbi_scratch *scratch;
	ഘടന aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	എങ്കിൽ (!scratch)
		തിരികെ;

	mswi = mswi_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mswi)
		തിരികെ;

	/* Set ACLINT IPI */
	msip = (ശൂന്യം *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

സ്ഥിതം ശൂന്യം mswi_ipi_clear(ശൂന്യം)
{
	u32 *msip;
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mswi)
		തിരികെ;

	/* Clear ACLINT IPI */
	msip = (ശൂന്യം *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

സ്ഥിതം ഘടന sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

പൂർണ്ണസംഖ്യ aclint_mswi_cold_init(ഘടന aclint_mswi_data *mswi)
{
	u32 i;
	പൂർണ്ണസംഖ്യ rc;
	ഘടന sbi_scratch *scratch;

	/* Sanity checks */
	എങ്കിൽ (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_എണ്ണം * വലുപ്പം(u32))) ||
	    (!mswi->hart_എണ്ണം || mswi->hart_എണ്ണം > ACLINT_MSWI_MAX_HARTS))
		തിരികെ SBI_EINVAL;

	/* Allocate scratch space pointer */
	എങ്കിൽ (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!mswi_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	വേണ്ടി (i = 0; i < mswi->hart_എണ്ണം; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		എങ്കിൽ (!scratch)
			തുടരുക;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_വേര്_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sbi_ipi_add_device(&aclint_mswi);

	തിരികെ 0;
}
