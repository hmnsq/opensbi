/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_atomic.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_bitops.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/timer/aclint_mtimer.h>

स्थिर unsigned long mtimer_ptr_offset;

#define mtimer_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), शून्य *, mtimer_ptr_offset)

#define mtimer_set_hart_data_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), शून्य *, mtimer_ptr_offset, (__mtimer))

#यदि __riscv_xlen != 32
स्थिर u64 mtimer_time_rd64(चञ्चल u64 *addr)
{
	प्रत्यागम readq_relaxed(addr);
}

स्थिर शून्य mtimer_time_wr64(bool timecmp, u64 मूल्य, चञ्चल u64 *addr)
{
	writeq_relaxed(मूल्य, addr);
}
#endif

स्थिर u64 mtimer_time_rd32(चञ्चल u64 *addr)
{
	u32 lo, hi;

	कुरु {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} यावत् (hi != readl_relaxed((u32 *)addr + 1));

	प्रत्यागम ((u64)hi << 32) | (u64)lo;
}

स्थिर शून्य mtimer_time_wr32(bool timecmp, u64 मूल्य, चञ्चल u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (शून्य *)(addr));
	writel_relaxed((u32)(मूल्य >> 32), (वर्ण *)(addr) + 0x04);
	writel_relaxed((u32)मूल्य, (शून्य *)(addr));
}

स्थिर u64 mtimer_मूल्य(शून्य)
{
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	यदि (!mt)
		प्रत्यागम 0;

	/* Read MTIMER Time Value */
	प्रत्यागम mt->time_rd((शून्य *)mt->mtime_addr);
}

स्थिर शून्य mtimer_event_stop(शून्य)
{
	u32 target_hart = current_hartid();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	यदि (!mt)
		प्रत्यागम;

	/* Clear MTIMER Time Compare */
	time_cmp = (शून्य *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

स्थिर शून्य mtimer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mtimer_data *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_data_ptr(scratch);
	यदि (!mt)
		प्रत्यागम;

	/* Program MTIMER Time Compare */
	time_cmp = (शून्य *)mt->mtimecmp_addr;
	mt->time_wr(true, next_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

स्थिर संरचना sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_मूल्य = mtimer_मूल्य,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

संरचना aclint_mtimer_data *aclint_get_mtimer_data(शून्य)
{
	प्रत्यागम mtimer_get_hart_data_ptr(sbi_scratch_thishart_ptr());
}

शून्य aclint_mtimer_update(संरचना aclint_mtimer_data *mt,
			  संरचना aclint_mtimer_data *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	यदि (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		प्रत्यागम;

	mt_time_val = (शून्य *)mt->mtime_addr;
	ref_time_val = (शून्य *)ref->mtime_addr;
	यदि (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

शून्य aclint_mtimer_sync(संरचना aclint_mtimer_data *mt)
{
	संरचना aclint_mtimer_data *reference;

	/* Sync-up non-shared MTIME if reference is available */
	यदि (mt->has_shared_mtime || !mt->time_delta_reference)
		प्रत्यागम;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

शून्य aclint_mtimer_set_reference(संरचना aclint_mtimer_data *mt,
				 संरचना aclint_mtimer_data *ref)
{
	यदि (!mt || !ref || mt == ref)
		प्रत्यागम;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

स्थिर पूर्णाङ्क aclint_mtimer_warm_init(शून्य)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	संरचना sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	संरचना aclint_mtimer_data *mt;

	mt = mtimer_get_hart_data_ptr(scratch);
	यदि (!mt)
		प्रत्यागम SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (शून्य *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	प्रत्यागम 0;
}

पूर्णाङ्क aclint_mtimer_cold_init(संरचना aclint_mtimer_data *mt,
			    संरचना aclint_mtimer_data *reference)
{
	u32 i;
	पूर्णाङ्क rc;
	संरचना sbi_scratch *scratch;

	/* Sanity checks */
	यदि (!mt ||
	    (mt->hart_गिनती && !mt->mtimecmp_size) ||
	    (mt->mtime_size && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_size && (mt->mtime_size & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_size & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_गिनती > ACLINT_MTIMER_MAX_HARTS))
		प्रत्यागम SBI_EINVAL;
	यदि (reference && mt->mtime_freq != reference->mtime_freq)
		प्रत्यागम SBI_EINVAL;

	/* Allocate scratch space pointer */
	यदि (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(शून्य *);
		यदि (!mtimer_ptr_offset)
			प्रत्यागम SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#यदि __riscv_xlen != 32
	यदि (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#endif

	/* Update MTIMER pointer in scratch space */
	प्रति (i = 0; i < mt->hart_गिनती; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		यदि (!scratch)
			अनुवर्त;
		mtimer_set_hart_data_ptr(scratch, mt);
	}

	यदि (!mt->mtime_size) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_मूल्य = रिक्त;
	}

	/* Add MTIMER regions to the root domain */
	यदि (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_size)) {
		rc = sbi_domain_मूल_add_memrange(mt->mtimecmp_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;
	} अन्यथा यदि (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_size)) {
		rc = sbi_domain_मूल_add_memrange(mt->mtime_addr,
					mt->mtime_size + mt->mtimecmp_size,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;
	} अन्यथा {
		rc = sbi_domain_मूल_add_memrange(mt->mtime_addr,
						mt->mtime_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;

		rc = sbi_domain_मूल_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_size, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		यदि (rc)
			प्रत्यागम rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	प्रत्यागम 0;
}
