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
#అన్తర్భావ <sbi/sbi_bitops.h>
#అన్తర్భావ <sbi/sbi_domain.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_scratch.h>
#అన్తర్భావ <sbi/sbi_timer.h>
#అన్తర్భావ <sbi_utils/timer/aclint_mtimer.h>

స్థిర గుర్తులేని దీర్ఘ mtimer_ptr_స్థానభ్రంశం;

#నిర్వచించు mtimer_get_hart_దత్తాంశం_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), శూన్య *, mtimer_ptr_స్థానభ్రంశం)

#నిర్వచించు mtimer_సమితి_hart_దత్తాంశం_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), శూన్య *, mtimer_ptr_స్థానభ్రంశం, (__mtimer))

#ఒకవేళ __riscv_xlen != 32
స్థిర u64 mtimer_సమయం_rd64(అస్థిర u64 *addr)
{
	తిరిగి readq_relaxed(addr);
}

స్థిర శూన్య mtimer_సమయం_wr64(bool timecmp, u64 విలువ, అస్థిర u64 *addr)
{
	writeq_relaxed(విలువ, addr);
}
#అయితే_ముగింపు

స్థిర u64 mtimer_సమయం_rd32(అస్థిర u64 *addr)
{
	u32 lo, hi;

	చేయి {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} అయితే (hi != readl_relaxed((u32 *)addr + 1));

	తిరిగి ((u64)hi << 32) | (u64)lo;
}

స్థిర శూన్య mtimer_సమయం_wr32(bool timecmp, u64 విలువ, అస్థిర u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (శూన్య *)(addr));
	writel_relaxed((u32)(విలువ >> 32), (అక్షరం *)(addr) + 0x04);
	writel_relaxed((u32)విలువ, (శూన్య *)(addr));
}

స్థిర u64 mtimer_విలువ(శూన్య)
{
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt;

	mt = mtimer_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mt)
		తిరిగి 0;

	/* Read MTIMER Time Value */
	తిరిగి mt->సమయం_rd((శూన్య *)mt->mtime_addr);
}

స్థిర శూన్య mtimer_event_stop(శూన్య)
{
	u32 లక్ష్యం_hart = current_hartid();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt;
	u64 *సమయం_cmp;

	mt = mtimer_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mt)
		తిరిగి;

	/* Clear MTIMER Time Compare */
	సమయం_cmp = (శూన్య *)mt->mtimecmp_addr;
	mt->సమయం_wr(true, -1ULL, &సమయం_cmp[లక్ష్యం_hart - mt->మొదటి_hartid]);
}

స్థిర శూన్య mtimer_event_ప్రారంభం(u64 తదుపరి_event)
{
	u32 లక్ష్యం_hart = current_hartid();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt;
	u64 *సమయం_cmp;

	mt = mtimer_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mt)
		తిరిగి;

	/* Program MTIMER Time Compare */
	సమయం_cmp = (శూన్య *)mt->mtimecmp_addr;
	mt->సమయం_wr(true, తదుపరి_event,
		    &సమయం_cmp[లక్ష్యం_hart - mt->మొదటి_hartid]);
}

స్థిర నిర్మాణం sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_విలువ = mtimer_విలువ,
	.timer_event_ప్రారంభం = mtimer_event_ప్రారంభం,
	.timer_event_stop = mtimer_event_stop
};

నిర్మాణం aclint_mtimer_దత్తాంశం *aclint_get_mtimer_దత్తాంశం(శూన్య)
{
	తిరిగి mtimer_get_hart_దత్తాంశం_ptr(sbi_scratch_thishart_ptr());
}

శూన్య aclint_mtimer_update(నిర్మాణం aclint_mtimer_దత్తాంశం *mt,
			  నిర్మాణం aclint_mtimer_దత్తాంశం *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_సమయం_val, *ref_సమయం_val;

	ఒకవేళ (!mt || !ref || !mt->సమయం_rd || !mt->సమయం_wr || !ref->సమయం_rd)
		తిరిగి;

	mt_సమయం_val = (శూన్య *)mt->mtime_addr;
	ref_సమయం_val = (శూన్య *)ref->mtime_addr;
	ఒకవేళ (!atomic_raw_xchg_ulong(&mt->సమయం_delta_computed, 1)) {
		v1 = mt->సమయం_rd(mt_సమయం_val);
		mv = ref->సమయం_rd(ref_సమయం_val);
		v2 = mt->సమయం_rd(mt_సమయం_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->సమయం_wr(false, mt->సమయం_rd(mt_సమయం_val) + delta,
			    mt_సమయం_val);
	}
}

శూన్య aclint_mtimer_sync(నిర్మాణం aclint_mtimer_దత్తాంశం *mt)
{
	నిర్మాణం aclint_mtimer_దత్తాంశం *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ఒకవేళ (mt->has_shared_mtime || !mt->సమయం_delta_reference)
		తిరిగి;

	reference = mt->సమయం_delta_reference;
	aclint_mtimer_update(mt, reference);
}

శూన్య aclint_mtimer_సమితి_reference(నిర్మాణం aclint_mtimer_దత్తాంశం *mt,
				 నిర్మాణం aclint_mtimer_దత్తాంశం *ref)
{
	ఒకవేళ (!mt || !ref || mt == ref)
		తిరిగి;

	mt->సమయం_delta_reference = ref;
	mt->సమయం_delta_computed = 0;
}

స్థిర పూర్ణాంకం aclint_mtimer_warm_init(శూన్య)
{
	u64 *mt_సమయం_cmp;
	u32 లక్ష్యం_hart = current_hartid();
	నిర్మాణం sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	నిర్మాణం aclint_mtimer_దత్తాంశం *mt;

	mt = mtimer_get_hart_దత్తాంశం_ptr(scratch);
	ఒకవేళ (!mt)
		తిరిగి SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_సమయం_cmp = (శూన్య *)mt->mtimecmp_addr;
	mt->సమయం_wr(true, -1ULL,
		    &mt_సమయం_cmp[లక్ష్యం_hart - mt->మొదటి_hartid]);

	తిరిగి 0;
}

పూర్ణాంకం aclint_mtimer_cold_init(నిర్మాణం aclint_mtimer_దత్తాంశం *mt,
			    నిర్మాణం aclint_mtimer_దత్తాంశం *reference)
{
	u32 i;
	పూర్ణాంకం rc;
	నిర్మాణం sbi_scratch *scratch;

	/* Sanity checks */
	ఒకవేళ (!mt ||
	    (mt->hart_లెక్క && !mt->mtimecmp_కొలత) ||
	    (mt->mtime_కొలత && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_కొలత && (mt->mtime_కొలత & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_కొలత & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_లెక్క > ACLINT_MTIMER_MAX_HARTS))
		తిరిగి SBI_EINVAL;
	ఒకవేళ (reference && mt->mtime_freq != reference->mtime_freq)
		తిరిగి SBI_EINVAL;

	/* Allocate scratch space pointer */
	ఒకవేళ (!mtimer_ptr_స్థానభ్రంశం) {
		mtimer_ptr_స్థానభ్రంశం = sbi_scratch_alloc_type_స్థానభ్రంశం(శూన్య *);
		ఒకవేళ (!mtimer_ptr_స్థానభ్రంశం)
			తిరిగి SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_సమితి_reference(mt, reference);
	mt->సమయం_rd = mtimer_సమయం_rd32;
	mt->సమయం_wr = mtimer_సమయం_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ఒకవేళ __riscv_xlen != 32
	ఒకవేళ (mt->has_64bit_mmio) {
		mt->సమయం_rd = mtimer_సమయం_rd64;
		mt->సమయం_wr = mtimer_సమయం_wr64;
	}
#అయితే_ముగింపు

	/* Update MTIMER pointer in scratch space */
	కొరకు (i = 0; i < mt->hart_లెక్క; i++) {
		scratch = sbi_hartid_గమ్యం_scratch(mt->మొదటి_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ఒకవేళ (!scratch)
			కొనసాగు;
		mtimer_సమితి_hart_దత్తాంశం_ptr(scratch, mt);
	}

	ఒకవేళ (!mt->mtime_కొలత) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_విలువ = ఖాళీ;
	}

	/* Add MTIMER regions to the root domain */
	ఒకవేళ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_కొలత)) {
		rc = sbi_domain_మూలం_కూడు_memrange(mt->mtimecmp_addr,
					mt->mtime_కొలత + mt->mtimecmp_కొలత,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;
	} లేకపోతే ఒకవేళ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_కొలత)) {
		rc = sbi_domain_మూలం_కూడు_memrange(mt->mtime_addr,
					mt->mtime_కొలత + mt->mtimecmp_కొలత,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;
	} లేకపోతే {
		rc = sbi_domain_మూలం_కూడు_memrange(mt->mtime_addr,
						mt->mtime_కొలత, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;

		rc = sbi_domain_మూలం_కూడు_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_కొలత, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ఒకవేళ (rc)
			తిరిగి rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_సమితి_device(&mtimer);

	తిరిగి 0;
}
