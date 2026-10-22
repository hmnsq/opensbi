/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_asm.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_atomic.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_bitops.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_domain.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_scratch.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_timer.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/timer/aclint_mtimer.h>

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mtimer_ptr_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿಸು mtimer_get_hart_ದತ್ತಾಂಶ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, mtimer_ptr_ಸ್ಥಾನಾಂತರ)

#ವ್ಯಾಖ್ಯಾನಿಸು mtimer_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, mtimer_ptr_ಸ್ಥಾನಾಂತರ, (__mtimer))

#ಆದರೆ __riscv_xlen != 32
ಸ್ಥಿರ u64 mtimer_ಸಮಯ_rd64(ಅಸ್ಥಿರ u64 *addr)
{
	ಹಿಂತಿರುಗಿ readq_relaxed(addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_ಸಮಯ_wr64(bool timecmp, u64 ಮೌಲ್ಯ, ಅಸ್ಥಿರ u64 *addr)
{
	writeq_relaxed(ಮೌಲ್ಯ, addr);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ u64 mtimer_ಸಮಯ_rd32(ಅಸ್ಥಿರ u64 *addr)
{
	u32 lo, hi;

	ಮಾಡು {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ಆಗ (hi != readl_relaxed((u32 *)addr + 1));

	ಹಿಂತಿರುಗಿ ((u64)hi << 32) | (u64)lo;
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_ಸಮಯ_wr32(bool timecmp, u64 ಮೌಲ್ಯ, ಅಸ್ಥಿರ u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (ಶೂನ್ಯ *)(addr));
	writel_relaxed((u32)(ಮೌಲ್ಯ >> 32), (ಅಕ್ಷರ *)(addr) + 0x04);
	writel_relaxed((u32)ಮೌಲ್ಯ, (ಶೂನ್ಯ *)(addr));
}

ಸ್ಥಿರ u64 mtimer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt;

	mt = mtimer_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mt)
		ಹಿಂತಿರುಗಿ 0;

	/* Read MTIMER Time Value */
	ಹಿಂತಿರುಗಿ mt->ಸಮಯ_rd((ಶೂನ್ಯ *)mt->mtime_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_event_stop(ಶೂನ್ಯ)
{
	u32 ಗುರಿ_hart = current_hartid();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt;
	u64 *ಸಮಯ_cmp;

	mt = mtimer_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mt)
		ಹಿಂತಿರುಗಿ;

	/* Clear MTIMER Time Compare */
	ಸಮಯ_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->ಸಮಯ_wr(true, -1ULL, &ಸಮಯ_cmp[ಗುರಿ_hart - mt->ಮೊದಲ_hartid]);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_event_ಆರಂಭ(u64 ಮುಂದಿನ_event)
{
	u32 ಗುರಿ_hart = current_hartid();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt;
	u64 *ಸಮಯ_cmp;

	mt = mtimer_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mt)
		ಹಿಂತಿರುಗಿ;

	/* Program MTIMER Time Compare */
	ಸಮಯ_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->ಸಮಯ_wr(true, ಮುಂದಿನ_event,
		    &ಸಮಯ_cmp[ಗುರಿ_hart - mt->ಮೊದಲ_hartid]);
}

ಸ್ಥಿರ ರಚನೆ sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_ಮೌಲ್ಯ = mtimer_ಮೌಲ್ಯ,
	.timer_event_ಆರಂಭ = mtimer_event_ಆರಂಭ,
	.timer_event_stop = mtimer_event_stop
};

ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *aclint_get_mtimer_ದತ್ತಾಂಶ(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ mtimer_get_hart_ದತ್ತಾಂಶ_ptr(sbi_scratch_thishart_ptr());
}

ಶೂನ್ಯ aclint_mtimer_update(ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt,
			  ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_ಸಮಯ_val, *ref_ಸಮಯ_val;

	ಆದರೆ (!mt || !ref || !mt->ಸಮಯ_rd || !mt->ಸಮಯ_wr || !ref->ಸಮಯ_rd)
		ಹಿಂತಿರುಗಿ;

	mt_ಸಮಯ_val = (ಶೂನ್ಯ *)mt->mtime_addr;
	ref_ಸಮಯ_val = (ಶೂನ್ಯ *)ref->mtime_addr;
	ಆದರೆ (!atomic_raw_xchg_ulong(&mt->ಸಮಯ_delta_computed, 1)) {
		v1 = mt->ಸಮಯ_rd(mt_ಸಮಯ_val);
		mv = ref->ಸಮಯ_rd(ref_ಸಮಯ_val);
		v2 = mt->ಸಮಯ_rd(mt_ಸಮಯ_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->ಸಮಯ_wr(false, mt->ಸಮಯ_rd(mt_ಸಮಯ_val) + delta,
			    mt_ಸಮಯ_val);
	}
}

ಶೂನ್ಯ aclint_mtimer_sync(ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt)
{
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ಆದರೆ (mt->has_shared_mtime || !mt->ಸಮಯ_delta_reference)
		ಹಿಂತಿರುಗಿ;

	reference = mt->ಸಮಯ_delta_reference;
	aclint_mtimer_update(mt, reference);
}

ಶೂನ್ಯ aclint_mtimer_ಗಣ_reference(ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt,
				 ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *ref)
{
	ಆದರೆ (!mt || !ref || mt == ref)
		ಹಿಂತಿರುಗಿ;

	mt->ಸಮಯ_delta_reference = ref;
	mt->ಸಮಯ_delta_computed = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aclint_mtimer_warm_init(ಶೂನ್ಯ)
{
	u64 *mt_ಸಮಯ_cmp;
	u32 ಗುರಿ_hart = current_hartid();
	ರಚನೆ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt;

	mt = mtimer_get_hart_ದತ್ತಾಂಶ_ptr(scratch);
	ಆದರೆ (!mt)
		ಹಿಂತಿರುಗಿ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_ಸಮಯ_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->ಸಮಯ_wr(true, -1ULL,
		    &mt_ಸಮಯ_cmp[ಗುರಿ_hart - mt->ಮೊದಲ_hartid]);

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ aclint_mtimer_cold_init(ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *mt,
			    ರಚನೆ aclint_mtimer_ದತ್ತಾಂಶ *reference)
{
	u32 i;
	ಪೂರ್ಣಾಂಕ rc;
	ರಚನೆ sbi_scratch *scratch;

	/* Sanity checks */
	ಆದರೆ (!mt ||
	    (mt->hart_ಎಣಿಕೆ && !mt->mtimecmp_ಗಾತ್ರ) ||
	    (mt->mtime_ಗಾತ್ರ && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_ಗಾತ್ರ && (mt->mtime_ಗಾತ್ರ & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_ಗಾತ್ರ & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ಎಣಿಕೆ > ACLINT_MTIMER_MAX_HARTS))
		ಹಿಂತಿರುಗಿ SBI_EINVAL;
	ಆದರೆ (reference && mt->mtime_freq != reference->mtime_freq)
		ಹಿಂತಿರುಗಿ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಆದರೆ (!mtimer_ptr_ಸ್ಥಾನಾಂತರ) {
		mtimer_ptr_ಸ್ಥಾನಾಂತರ = sbi_scratch_alloc_type_ಸ್ಥಾನಾಂತರ(ಶೂನ್ಯ *);
		ಆದರೆ (!mtimer_ptr_ಸ್ಥಾನಾಂತರ)
			ಹಿಂತಿರುಗಿ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_ಗಣ_reference(mt, reference);
	mt->ಸಮಯ_rd = mtimer_ಸಮಯ_rd32;
	mt->ಸಮಯ_wr = mtimer_ಸಮಯ_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ಆದರೆ __riscv_xlen != 32
	ಆದರೆ (mt->has_64bit_mmio) {
		mt->ಸಮಯ_rd = mtimer_ಸಮಯ_rd64;
		mt->ಸಮಯ_wr = mtimer_ಸಮಯ_wr64;
	}
#ಆದರೆ_ಅಂತ್ಯ

	/* Update MTIMER pointer in scratch space */
	ಗಾಗಿ (i = 0; i < mt->hart_ಎಣಿಕೆ; i++) {
		scratch = sbi_hartid_ಗಮ್ಯ_scratch(mt->ಮೊದಲ_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ಆದರೆ (!scratch)
			ಮುಂದುವರಿ;
		mtimer_ಗಣ_hart_ದತ್ತಾಂಶ_ptr(scratch, mt);
	}

	ಆದರೆ (!mt->mtime_ಗಾತ್ರ) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_ಮೌಲ್ಯ = ಖಾಲಿ;
	}

	/* Add MTIMER regions to the root domain */
	ಆದರೆ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_ಗಾತ್ರ)) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mt->mtimecmp_addr,
					mt->mtime_ಗಾತ್ರ + mt->mtimecmp_ಗಾತ್ರ,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_ಗಾತ್ರ)) {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mt->mtime_addr,
					mt->mtime_ಗಾತ್ರ + mt->mtimecmp_ಗಾತ್ರ,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	} ಇಲ್ಲದಿದ್ದರೆ {
		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mt->mtime_addr,
						mt->mtime_ಗಾತ್ರ, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;

		rc = sbi_domain_ಬೇರು_ಕೂಡಿಸು_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_ಗಾತ್ರ, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಆದರೆ (rc)
			ಹಿಂತಿರುಗಿ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_ಗಣ_device(&mtimer);

	ಹಿಂತಿರುಗಿ 0;
}
