/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_ipi.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/ipi/aclint_mswi.h>

स्थिर अचिन्हित दीर्घ mswi_ptr_offset;

#define mswi_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, mswi_ptr_offset)

#define mswi_set_hart_data_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), शून्य *, mswi_ptr_offset, (__mswi))

स्थिर शून्य mswi_ipi_send(u32 hart_index)
{
	u32 *msip;
	संरचना sbi_scratch *scratch;
	संरचना aclint_mswi_data *mswi;

	scratch = sbi_hartindex_to_scratch(hart_index);
	यदि (!scratch)
		फर्काऊ;

	mswi = mswi_get_hart_data_ptr(scratch);
	यदि (!mswi)
		फर्काऊ;

	/* Set ACLINT IPI */
	msip = (शून्य *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_to_hartid(hart_index) -
			mswi->first_hartid]);
}

स्थिर शून्य mswi_ipi_clear(शून्य)
{
	u32 *msip;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mswi_data *mswi;

	mswi = mswi_get_hart_data_ptr(scratch);
	यदि (!mswi)
		फर्काऊ;

	/* Clear ACLINT IPI */
	msip = (शून्य *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->first_hartid]);
}

स्थिर संरचना sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

पूर्णाङ्क aclint_mswi_cold_init(संरचना aclint_mswi_data *mswi)
{
	u32 i;
	पूर्णाङ्क rc;
	संरचना sbi_scratch *scratch;

	/* Sanity checks */
	यदि (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < (mswi->hart_गन्ती * परिमाण(u32))) ||
	    (!mswi->hart_गन्ती || mswi->hart_गन्ती > ACLINT_MSWI_MAX_HARTS))
		फर्काऊ SBI_EINVAL;

	/* Allocate scratch space pointer */
	यदि (!mswi_ptr_offset) {
		mswi_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		यदि (!mswi_ptr_offset)
			फर्काऊ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	लागि (i = 0; i < mswi->hart_गन्ती; i++) {
		scratch = sbi_hartid_to_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		यदि (!scratch)
			जारी;
		mswi_set_hart_data_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_जरा_add_memrange(mswi->addr, mswi->size, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	यदि (rc)
		फर्काऊ rc;

	sbi_ipi_add_device(&aclint_mswi);

	फर्काऊ 0;
}
