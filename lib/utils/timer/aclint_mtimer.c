/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#அந்தர்பாவ <sbi/riscv_asm.h>
#அந்தர்பாவ <sbi/riscv_atomic.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_bitops.h>
#அந்தர்பாவ <sbi/sbi_domain.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_scratch.h>
#அந்தர்பாவ <sbi/sbi_timer.h>
#அந்தர்பாவ <sbi_utils/timer/aclint_mtimer.h>

நிலையான unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), வெறுமை *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), வெறுமை *, mtimer_ptr_offset, (__mtimer))

#எனில் __riscv_xlen != 32
நிலையான u64 mtimer_time_rd64(நிலையற்ற u64 *addr)
{
	திரும்பு readq_relaxed(addr);
}

நிலையான வெறுமை mtimer_time_wr64(bool timecmp, u64 மதிப்பு, நிலையற்ற u64 *addr)
{
	writeq_relaxed(மதிப்பு, addr);
}
#endif

நிலையான u64 mtimer_time_rd32(நிலையற்ற u64 *addr)
{
	u32 lo, hi;

	செய் {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} போது (hi != readl_relaxed((u32 *)addr + 1));

	திரும்பு ((u64)hi << 32) | (u64)lo;
}

நிலையான வெறுமை mtimer_time_wr32(bool timecmp, u64 மதிப்பு, நிலையற்ற u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (வெறுமை *)(addr));
	writel_relaxed((u32)(மதிப்பு >> 32), (எழுத்து *)(addr) + 0x04);
	writel_relaxed((u32)மதிப்பு, (வெறுமை *)(addr));
}

நிலையான u64 mtimer_மதிப்பு(வெறுமை)
{
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	எனில் (!mt)
		திரும்பு 0;

	/* Read MTIMER Time Value */
	திரும்பு mt->time_rd((வெறுமை *)mt->mtime_addr);
}

நிலையான வெறுமை mtimer_event_stop(வெறுமை)
{
	u32 target_hart = current_hartid();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	எனில் (!mt)
		திரும்பு;

	/* Clear MTIMER Time Compare */
	time_cmp = (வெறுமை *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

நிலையான வெறுமை mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	எனில் (!mt)
		திரும்பு;

	/* Program MTIMER Time Compare */
	time_cmp = (வெறுமை *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

நிலையான கட்டமைப்பு sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_மதிப்பு = mtimer_மதிப்பு,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

கட்டமைப்பு aclint_mtimer_data *aclint_get_mtimer_data(வெறுமை)
{
	திரும்பு mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

வெறுமை aclint_mtimer_update(கட்டமைப்பு aclint_mtimer_data *mt,
			  கட்டமைப்பு aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	எனில் (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		திரும்பு;

	mt_time_val = (வெறுமை *)mt->mtime_addr;
	ref_time_val = (வெறுமை *)ref->mtime_addr;
	எனில் (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

வெறுமை aclint_mtimer_sync(கட்டமைப்பு aclint_mtimer_data *mt)
{
	கட்டமைப்பு aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	எனில் (mt->has_shared_mtime || !mt->time_delta_reference)
		திரும்பு;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

வெறுமை aclint_mtimer_set_reference(கட்டமைப்பு aclint_mtimer_data *mt,
				 கட்டமைப்பு aclint_mtimer_data *ref)
{
	எனில் (!mt || !ref || mt == ref)
		திரும்பு;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

நிலையான முழு aclint_mtimer_warm_init(வெறுமை)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	கட்டமைப்பு sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	கட்டமைப்பு aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	எனில் (!mt)
		திரும்பு SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (வெறுமை *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	திரும்பு 0;
}

முழு aclint_mtimer_cold_init(கட்டமைப்பு aclint_mtimer_data *mt,
			    கட்டமைப்பு aclint_mtimer_data *reference)
{
	u32 i;
	முழு rc;
	கட்டமைப்பு sbi_scratch *scratch;

	/* Sanity checks */
	எனில் (!mt ||
	    (mt->hart_மொத்தம் && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_மொத்தம் > ACLINT_MTIMER_MAX_HARTS))
		திரும்பு SBI_EINVAL;
	எனில் (reference && mt->mtime_freq != reference->mtime_freq)
		திரும்பு SBI_EINVAL;

	/* Allocate scratch space pointer */
	எனில் (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(வெறுமை *);
		எனில் (!mtimer_ptr_offset)
			திரும்பு SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#எனில் __riscv_xlen != 32
	எனில் (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	க்கு (i = 0; i < mt->hart_மொத்தம்; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		எனில் (!scratch)
			தொடர்;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	எனில் (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_மதிப்பு = வெற்று;
	}

	/* Add MTIMER regions to the root domain */
	எனில் (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_வேர்_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (rc)
			திரும்பு rc;
	} இல்லையெனில் எனில் (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_வேர்_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (rc)
			திரும்பு rc;
	} இல்லையெனில் {
		rc = sbi_domain_வேர்_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (rc)
			திரும்பு rc;

		rc = sbi_domain_வேர்_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		எனில் (rc)
			திரும்பு rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	திரும்பு 0;
}
