/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_atomic.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_bitops.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_scratch.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/timer/aclint_mtimer.h>

സ്ഥിതം unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), ശൂന്യം *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), ശൂന്യം *, mtimer_ptr_offset, (__mtimer))

#എങ്കിൽ __riscv_xlen != 32
സ്ഥിതം u64 mtimer_time_rd64(അസ്ഥിര u64 *addr)
{
	തിരികെ readq_relaxed(addr);
}

സ്ഥിതം ശൂന്യം mtimer_time_wr64(bool timecmp, u64 മൂല്യം, അസ്ഥിര u64 *addr)
{
	writeq_relaxed(മൂല്യം, addr);
}
#endif

സ്ഥിതം u64 mtimer_time_rd32(അസ്ഥിര u64 *addr)
{
	u32 lo, hi;

	ചെയ്യുക {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} ആകുമ്പോൾ (hi != readl_relaxed((u32 *)addr + 1));

	തിരികെ ((u64)hi << 32) | (u64)lo;
}

സ്ഥിതം ശൂന്യം mtimer_time_wr32(bool timecmp, u64 മൂല്യം, അസ്ഥിര u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (ശൂന്യം *)(addr));
	writel_relaxed((u32)(മൂല്യം >> 32), (അക്ഷരം *)(addr) + 0x04);
	writel_relaxed((u32)മൂല്യം, (ശൂന്യം *)(addr));
}

സ്ഥിതം u64 mtimer_മൂല്യം(ശൂന്യം)
{
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mt)
		തിരികെ 0;

	/* Read MTIMER Time Value */
	തിരികെ mt->time_rd((ശൂന്യം *)mt->mtime_addr);
}

സ്ഥിതം ശൂന്യം mtimer_event_stop(ശൂന്യം)
{
	u32 target_hart = current_hartid();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mt)
		തിരികെ;

	/* Clear MTIMER Time Compare */
	time_cmp = (ശൂന്യം *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

സ്ഥിതം ശൂന്യം mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mt)
		തിരികെ;

	/* Program MTIMER Time Compare */
	time_cmp = (ശൂന്യം *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

സ്ഥിതം ഘടന sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_മൂല്യം = mtimer_മൂല്യം,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

ഘടന aclint_mtimer_data *aclint_get_mtimer_data(ശൂന്യം)
{
	തിരികെ mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

ശൂന്യം aclint_mtimer_update(ഘടന aclint_mtimer_data *mt,
			  ഘടന aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	എങ്കിൽ (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		തിരികെ;

	mt_time_val = (ശൂന്യം *)mt->mtime_addr;
	ref_time_val = (ശൂന്യം *)ref->mtime_addr;
	എങ്കിൽ (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

ശൂന്യം aclint_mtimer_sync(ഘടന aclint_mtimer_data *mt)
{
	ഘടന aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	എങ്കിൽ (mt->has_shared_mtime || !mt->time_delta_reference)
		തിരികെ;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

ശൂന്യം aclint_mtimer_set_reference(ഘടന aclint_mtimer_data *mt,
				 ഘടന aclint_mtimer_data *ref)
{
	എങ്കിൽ (!mt || !ref || mt == ref)
		തിരികെ;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ aclint_mtimer_warm_init(ശൂന്യം)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	ഘടന sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ഘടന aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	എങ്കിൽ (!mt)
		തിരികെ SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (ശൂന്യം *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ aclint_mtimer_cold_init(ഘടന aclint_mtimer_data *mt,
			    ഘടന aclint_mtimer_data *reference)
{
	u32 i;
	പൂർണ്ണസംഖ്യ rc;
	ഘടന sbi_scratch *scratch;

	/* Sanity checks */
	എങ്കിൽ (!mt ||
	    (mt->hart_എണ്ണം && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_എണ്ണം > ACLINT_MTIMER_MAX_HARTS))
		തിരികെ SBI_EINVAL;
	എങ്കിൽ (reference && mt->mtime_freq != reference->mtime_freq)
		തിരികെ SBI_EINVAL;

	/* Allocate scratch space pointer */
	എങ്കിൽ (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(ശൂന്യം *);
		എങ്കിൽ (!mtimer_ptr_offset)
			തിരികെ SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#എങ്കിൽ __riscv_xlen != 32
	എങ്കിൽ (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	വേണ്ടി (i = 0; i < mt->hart_എണ്ണം; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		എങ്കിൽ (!scratch)
			തുടരുക;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	എങ്കിൽ (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_മൂല്യം = ശൂന്യ;
	}

	/* Add MTIMER regions to the root domain */
	എങ്കിൽ (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_വേര്_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (rc)
			തിരികെ rc;
	} അല്ലെങ്കിൽ എങ്കിൽ (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_വേര്_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (rc)
			തിരികെ rc;
	} അല്ലെങ്കിൽ {
		rc = sbi_domain_വേര്_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (rc)
			തിരികെ rc;

		rc = sbi_domain_വേര്_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		എങ്കിൽ (rc)
			തിരികെ rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	തിരികെ 0;
}
