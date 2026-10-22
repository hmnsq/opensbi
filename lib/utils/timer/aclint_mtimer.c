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

ساكن بنانشان ڊگهو mtimer_ptr_وٿي;

#وصف_ڪريو mtimer_get_hart_معلومات_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mtimer_ptr_وٿي)

#وصف_ڪريو mtimer_سيٽ_hart_معلومات_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), فراغ *, mtimer_ptr_وٿي, (__mtimer))

#إذا __riscv_xlen != 32
ساكن u64 mtimer_وقت_rd64(متطاير u64 *addr)
{
	أرجع readq_relaxed(addr);
}

ساكن فراغ mtimer_وقت_wr64(bool timecmp, u64 قدر, متطاير u64 *addr)
{
	writeq_relaxed(قدر, addr);
}
#جيڪڏهن_پورو

ساكن u64 mtimer_وقت_rd32(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ mtimer_وقت_wr32(bool timecmp, u64 قدر, متطاير u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (فراغ *)(addr));
	writel_relaxed((u32)(قدر >> 32), (حرف *)(addr) + 0x04);
	writel_relaxed((u32)قدر, (فراغ *)(addr));
}

ساكن u64 mtimer_قدر(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع 0;

	/* Read MTIMER Time Value */
	أرجع mt->وقت_rd((فراغ *)mt->mtime_addr);
}

ساكن فراغ mtimer_event_stop(فراغ)
{
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;
	u64 *وقت_cmp;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Clear MTIMER Time Compare */
	وقت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وقت_wr(true, -1ULL, &وقت_cmp[هدف_hart - mt->پهريون_hartid]);
}

ساكن فراغ mtimer_event_شروعات(u64 ايندڙ_event)
{
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;
	u64 *وقت_cmp;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Program MTIMER Time Compare */
	وقت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وقت_wr(true, ايندڙ_event,
		    &وقت_cmp[هدف_hart - mt->پهريون_hartid]);
}

ساكن بنية sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_قدر = mtimer_قدر,
	.timer_event_شروعات = mtimer_event_شروعات,
	.timer_event_stop = mtimer_event_stop
};

بنية aclint_mtimer_معلومات *aclint_get_mtimer_معلومات(فراغ)
{
	أرجع mtimer_get_hart_معلومات_ptr(sbi_scratch_thishart_ptr());
}

فراغ aclint_mtimer_update(بنية aclint_mtimer_معلومات *mt,
			  بنية aclint_mtimer_معلومات *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_وقت_val, *ref_وقت_val;

	إذا (!mt || !ref || !mt->وقت_rd || !mt->وقت_wr || !ref->وقت_rd)
		أرجع;

	mt_وقت_val = (فراغ *)mt->mtime_addr;
	ref_وقت_val = (فراغ *)ref->mtime_addr;
	إذا (!atomic_raw_xchg_ulong(&mt->وقت_delta_computed, 1)) {
		v1 = mt->وقت_rd(mt_وقت_val);
		mv = ref->وقت_rd(ref_وقت_val);
		v2 = mt->وقت_rd(mt_وقت_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->وقت_wr(false, mt->وقت_rd(mt_وقت_val) + delta,
			    mt_وقت_val);
	}
}

فراغ aclint_mtimer_sync(بنية aclint_mtimer_معلومات *mt)
{
	بنية aclint_mtimer_معلومات *reference;

	/* Sync-up non-shared MTIME if reference is available */
	إذا (mt->has_shared_mtime || !mt->وقت_delta_reference)
		أرجع;

	reference = mt->وقت_delta_reference;
	aclint_mtimer_update(mt, reference);
}

فراغ aclint_mtimer_سيٽ_reference(بنية aclint_mtimer_معلومات *mt,
				 بنية aclint_mtimer_معلومات *ref)
{
	إذا (!mt || !ref || mt == ref)
		أرجع;

	mt->وقت_delta_reference = ref;
	mt->وقت_delta_computed = 0;
}

ساكن صحيح aclint_mtimer_warm_init(فراغ)
{
	u64 *mt_وقت_cmp;
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_وقت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وقت_wr(true, -1ULL,
		    &mt_وقت_cmp[هدف_hart - mt->پهريون_hartid]);

	أرجع 0;
}

صحيح aclint_mtimer_cold_init(بنية aclint_mtimer_معلومات *mt,
			    بنية aclint_mtimer_معلومات *reference)
{
	u32 i;
	صحيح rc;
	بنية sbi_scratch *scratch;

	/* Sanity checks */
	إذا (!mt ||
	    (mt->hart_ڳڻپ && !mt->mtimecmp_سائيز) ||
	    (mt->mtime_سائيز && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_سائيز && (mt->mtime_سائيز & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_سائيز & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_ڳڻپ > ACLINT_MTIMER_MAX_HARTS))
		أرجع SBI_EINVAL;
	إذا (reference && mt->mtime_freq != reference->mtime_freq)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mtimer_ptr_وٿي) {
		mtimer_ptr_وٿي = sbi_scratch_alloc_type_وٿي(فراغ *);
		إذا (!mtimer_ptr_وٿي)
			أرجع SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_سيٽ_reference(mt, reference);
	mt->وقت_rd = mtimer_وقت_rd32;
	mt->وقت_wr = mtimer_وقت_wr32;

	/* Override read/write accessors for 64bit MMIO */
#إذا __riscv_xlen != 32
	إذا (mt->has_64bit_mmio) {
		mt->وقت_rd = mtimer_وقت_rd64;
		mt->وقت_wr = mtimer_وقت_wr64;
	}
#جيڪڏهن_پورو

	/* Update MTIMER pointer in scratch space */
	لكل (i = 0; i < mt->hart_ڳڻپ; i++) {
		scratch = sbi_hartid_منزل_scratch(mt->پهريون_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mtimer_سيٽ_hart_معلومات_ptr(scratch, mt);
	}

	إذا (!mt->mtime_سائيز) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_قدر = عدم;
	}

	/* Add MTIMER regions to the root domain */
	إذا (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_سائيز)) {
		rc = sbi_domain_پاڙ_جوڙيو_memrange(mt->mtimecmp_addr,
					mt->mtime_سائيز + mt->mtimecmp_سائيز,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا إذا (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_سائيز)) {
		rc = sbi_domain_پاڙ_جوڙيو_memrange(mt->mtime_addr,
					mt->mtime_سائيز + mt->mtimecmp_سائيز,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا {
		rc = sbi_domain_پاڙ_جوڙيو_memrange(mt->mtime_addr,
						mt->mtime_سائيز, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		rc = sbi_domain_پاڙ_جوڙيو_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_سائيز, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_سيٽ_device(&mtimer);

	أرجع 0;
}
