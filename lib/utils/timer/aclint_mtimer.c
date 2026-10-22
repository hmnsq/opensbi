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
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_bitops.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_scratch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/aclint_mtimer.h>

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mtimer_ptr_ਵਿੱਥ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mtimer_get_hart_ਅੰਕੜੇ_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ਖਾਲੀ *, mtimer_ptr_ਵਿੱਥ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mtimer_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), ਖਾਲੀ *, mtimer_ptr_ਵਿੱਥ, (__mtimer))

#ਜੇ __riscv_xlen != 32
ਸਥਿਰ u64 mtimer_ਸਮਾਂ_rd64(ਅਸਥਿਰ u64 *addr)
{
	ਵਾਪਸ readq_relaxed(addr);
}

ਸਥਿਰ ਖਾਲੀ mtimer_ਸਮਾਂ_wr64(bool timecmp, u64 ਮੁੱਲ, ਅਸਥਿਰ u64 *addr)
{
	writeq_relaxed(ਮੁੱਲ, addr);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ u64 mtimer_ਸਮਾਂ_rd32(ਅਸਥਿਰ u64 *addr)
{
	u32 lo, hi;

	ਕਰੋ {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ਜਦੋਂ (hi != readl_relaxed((u32 *)addr + 1));

	ਵਾਪਸ ((u64)hi << 32) | (u64)lo;
}

ਸਥਿਰ ਖਾਲੀ mtimer_ਸਮਾਂ_wr32(bool timecmp, u64 ਮੁੱਲ, ਅਸਥਿਰ u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (ਖਾਲੀ *)(addr));
	writel_relaxed((u32)(ਮੁੱਲ >> 32), (ਅੱਖਰ *)(addr) + 0x04);
	writel_relaxed((u32)ਮੁੱਲ, (ਖਾਲੀ *)(addr));
}

ਸਥਿਰ u64 mtimer_ਮੁੱਲ(ਖਾਲੀ)
{
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt;

	mt = mtimer_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mt)
		ਵਾਪਸ 0;

	/* Read MTIMER Time Value */
	ਵਾਪਸ mt->ਸਮਾਂ_rd((ਖਾਲੀ *)mt->mtime_addr);
}

ਸਥਿਰ ਖਾਲੀ mtimer_event_stop(ਖਾਲੀ)
{
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt;
	u64 *ਸਮਾਂ_cmp;

	mt = mtimer_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mt)
		ਵਾਪਸ;

	/* Clear MTIMER Time Compare */
	ਸਮਾਂ_cmp = (ਖਾਲੀ *)mt->mtimecmp_addr;
	mt->ਸਮਾਂ_wr(true, -1ULL, &ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart - mt->ਪਹਿਲਾ_hartid]);
}

ਸਥਿਰ ਖਾਲੀ mtimer_event_ਸ਼ੁਰੂ(u64 ਅਗਲਾ_event)
{
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt;
	u64 *ਸਮਾਂ_cmp;

	mt = mtimer_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mt)
		ਵਾਪਸ;

	/* Program MTIMER Time Compare */
	ਸਮਾਂ_cmp = (ਖਾਲੀ *)mt->mtimecmp_addr;
	mt->ਸਮਾਂ_wr(true, ਅਗਲਾ_event,
		    &ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart - mt->ਪਹਿਲਾ_hartid]);
}

ਸਥਿਰ ਬਣਤਰ sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_ਮੁੱਲ = mtimer_ਮੁੱਲ,
	.timer_event_ਸ਼ੁਰੂ = mtimer_event_ਸ਼ੁਰੂ,
	.timer_event_stop = mtimer_event_stop
};

ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *aclint_get_mtimer_ਅੰਕੜੇ(ਖਾਲੀ)
{
	ਵਾਪਸ mtimer_get_hart_ਅੰਕੜੇ_ptr(sbi_scratch_thishart_ptr());
}

ਖਾਲੀ aclint_mtimer_update(ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt,
			  ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_ਸਮਾਂ_val, *ref_ਸਮਾਂ_val;

	ਜੇ (!mt || !ref || !mt->ਸਮਾਂ_rd || !mt->ਸਮਾਂ_wr || !ref->ਸਮਾਂ_rd)
		ਵਾਪਸ;

	mt_ਸਮਾਂ_val = (ਖਾਲੀ *)mt->mtime_addr;
	ref_ਸਮਾਂ_val = (ਖਾਲੀ *)ref->mtime_addr;
	ਜੇ (!atomic_raw_xchg_ulong(&mt->ਸਮਾਂ_delta_computed, 1)) {
		v1 = mt->ਸਮਾਂ_rd(mt_ਸਮਾਂ_val);
		mv = ref->ਸਮਾਂ_rd(ref_ਸਮਾਂ_val);
		v2 = mt->ਸਮਾਂ_rd(mt_ਸਮਾਂ_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->ਸਮਾਂ_wr(false, mt->ਸਮਾਂ_rd(mt_ਸਮਾਂ_val) + delta,
			    mt_ਸਮਾਂ_val);
	}
}

ਖਾਲੀ aclint_mtimer_sync(ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt)
{
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ਜੇ (mt->has_shared_mtime || !mt->ਸਮਾਂ_delta_reference)
		ਵਾਪਸ;

	reference = mt->ਸਮਾਂ_delta_reference;
	aclint_mtimer_update(mt, reference);
}

ਖਾਲੀ aclint_mtimer_ਸੈੱਟ_reference(ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt,
				 ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *ref)
{
	ਜੇ (!mt || !ref || mt == ref)
		ਵਾਪਸ;

	mt->ਸਮਾਂ_delta_reference = ref;
	mt->ਸਮਾਂ_delta_computed = 0;
}

ਸਥਿਰ ਪੂਰਨ aclint_mtimer_warm_init(ਖਾਲੀ)
{
	u64 *mt_ਸਮਾਂ_cmp;
	u32 ਨਿਸ਼ਾਨਾ_hart = current_hartid();
	ਬਣਤਰ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt;

	mt = mtimer_get_hart_ਅੰਕੜੇ_ptr(scratch);
	ਜੇ (!mt)
		ਵਾਪਸ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_ਸਮਾਂ_cmp = (ਖਾਲੀ *)mt->mtimecmp_addr;
	mt->ਸਮਾਂ_wr(true, -1ULL,
		    &mt_ਸਮਾਂ_cmp[ਨਿਸ਼ਾਨਾ_hart - mt->ਪਹਿਲਾ_hartid]);

	ਵਾਪਸ 0;
}

ਪੂਰਨ aclint_mtimer_cold_init(ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *mt,
			    ਬਣਤਰ aclint_mtimer_ਅੰਕੜੇ *reference)
{
	u32 i;
	ਪੂਰਨ rc;
	ਬਣਤਰ sbi_scratch *scratch;

	/* Sanity checks */
	ਜੇ (!mt ||
	    (mt->hart_ਗਿਣਤੀ && !mt->mtimecmp_ਮਾਪ) ||
	    (mt->mtime_ਮਾਪ && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_ਮਾਪ && (mt->mtime_ਮਾਪ & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_ਮਾਪ & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ਗਿਣਤੀ > ACLINT_MTIMER_MAX_HARTS))
		ਵਾਪਸ SBI_EINVAL;
	ਜੇ (reference && mt->mtime_freq != reference->mtime_freq)
		ਵਾਪਸ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ਜੇ (!mtimer_ptr_ਵਿੱਥ) {
		mtimer_ptr_ਵਿੱਥ = sbi_scratch_alloc_type_ਵਿੱਥ(ਖਾਲੀ *);
		ਜੇ (!mtimer_ptr_ਵਿੱਥ)
			ਵਾਪਸ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_ਸੈੱਟ_reference(mt, reference);
	mt->ਸਮਾਂ_rd = mtimer_ਸਮਾਂ_rd32;
	mt->ਸਮਾਂ_wr = mtimer_ਸਮਾਂ_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ਜੇ __riscv_xlen != 32
	ਜੇ (mt->has_64bit_mmio) {
		mt->ਸਮਾਂ_rd = mtimer_ਸਮਾਂ_rd64;
		mt->ਸਮਾਂ_wr = mtimer_ਸਮਾਂ_wr64;
	}
#ਜੇ_ਅੰਤ

	/* Update MTIMER pointer in scratch space */
	ਲਈ (i = 0; i < mt->hart_ਗਿਣਤੀ; i++) {
		scratch = sbi_hartid_ਮੰਜ਼ਿਲ_scratch(mt->ਪਹਿਲਾ_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ਜੇ (!scratch)
			ਜਾਰੀ;
		mtimer_ਸੈੱਟ_hart_ਅੰਕੜੇ_ptr(scratch, mt);
	}

	ਜੇ (!mt->mtime_ਮਾਪ) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_ਮੁੱਲ = ਨਿਰਾ;
	}

	/* Add MTIMER regions to the root domain */
	ਜੇ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_ਮਾਪ)) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mt->mtimecmp_addr,
					mt->mtime_ਮਾਪ + mt->mtimecmp_ਮਾਪ,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;
	} ਨਹੀਂਤਾਂ ਜੇ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_ਮਾਪ)) {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mt->mtime_addr,
					mt->mtime_ਮਾਪ + mt->mtimecmp_ਮਾਪ,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;
	} ਨਹੀਂਤਾਂ {
		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mt->mtime_addr,
						mt->mtime_ਮਾਪ, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;

		rc = sbi_domain_ਜੜ੍ਹ_ਜੋੜੋ_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_ਮਾਪ, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ਜੇ (rc)
			ਵਾਪਸ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_ਸੈੱਟ_device(&mtimer);

	ਵਾਪਸ 0;
}
