/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/timer/aclint_mtimer.h>

अटल अचिन्हित दीर्घ mtimer_ptr_offset;

#घोषणा mtimer_get_hart_तथ्य_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), व्योम *, mtimer_ptr_offset)

#घोषणा mtimer_set_hart_तथ्य_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), व्योम *, mtimer_ptr_offset, (__mtimer))

#अगर __riscv_xlen != 32
अटल u64 mtimer_time_rd64(अस्थिर u64 *addr)
{
	वापस readq_relaxed(addr);
}

अटल व्योम mtimer_time_wr64(bool timecmp, u64 मूल्य, अस्थिर u64 *addr)
{
	writeq_relaxed(मूल्य, addr);
}
#पूर्ण_अगर

अटल u64 mtimer_time_rd32(अस्थिर u64 *addr)
{
	u32 lo, hi;

	करो {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} जबतक (hi != readl_relaxed((u32 *)addr + 1));

	वापस ((u64)hi << 32) | (u64)lo;
}

अटल व्योम mtimer_time_wr32(bool timecmp, u64 मूल्य, अस्थिर u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (व्योम *)(addr));
	writel_relaxed((u32)(मूल्य >> 32), (अक्षर *)(addr) + 0x04);
	writel_relaxed((u32)मूल्य, (व्योम *)(addr));
}

अटल u64 mtimer_मूल्य(व्योम)
{
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा aclint_mtimer_तथ्य *mt;

	mt = mtimer_get_hart_तथ्य_ptr(scratch);
	अगर (!mt)
		वापस 0;

	/* Read MTIMER Time Value */
	वापस mt->time_rd((व्योम *)mt->mtime_addr);
}

अटल व्योम mtimer_event_stop(व्योम)
{
	u32 target_hart = current_hartid();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा aclint_mtimer_तथ्य *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_तथ्य_ptr(scratch);
	अगर (!mt)
		वापस;

	/* Clear MTIMER Time Compare */
	time_cmp = (व्योम *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL, &time_cmp[target_hart - mt->first_hartid]);
}

अटल व्योम mtimer_event_start(u64 अग्र_event)
{
	u32 target_hart = current_hartid();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा aclint_mtimer_तथ्य *mt;
	u64 *time_cmp;

	mt = mtimer_get_hart_तथ्य_ptr(scratch);
	अगर (!mt)
		वापस;

	/* Program MTIMER Time Compare */
	time_cmp = (व्योम *)mt->mtimecmp_addr;
	mt->time_wr(true, अग्र_event,
		    &time_cmp[target_hart - mt->first_hartid]);
}

अटल काष्ठा sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_मूल्य = mtimer_मूल्य,
	.timer_event_start = mtimer_event_start,
	.timer_event_stop = mtimer_event_stop
};

काष्ठा aclint_mtimer_तथ्य *aclint_get_mtimer_तथ्य(व्योम)
{
	वापस mtimer_get_hart_तथ्य_ptr(sbi_scratch_thishart_ptr());
}

व्योम aclint_mtimer_update(काष्ठा aclint_mtimer_तथ्य *mt,
			  काष्ठा aclint_mtimer_तथ्य *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_time_val, *ref_time_val;

	अगर (!mt || !ref || !mt->time_rd || !mt->time_wr || !ref->time_rd)
		वापस;

	mt_time_val = (व्योम *)mt->mtime_addr;
	ref_time_val = (व्योम *)ref->mtime_addr;
	अगर (!atomic_raw_xchg_ulong(&mt->time_delta_computed, 1)) {
		v1 = mt->time_rd(mt_time_val);
		mv = ref->time_rd(ref_time_val);
		v2 = mt->time_rd(mt_time_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->time_wr(false, mt->time_rd(mt_time_val) + delta,
			    mt_time_val);
	}
}

व्योम aclint_mtimer_sync(काष्ठा aclint_mtimer_तथ्य *mt)
{
	काष्ठा aclint_mtimer_तथ्य *reference;

	/* Sync-up non-shared MTIME if reference is available */
	अगर (mt->has_shared_mtime || !mt->time_delta_reference)
		वापस;

	reference = mt->time_delta_reference;
	aclint_mtimer_update(mt, reference);
}

व्योम aclint_mtimer_set_reference(काष्ठा aclint_mtimer_तथ्य *mt,
				 काष्ठा aclint_mtimer_तथ्य *ref)
{
	अगर (!mt || !ref || mt == ref)
		वापस;

	mt->time_delta_reference = ref;
	mt->time_delta_computed = 0;
}

अटल पूर्णांक aclint_mtimer_warm_init(व्योम)
{
	u64 *mt_time_cmp;
	u32 target_hart = current_hartid();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा aclint_mtimer_तथ्य *mt;

	mt = mtimer_get_hart_तथ्य_ptr(scratch);
	अगर (!mt)
		वापस SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_time_cmp = (व्योम *)mt->mtimecmp_addr;
	mt->time_wr(true, -1ULL,
		    &mt_time_cmp[target_hart - mt->first_hartid]);

	वापस 0;
}

पूर्णांक aclint_mtimer_cold_init(काष्ठा aclint_mtimer_तथ्य *mt,
			    काष्ठा aclint_mtimer_तथ्य *reference)
{
	u32 i;
	पूर्णांक rc;
	काष्ठा sbi_scratch *scratch;

	/* Sanity checks */
	अगर (!mt ||
	    (mt->hart_गिनती && !mt->mtimecmp_आकार) ||
	    (mt->mtime_आकार && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_आकार && (mt->mtime_आकार & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_आकार & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_गिनती > ACLINT_MTIMER_MAX_HARTS))
		वापस SBI_EINVAL;
	अगर (reference && mt->mtime_freq != reference->mtime_freq)
		वापस SBI_EINVAL;

	/* Allocate scratch space pointer */
	अगर (!mtimer_ptr_offset) {
		mtimer_ptr_offset = sbi_scratch_alloc_type_offset(व्योम *);
		अगर (!mtimer_ptr_offset)
			वापस SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_set_reference(mt, reference);
	mt->time_rd = mtimer_time_rd32;
	mt->time_wr = mtimer_time_wr32;

	/* Override read/write accessors for 64bit MMIO */
#अगर __riscv_xlen != 32
	अगर (mt->has_64bit_mmio) {
		mt->time_rd = mtimer_time_rd64;
		mt->time_wr = mtimer_time_wr64;
	}
#पूर्ण_अगर

	/* Update MTIMER pointer in scratch space */
	क्रम (i = 0; i < mt->hart_गिनती; i++) {
		scratch = sbi_hartid_to_scratch(mt->first_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		अगर (!scratch)
			जारी;
		mtimer_set_hart_तथ्य_ptr(scratch, mt);
	}

	अगर (!mt->mtime_आकार) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_मूल्य = शून्य;
	}

	/* Add MTIMER regions to the root domain */
	अगर (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_आकार)) {
		rc = sbi_domain_मूल_add_memrange(mt->mtimecmp_addr,
					mt->mtime_आकार + mt->mtimecmp_आकार,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;
	} अन्यथा अगर (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_आकार)) {
		rc = sbi_domain_मूल_add_memrange(mt->mtime_addr,
					mt->mtime_आकार + mt->mtimecmp_आकार,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;
	} अन्यथा {
		rc = sbi_domain_मूल_add_memrange(mt->mtime_addr,
						mt->mtime_आकार, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;

		rc = sbi_domain_मूल_add_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_आकार, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		अगर (rc)
			वापस rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_set_device(&mtimer);

	वापस 0;
}
