/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_asm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_atomic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/ipi/aclint_mswi.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mswi_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mswi_get_hart_ਅੰਕੜੇ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, mswi_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mswi_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(__scratch, __mswi)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, mswi_ptr_ਵਿੱਥ, (__mswi))

ਸਥਿਰ ਖਾਲੀ mswi_ipi_send(u32 hart_ਸੂਚਕਾਂਕ)
{
	u32 *msip;
	ਬਣਤਰ sbi_scratch *scratch;
	ਬਣਤਰ aclint_mswi_ਅੰਕੜੇ *mswi;

	scratch = sbi_hartindex_ਮੰਜ਼ਿਲ_scratch(hart_ਸੂਚਕਾਂਕ);
	ਜੇ (!scratch)
		ਵਾਪਸ;

	mswi = mswi_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mswi)
		ਵਾਪਸ;

	/* Set ACLINT IPI */
	msip = (ਖਾਲੀ *)mswi->addr;
	writel_relaxed(1, &msip[sbi_hartindex_ਮੰਜ਼ਿਲ_hartid(hart_ਸੂਚਕਾਂਕ) -
			mswi->ਪਹਿਲਾ_hartid]);
}

ਸਥਿਰ ਖਾਲੀ mswi_ipi_clear(ਖਾਲੀ)
{
	u32 *msip;
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ aclint_mswi_ਅੰਕੜੇ *mswi;

	mswi = mswi_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mswi)
		ਵਾਪਸ;

	/* Clear ACLINT IPI */
	msip = (ਖਾਲੀ *)mswi->addr;
	writel_relaxed(0, &msip[current_hartid() - mswi->ਪਹਿਲਾ_hartid]);
}

ਸਥਿਰ ਬਣਤਰ sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.rating = 100,
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

ਪੂਰਨ aclint_mswi_cold_init(ਬਣਤਰ aclint_mswi_ਅੰਕੜੇ *mswi)
{
	u32 i;
	ਪੂਰਨ rc;
	ਬਣਤਰ sbi_scratch *scratch;

	/* Sanity checks */
	ਜੇ (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->ਮਾਪ < (mswi->hart_ਗਿਣਤੀ * ਆਕਾਰ(u32))) ||
	    (!mswi->hart_ਗਿਣਤੀ || mswi->hart_ਗਿਣਤੀ > ACLINT_MSWI_MAX_HARTS))
		ਵਾਪਸ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ਜੇ (!mswi_ptr_ਵਿੱਥ) {
		mswi_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!mswi_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/* Update MSWI pointer in scratch space */
	ਲਈ (i = 0; i < mswi->hart_ਗਿਣਤੀ; i++) {
		scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(mswi->ਪਹਿਲਾ_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ਜੇ (!scratch)
			ਜਾਰੀ;
		mswi_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(scratch, mswi);
	}

	/* Add MSWI regions to the root domain */
	rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mswi->addr, mswi->ਮਾਪ, ACLINT_MSWI_ALIGN,
					  SBI_DOMAIN_MEMREGION_MMIO |
					  SBI_DOMAIN_MEMREGION_M_READABLE |
					  SBI_DOMAIN_MEMREGION_M_WRITABLE);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_ipi_ਜੋੜੋ_device(&aclint_mswi);

	ਵਾਪਸ 0;
}
