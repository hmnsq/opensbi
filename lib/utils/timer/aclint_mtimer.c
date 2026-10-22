/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_atomic.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_bitops.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/timer/aclint_mtimer.h>

static unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), void *, mtimer_ptr_offset, (__mtimer))

#ប្រសិនបើ __riscv_xlen != 32
static u64 mtimer_time_rd64(ca_ncala u64 *addr)
{
	ត្រឡប់ readq_relaxed(addr);
}

static void mtimer_time_wr64(bool timecmp, u64 value, ca_ncala u64 *addr)
{
	writeq_relaxed(value, addr);
}
#endif

static u64 mtimer_time_rd32(ca_ncala u64 *addr)
{
	u32 lo, hi;

	do {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ខណៈពេល (hi != readl_relaxed((u32 *)addr + 1));

	ត្រឡប់ ((u64)hi << 32) | (u64)lo;
}

static void mtimer_time_wr32(bool timecmp, u64 value, ca_ncala u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (void *)(addr));
	writel_relaxed((u32)(value >> 32), (var_na *)(addr) + 0x04);
	writel_relaxed((u32)value, (void *)(addr));
}

static u64 mtimer_value(void)
{
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ប្រសិនបើ (!mt)
		ត្រឡប់ 0;

	/* Read MTIMER Time Value */
	ត្រឡប់ mt->time_rd((void *)mt->mtime_addr);
}

static void mtimer_event_stop(void)
{
	u32 target_hart = current_hartid();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ប្រសិនបើ (!mt)
		ត្រឡប់;

	/* Clear MTIMER Time Compare */
	time_cmp = (void *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

static void mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	ប្រសិនបើ (!mt)
		ត្រឡប់;

	/* Program MTIMER Time Compare */
	time_cmp = (void *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

static sa_nracanaa sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_value = mtimer_value,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

sa_nracanaa aclint_mtimer_data *aclint_get_mtimer_data(void)
{
	ត្រឡប់ mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

void aclint_mtimer_update(sa_nracanaa aclint_mtimer_data *mt,
			  sa_nracanaa aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	ប្រសិនបើ (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		ត្រឡប់;

	mt_time_val = (void *)mt->mtime_addr;
	ref_time_val = (void *)ref->mtime_addr;
	ប្រសិនបើ (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

void aclint_mtimer_sync(sa_nracanaa aclint_mtimer_data *mt)
{
	sa_nracanaa aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	ប្រសិនបើ (mt->has_shared_mtime || !mt->time_delta_reference)
		ត្រឡប់;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

void aclint_mtimer_set_reference(sa_nracanaa aclint_mtimer_data *mt,
				 sa_nracanaa aclint_mtimer_data *ref)
{
	ប្រសិនបើ (!mt || !ref || mt == ref)
		ត្រឡប់;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

static ចំនួនគត់ aclint_mtimer_warm_init(void)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	sa_nracanaa sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sa_nracanaa aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	ប្រសិនបើ (!mt)
		ត្រឡប់ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (void *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	ត្រឡប់ 0;
}

ចំនួនគត់ aclint_mtimer_cold_init(sa_nracanaa aclint_mtimer_data *mt,
			    sa_nracanaa aclint_mtimer_data *reference)
{
	u32 i;
	ចំនួនគត់ rc;
	sa_nracanaa sbi_scratch *scratch;

	/* Sanity checks */
	ប្រសិនបើ (!mt ||
	    (mt->hart_count && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_count > ACLINT_MTIMER_MAX_HARTS))
		ត្រឡប់ SBI_EINVAL;
	ប្រសិនបើ (reference && mt->mtime_freq != reference->mtime_freq)
		ត្រឡប់ SBI_EINVAL;

	/* Allocate scratch space pointer */
	ប្រសិនបើ (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ប្រសិនបើ (!mtimer_ptr_offset)
			ត្រឡប់ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#ប្រសិនបើ __riscv_xlen != 32
	ប្រសិនបើ (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	សម្រាប់ (i = 0; i < mt->hart_count; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		ប្រសិនបើ (!scratch)
			continue;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	ប្រសិនបើ (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_value = NULL;
	}

	/* Add MTIMER regions to the root domain */
	ប្រសិនបើ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_root_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;
	} បើមិនដូច្នេះ ប្រសិនបើ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_root_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;
	} បើមិនដូច្នេះ {
		rc = sbi_domain_root_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;

		rc = sbi_domain_root_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		ប្រសិនបើ (rc)
			ត្រឡប់ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	ត្រឡប់ 0;
}
