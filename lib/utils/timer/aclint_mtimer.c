/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_asm.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_atomic.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_bitops.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_domain.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_scratch.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_timer.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/timer/aclint_mtimer.h>

ସ୍ଥିର unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ଶୂନ୍ୟ *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), ଶୂନ୍ୟ *, mtimer_ptr_offset, (__mtimer))

#ଯଦି __riscv_xlen != 32
ସ୍ଥିର u64 mtimer_time_rd64(ଅସ୍ଥିର u64 *addr)
{
	ଫେରନ୍ତୁ readq_relaxed(addr);
}

ସ୍ଥିର ଶୂନ୍ୟ mtimer_time_wr64(bool timecmp, u64 ମୂଲ୍ୟ, ଅସ୍ଥିର u64 *addr)
{
	writeq_relaxed(ମୂଲ୍ୟ, addr);
}
#endif

ସ୍ଥିର u64 mtimer_time_rd32(ଅସ୍ଥିର u64 *addr)
{
	u32 lo, hi;

	କର {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ଯେତେବେଳେ (hi != readl_relaxed((u32 *)addr + 1));

	ଫେରନ୍ତୁ ((u64)hi << 32) | (u64)lo;
}

ସ୍ଥିର ଶୂନ୍ୟ mtimer_time_wr32(bool timecmp, u64 ମୂଲ୍ୟ, ଅସ୍ଥିର u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (ଶୂନ୍ୟ *)(addr));
	writel_relaxed((u32)(ମୂଲ୍ୟ >> 32), (ଅକ୍ଷର *)(addr) + 0x04);
	writel_relaxed((u32)ମୂଲ୍ୟ, (ଶୂନ୍ୟ *)(addr));
}

ସ୍ଥିର u64 mtimer_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ଯଦି (!mt)
		ଫେରନ୍ତୁ 0;

	/* Read MTIMER Time Value */
	ଫେରନ୍ତୁ mt->time_rd((ଶୂନ୍ୟ *)mt->mtime_addr);
}

ସ୍ଥିର ଶୂନ୍ୟ mtimer_event_stop(ଶୂନ୍ୟ)
{
	u32 target_hart = current_hartid();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ଯଦି (!mt)
		ଫେରନ୍ତୁ;

	/* Clear MTIMER Time Compare */
	time_cmp = (ଶୂନ୍ୟ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

ସ୍ଥିର ଶୂନ୍ୟ mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ଯଦି (!mt)
		ଫେରନ୍ତୁ;

	/* Program MTIMER Time Compare */
	time_cmp = (ଶୂନ୍ୟ *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

ସ୍ଥିର ସଂରଚନା sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_ମୂଲ୍ୟ = mtimer_ମୂଲ୍ୟ,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

ସଂରଚନା aclint_mtimer_data *aclint_get_mtimer_data(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

ଶୂନ୍ୟ aclint_mtimer_update(ସଂରଚନା aclint_mtimer_data *mt,
			  ସଂରଚନା aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	ଯଦି (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		ଫେରନ୍ତୁ;

	mt_time_val = (ଶୂନ୍ୟ *)mt->mtime_addr;
	ref_time_val = (ଶୂନ୍ୟ *)ref->mtime_addr;
	ଯଦି (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

ଶୂନ୍ୟ aclint_mtimer_sync(ସଂରଚନା aclint_mtimer_data *mt)
{
	ସଂରଚନା aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ଯଦି (mt->has_shared_mtime || !mt->time_delta_reference)
		ଫେରନ୍ତୁ;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

ଶୂନ୍ୟ aclint_mtimer_set_reference(ସଂରଚନା aclint_mtimer_data *mt,
				 ସଂରଚନା aclint_mtimer_data *ref)
{
	ଯଦି (!mt || !ref || mt == ref)
		ଫେରନ୍ତୁ;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aclint_mtimer_warm_init(ଶୂନ୍ୟ)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	ସଂରଚନା sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ସଂରଚନା aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ଯଦି (!mt)
		ଫେରନ୍ତୁ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (ଶୂନ୍ୟ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା aclint_mtimer_cold_init(ସଂରଚନା aclint_mtimer_data *mt,
			    ସଂରଚନା aclint_mtimer_data *reference)
{
	u32 i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ସଂରଚନା sbi_scratch *scratch;

	/* Sanity checks */
	ଯଦି (!mt ||
	    (mt->hart_ଗଣନା && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ଗଣନା > ACLINT_MTIMER_MAX_HARTS))
		ଫେରନ୍ତୁ SBI_EINVAL;
	ଯଦି (reference && mt->mtime_freq != reference->mtime_freq)
		ଫେରନ୍ତୁ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ଯଦି (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(ଶୂନ୍ୟ *);
		ଯଦି (!mtimer_ptr_offset)
			ଫେରନ୍ତୁ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ଯଦି __riscv_xlen != 32
	ଯଦି (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	ପାଇଁ (i = 0; i < mt->hart_ଗଣନା; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ଯଦି (!scratch)
			ଜାରି;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	ଯଦି (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_ମୂଲ୍ୟ = ଖାଲି;
	}

	/* Add MTIMER regions to the root domain */
	ଯଦି (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_ମୂଳ_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	} ଅନ୍ୟଥା ଯଦି (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_ମୂଳ_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	} ଅନ୍ୟଥା {
		rc = sbi_domain_ମୂଳ_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;

		rc = sbi_domain_ମୂଳ_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ଯଦି (rc)
			ଫେରନ୍ତୁ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	ଫେରନ୍ତୁ 0;
}
