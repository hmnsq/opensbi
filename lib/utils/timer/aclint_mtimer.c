/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/timer/aclint_mtimer.h>

ایستا unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), تهی *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), تهی *, mtimer_ptr_offset, (__mtimer))

#اگر __riscv_xlen != 32
ایستا u64 mtimer_time_rd64(فرار u64 *addr)
{
	بازگشت readq_relaxed(addr);
}

ایستا تهی mtimer_time_wr64(bool timecmp, u64 مقدار, فرار u64 *addr)
{
	writeq_relaxed(مقدار, addr);
}
#endif

ایستا u64 mtimer_time_rd32(فرار u64 *addr)
{
	u32 lo, hi;

	انجامبده {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} وقتی (hi != readl_relaxed((u32 *)addr + 1));

	بازگشت ((u64)hi << 32) | (u64)lo;
}

ایستا تهی mtimer_time_wr32(bool timecmp, u64 مقدار, فرار u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (تهی *)(addr));
	writel_relaxed((u32)(مقدار >> 32), (نویسه *)(addr) + 0x04);
	writel_relaxed((u32)مقدار, (تهی *)(addr));
}

ایستا u64 mtimer_مقدار(تهی)
{
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	اگر (!mt)
		بازگشت 0;

	/* Read MTIMER Time Value */
	بازگشت mt->time_rd((تهی *)mt->mtime_addr);
}

ایستا تهی mtimer_event_stop(تهی)
{
	u32 target_hart = current_hartid();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	اگر (!mt)
		بازگشت;

	/* Clear MTIMER Time Compare */
	time_cmp = (تهی *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

ایستا تهی mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	اگر (!mt)
		بازگشت;

	/* Program MTIMER Time Compare */
	time_cmp = (تهی *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

ایستا ساختار sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_مقدار = mtimer_مقدار,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

ساختار aclint_mtimer_data *aclint_get_mtimer_data(تهی)
{
	بازگشت mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

تهی aclint_mtimer_update(ساختار aclint_mtimer_data *mt,
			  ساختار aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	اگر (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		بازگشت;

	mt_time_val = (تهی *)mt->mtime_addr;
	ref_time_val = (تهی *)ref->mtime_addr;
	اگر (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

تهی aclint_mtimer_sync(ساختار aclint_mtimer_data *mt)
{
	ساختار aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	اگر (mt->has_shared_mtime || !mt->time_delta_reference)
		بازگشت;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

تهی aclint_mtimer_set_reference(ساختار aclint_mtimer_data *mt,
				 ساختار aclint_mtimer_data *ref)
{
	اگر (!mt || !ref || mt == ref)
		بازگشت;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

ایستا صحیح aclint_mtimer_warm_init(تهی)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	اگر (!mt)
		بازگشت SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (تهی *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	بازگشت 0;
}

صحیح aclint_mtimer_cold_init(ساختار aclint_mtimer_data *mt,
			    ساختار aclint_mtimer_data *reference)
{
	u32 i;
	صحیح rc;
	ساختار sbi_scratch *scratch;

	/* Sanity checks */
	اگر (!mt ||
	    (mt->hart_شمار && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_شمار > ACLINT_MTIMER_MAX_HARTS))
		بازگشت SBI_EINVAL;
	اگر (reference && mt->mtime_freq != reference->mtime_freq)
		بازگشت SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(تهی *);
		اگر (!mtimer_ptr_offset)
			بازگشت SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#اگر __riscv_xlen != 32
	اگر (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	برای (i = 0; i < mt->hart_شمار; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		اگر (!scratch)
			ادامه;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	اگر (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_مقدار = پوچ;
	}

	/* Add MTIMER regions to the root domain */
	اگر (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_ریشه_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			بازگشت rc;
	} وگرنه اگر (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_ریشه_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			بازگشت rc;
	} وگرنه {
		rc = sbi_domain_ریشه_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			بازگشت rc;

		rc = sbi_domain_ریشه_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			بازگشت rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	بازگشت 0;
}
