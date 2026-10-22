/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#અન્તર્ભાવ <sbi/riscv_asm.h>
#અન્તર્ભાવ <sbi/riscv_atomic.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_bitops.h>
#અન્તર્ભાવ <sbi/sbi_domain.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>
#અન્તર્ભાવ <sbi/sbi_timer.h>
#અન્તર્ભાવ <sbi_utils/timer/aclint_mtimer.h>

સ્થિર unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), શૂન્ય *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), શૂન્ય *, mtimer_ptr_offset, (__mtimer))

#જો __riscv_xlen != 32
સ્થિર u64 mtimer_time_rd64(અસ્થિર u64 *addr)
{
	પાછા readq_relaxed(addr);
}

સ્થિર શૂન્ય mtimer_time_wr64(bool timecmp, u64 કિંમત, અસ્થિર u64 *addr)
{
	writeq_relaxed(કિંમત, addr);
}
#endif

સ્થિર u64 mtimer_time_rd32(અસ્થિર u64 *addr)
{
	u32 lo, hi;

	કરો {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} જ્યારે (hi != readl_relaxed((u32 *)addr + 1));

	પાછા ((u64)hi << 32) | (u64)lo;
}

સ્થિર શૂન્ય mtimer_time_wr32(bool timecmp, u64 કિંમત, અસ્થિર u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (શૂન્ય *)(addr));
	writel_relaxed((u32)(કિંમત >> 32), (અક્ષર *)(addr) + 0x04);
	writel_relaxed((u32)કિંમત, (શૂન્ય *)(addr));
}

સ્થિર u64 mtimer_કિંમત(શૂન્ય)
{
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	જો (!mt)
		પાછા 0;

	/* Read MTIMER Time Value */
	પાછા mt->time_rd((શૂન્ય *)mt->mtime_addr);
}

સ્થિર શૂન્ય mtimer_event_stop(શૂન્ય)
{
	u32 target_hart = current_hartid();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	જો (!mt)
		પાછા;

	/* Clear MTIMER Time Compare */
	time_cmp = (શૂન્ય *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

સ્થિર શૂન્ય mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	જો (!mt)
		પાછા;

	/* Program MTIMER Time Compare */
	time_cmp = (શૂન્ય *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

સ્થિર સંરચના sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_કિંમત = mtimer_કિંમત,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

સંરચના aclint_mtimer_data *aclint_get_mtimer_data(શૂન્ય)
{
	પાછા mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

શૂન્ય aclint_mtimer_update(સંરચના aclint_mtimer_data *mt,
			  સંરચના aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	જો (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		પાછા;

	mt_time_val = (શૂન્ય *)mt->mtime_addr;
	ref_time_val = (શૂન્ય *)ref->mtime_addr;
	જો (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

શૂન્ય aclint_mtimer_sync(સંરચના aclint_mtimer_data *mt)
{
	સંરચના aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	જો (mt->has_shared_mtime || !mt->time_delta_reference)
		પાછા;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

શૂન્ય aclint_mtimer_set_reference(સંરચના aclint_mtimer_data *mt,
				 સંરચના aclint_mtimer_data *ref)
{
	જો (!mt || !ref || mt == ref)
		પાછા;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

સ્થિર પૂર્ણાંક aclint_mtimer_warm_init(શૂન્ય)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	સંરચના sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	સંરચના aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	જો (!mt)
		પાછા SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (શૂન્ય *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	પાછા 0;
}

પૂર્ણાંક aclint_mtimer_cold_init(સંરચના aclint_mtimer_data *mt,
			    સંરચના aclint_mtimer_data *reference)
{
	u32 i;
	પૂર્ણાંક rc;
	સંરચના sbi_scratch *scratch;

	/* Sanity checks */
	જો (!mt ||
	    (mt->hart_ગણતરી && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ગણતરી > ACLINT_MTIMER_MAX_HARTS))
		પાછા SBI_EINVAL;
	જો (reference && mt->mtime_freq != reference->mtime_freq)
		પાછા SBI_EINVAL;

	/* Allocate scratch space pointer */
	જો (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(શૂન્ય *);
		જો (!mtimer_ptr_offset)
			પાછા SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#જો __riscv_xlen != 32
	જો (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	માટે (i = 0; i < mt->hart_ગણતરી; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		જો (!scratch)
			ચાલુ;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	જો (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_કિંમત = ખાલી;
	}

	/* Add MTIMER regions to the root domain */
	જો (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_મૂળ_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (rc)
			પાછા rc;
	} નહીંતર જો (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_મૂળ_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (rc)
			પાછા rc;
	} નહીંતર {
		rc = sbi_domain_મૂળ_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (rc)
			પાછા rc;

		rc = sbi_domain_મૂળ_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		જો (rc)
			પાછા rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	પાછા 0;
}
