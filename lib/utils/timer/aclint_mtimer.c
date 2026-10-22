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

ಸ್ಥಿರ unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ಶೂನ್ಯ *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), ಶೂನ್ಯ *, mtimer_ptr_offset, (__mtimer))

#ಯದಿ __riscv_xlen != 32
ಸ್ಥಿರ u64 mtimer_time_rd64(ಚಞ್ಚಲ u64 *addr)
{
	ಪ್ರತ್ಯಾಗಮ readq_relaxed(addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_time_wr64(bool timecmp, u64 ಮೌಲ್ಯ, ಚಞ್ಚಲ u64 *addr)
{
	writeq_relaxed(ಮೌಲ್ಯ, addr);
}
#endif

ಸ್ಥಿರ u64 mtimer_time_rd32(ಚಞ್ಚಲ u64 *addr)
{
	u32 lo, hi;

	ಕುರು {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ಯಾವತ್ (hi != readl_relaxed((u32 *)addr + 1));

	ಪ್ರತ್ಯಾಗಮ ((u64)hi << 32) | (u64)lo;
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_time_wr32(bool timecmp, u64 ಮೌಲ್ಯ, ಚಞ್ಚಲ u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (ಶೂನ್ಯ *)(addr));
	writel_relaxed((u32)(ಮೌಲ್ಯ >> 32), (ವರ್ಣ *)(addr) + 0x04);
	writel_relaxed((u32)ಮೌಲ್ಯ, (ಶೂನ್ಯ *)(addr));
}

ಸ್ಥಿರ u64 mtimer_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ಯದಿ (!mt)
		ಪ್ರತ್ಯಾಗಮ 0;

	/* Read MTIMER Time Value */
	ಪ್ರತ್ಯಾಗಮ mt->time_rd((ಶೂನ್ಯ *)mt->mtime_addr);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_event_stop(ಶೂನ್ಯ)
{
	u32 target_hart = current_hartid();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ಯದಿ (!mt)
		ಪ್ರತ್ಯಾಗಮ;

	/* Clear MTIMER Time Compare */
	time_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

ಸ್ಥಿರ ಶೂನ್ಯ mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ಯದಿ (!mt)
		ಪ್ರತ್ಯಾಗಮ;

	/* Program MTIMER Time Compare */
	time_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_ಮೌಲ್ಯ = mtimer_ಮೌಲ್ಯ,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

ಸಂರಚನಾ aclint_mtimer_data *aclint_get_mtimer_data(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

ಶೂನ್ಯ aclint_mtimer_update(ಸಂರಚನಾ aclint_mtimer_data *mt,
			  ಸಂರಚನಾ aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	ಯದಿ (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		ಪ್ರತ್ಯಾಗಮ;

	mt_time_val = (ಶೂನ್ಯ *)mt->mtime_addr;
	ref_time_val = (ಶೂನ್ಯ *)ref->mtime_addr;
	ಯದಿ (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

ಶೂನ್ಯ aclint_mtimer_sync(ಸಂರಚನಾ aclint_mtimer_data *mt)
{
	ಸಂರಚನಾ aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ಯದಿ (mt->has_shared_mtime || !mt->time_delta_reference)
		ಪ್ರತ್ಯಾಗಮ;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

ಶೂನ್ಯ aclint_mtimer_set_reference(ಸಂರಚನಾ aclint_mtimer_data *mt,
				 ಸಂರಚನಾ aclint_mtimer_data *ref)
{
	ಯದಿ (!mt || !ref || mt == ref)
		ಪ್ರತ್ಯಾಗಮ;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aclint_mtimer_warm_init(ಶೂನ್ಯ)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	ಸಂರಚನಾ sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ಸಂರಚನಾ aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ಯದಿ (!mt)
		ಪ್ರತ್ಯಾಗಮ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (ಶೂನ್ಯ *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ aclint_mtimer_cold_init(ಸಂರಚನಾ aclint_mtimer_data *mt,
			    ಸಂರಚನಾ aclint_mtimer_data *reference)
{
	u32 i;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಸಂರಚನಾ sbi_scratch *scratch;

	/* Sanity checks */
	ಯದಿ (!mt ||
	    (mt->hart_ಎಣಿಕೆ && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ಎಣಿಕೆ > ACLINT_MTIMER_MAX_HARTS))
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
	ಯದಿ (reference && mt->mtime_freq != reference->mtime_freq)
		ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ಯದಿ (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(ಶೂನ್ಯ *);
		ಯದಿ (!mtimer_ptr_offset)
			ಪ್ರತ್ಯಾಗಮ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ಯದಿ __riscv_xlen != 32
	ಯದಿ (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	ಪ್ರತಿ (i = 0; i < mt->hart_ಎಣಿಕೆ; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ಯದಿ (!scratch)
			ಅನುವರ್ತ;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	ಯದಿ (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_ಮೌಲ್ಯ = ರಿಕ್ತ;
	}

	/* Add MTIMER regions to the root domain */
	ಯದಿ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_ಬೇರು_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	} ಅನ್ಯಥಾ ಯದಿ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_ಬೇರು_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	} ಅನ್ಯಥಾ {
		rc = sbi_domain_ಬೇರು_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;

		rc = sbi_domain_ಬೇರು_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ಯದಿ (rc)
			ಪ್ರತ್ಯಾಗಮ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	ಪ್ರತ್ಯಾಗಮ 0;
}
