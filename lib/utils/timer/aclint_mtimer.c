/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_scratch.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/timer/aclint_mtimer.h>

ساكن unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), فراغ *, mtimer_ptr_offset, (__mtimer))

#إذا __riscv_xlen != 32
ساكن u64 mtimer_time_rd64(متطاير u64 *addr)
{
	أرجع readq_relaxed(addr);
}

ساكن فراغ mtimer_time_wr64(bool timecmp, u64 بەها, متطاير u64 *addr)
{
	writeq_relaxed(بەها, addr);
}
#endif

ساكن u64 mtimer_time_rd32(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ mtimer_time_wr32(bool timecmp, u64 بەها, متطاير u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (فراغ *)(addr));
	writel_relaxed((u32)(بەها >> 32), (حرف *)(addr) + 0x04);
	writel_relaxed((u32)بەها, (فراغ *)(addr));
}

ساكن u64 mtimer_بەها(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	إذا (!mt)
		أرجع 0;

	/* Read MTIMER Time Value */
	أرجع mt->time_rd((فراغ *)mt->mtime_addr);
}

ساكن فراغ mtimer_event_stop(فراغ)
{
	u32 target_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Clear MTIMER Time Compare */
	time_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

ساكن فراغ mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Program MTIMER Time Compare */
	time_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

ساكن بنية sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_بەها = mtimer_بەها,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

بنية aclint_mtimer_data *aclint_get_mtimer_data(فراغ)
{
	أرجع mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

فراغ aclint_mtimer_update(بنية aclint_mtimer_data *mt,
			  بنية aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	إذا (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		أرجع;

	mt_time_val = (فراغ *)mt->mtime_addr;
	ref_time_val = (فراغ *)ref->mtime_addr;
	إذا (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

فراغ aclint_mtimer_sync(بنية aclint_mtimer_data *mt)
{
	بنية aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	إذا (mt->has_shared_mtime || !mt->time_delta_reference)
		أرجع;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

فراغ aclint_mtimer_set_reference(بنية aclint_mtimer_data *mt,
				 بنية aclint_mtimer_data *ref)
{
	إذا (!mt || !ref || mt == ref)
		أرجع;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

ساكن صحيح aclint_mtimer_warm_init(فراغ)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	إذا (!mt)
		أرجع SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	أرجع 0;
}

صحيح aclint_mtimer_cold_init(بنية aclint_mtimer_data *mt,
			    بنية aclint_mtimer_data *reference)
{
	u32 i;
	صحيح rc;
	بنية sbi_scratch *scratch;

	/* Sanity checks */
	إذا (!mt ||
	    (mt->hart_ژماردن && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ژماردن > ACLINT_MTIMER_MAX_HARTS))
		أرجع SBI_EINVAL;
	إذا (reference && mt->mtime_freq != reference->mtime_freq)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(فراغ *);
		إذا (!mtimer_ptr_offset)
			أرجع SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#إذا __riscv_xlen != 32
	إذا (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	لكل (i = 0; i < mt->hart_ژماردن; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	إذا (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_بەها = عدم;
	}

	/* Add MTIMER regions to the root domain */
	إذا (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_ڕەگ_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا إذا (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_ڕەگ_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا {
		rc = sbi_domain_ڕەگ_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		rc = sbi_domain_ڕەگ_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	أرجع 0;
}
