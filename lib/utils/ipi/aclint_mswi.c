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

स्थिर अचिह्नित दीर्घ mswi_ptr_विस्थापनम्;

#परिभाषय mswi_get_hart_दत्त_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, mswi_ptr_विस्थापनम्)

#परिभाषय mswi_समुच्चयः_hart_दत्त_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), शून्य *, mswi_ptr_विस्थापनम्, (__mswi))

स्थिर शून्य mswi_ipi_send(u32 hart_सूचकांक)
{
	u32 *msip;
	संरचना sbi_scratch *scratch;
	संरचना aclint_mswi_दत्त *mswi;

	scratch = sbi_hartindex_गन्तव्यम्_scratch(hart_सूचकांक);
	यदि (!scratch)
		प्रत्यागम;

	mswi = mswi_get_hart_दत्त_ptr(scratch);
	यदि (!mswi)
		प्रत्यागम;

	/* Set ACLINT IPI */
	msip = (शून्य *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_गन्तव्यम्_hartid(hart_सूचकांक) -
			mswi->first_hartid]);
}

स्थिर शून्य mswi_ipi_clear(शून्य)
{
	u32 *msip;
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mswi_दत्त *mswi;

	mswi = mswi_get_hart_दत्त_ptr(scratch);
	यदि (!mswi)
		प्रत्यागम;

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

पूर्णाङ्क aclint_mswi_cold_init(संरचना aclint_mswi_दत्त *mswi)
{
	u32 i;
	पूर्णाङ्क rc;
	संरचना sbi_scratch *scratch;

	/* Sanity checks */
	यदि (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->आकार < (mswi->hart_गणनम् * परिमाण(u32))) ||
	    (!mswi->hart_गणनम् || mswi->hart_गणनम् > ACLINT_MSWI_MAX_HARTS))
		प्रत्यागम SBI_EINVAL;

	/* Allocate scratch space pointer */
	यदि (!mswi_ptr_विस्थापनम्) {
		mswi_ptr_विस्थापनम् = sbi_scratch_alloc_type_विस्थापनम्(शून्य *);
		यदि (!mswi_ptr_विस्थापनम्)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	प्रति (i = 0; i < mswi->hart_गणनम्; i++) {
		scratch = sbi_hartid_गन्तव्यम्_scratch(mswi->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		यदि (!scratch)
			अनुवर्त;
		mswi_समुच्चयः_hart_दत्त_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_मूलम्_सङ्कलय_memrange(mswi->addr, mswi->आकार, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	यदि (rc)
		प्रत्यागम rc;

	sbi_ipi_सङ्कलय_device(&aclint_mswi);

	प्रत्यागम 0;
}
