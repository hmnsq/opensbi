/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/ipi/aclint_mswi.h>

अटल अचिन्हित दीर्घ mswi_ptr_offset;

#घोषणा mswi_get_hart_तथ्य_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, mswi_ptr_offset)

#घोषणा mswi_set_hart_तथ्य_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), व्योम *, mswi_ptr_offset, (__mswi))

अटल व्योम mswi_ipi_send(u32 hart_सूचकांक)
{
	u32 *msip;
	काष्ठा sbi_scratch *scratch;
	काष्ठा aclint_mswi_तथ्य *mswi;

	scratch = sbi_hartindex_to_scratch(hart_सूचकांक);
	अगर (!scratch)
		वापस;

	mswi = mswi_get_hart_तथ्य_ptr(scratch);
	अगर (!mswi)
		वापस;

	/* Set ACLINT IPI */
	msip = (व्योम *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_सूचकांक) -
			mswi->first_hartid]);
}

अटल व्योम mswi_ipi_clear(व्योम)
{
	u32 *msip;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा aclint_mswi_तथ्य *mswi;

	mswi = mswi_get_hart_तथ्य_ptr(scratch);
	अगर (!mswi)
		वापस;

	/* Clear ACLINT IPI */
	msip = (व्योम *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

अटल काष्ठा sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

पूर्णांक aclint_mswi_cold_init(काष्ठा aclint_mswi_तथ्य *mswi)
{
	u32 i;
	पूर्णांक rc;
	काष्ठा sbi_scratch *scratch;

	/* Sanity checks */
	अगर (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->आकार < (mswi->hart_गिनती * माप(u32))) ||
	    (!mswi->hart_गिनती || mswi->hart_गिनती > ACLINT_MSWI_MAX_HARTS))
		वापस SBI_EINVAL;

	/* Allocate scratch space pointer */
	अगर (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!mswi_ptr_offset)
			वापस SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	क्रम (i = 0; i < mswi->hart_गिनती; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		अगर (!scratch)
			जारी;
		mswi_set_hart_तथ्य_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_मूल_add_memrange(mswi->addr, mswi->आकार, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	अगर (rc)
		वापस rc;

	sbi_ipi_add_device(&aclint_mswi);

	वापस 0;
}
