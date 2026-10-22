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

ساكن بینښه اوږد mtimer_ptr_دننه_واټن;

#تعریف_کړه mtimer_get_hart_معلومات_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mtimer_ptr_دننه_واټن)

#تعریف_کړه mtimer_سېټ_hart_معلومات_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), فراغ *, mtimer_ptr_دننه_واټن, (__mtimer))

#إذا __riscv_xlen != 32
ساكن u64 mtimer_وخت_rd64(متطاير u64 *addr)
{
	أرجع readq_relaxed(addr);
}

ساكن فراغ mtimer_وخت_wr64(bool timecmp, u64 ارزښت, متطاير u64 *addr)
{
	writeq_relaxed(ارزښت, addr);
}
#د_که_پای

ساكن u64 mtimer_وخت_rd32(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ mtimer_وخت_wr32(bool timecmp, u64 ارزښت, متطاير u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (فراغ *)(addr));
	writel_relaxed((u32)(ارزښت >> 32), (حرف *)(addr) + 0x04);
	writel_relaxed((u32)ارزښت, (فراغ *)(addr));
}

ساكن u64 mtimer_ارزښت(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع 0;

	/* Read MTIMER Time Value */
	أرجع mt->وخت_rd((فراغ *)mt->mtime_addr);
}

ساكن فراغ mtimer_event_stop(فراغ)
{
	u32 موخه_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;
	u64 *وخت_cmp;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Clear MTIMER Time Compare */
	وخت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وخت_wr(true, -1ULL, &وخت_cmp[موخه_hart - mt->لومړی_hartid]);
}

ساكن فراغ mtimer_event_پیل(u64 بل_event)
{
	u32 موخه_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;
	u64 *وخت_cmp;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Program MTIMER Time Compare */
	وخت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وخت_wr(true, بل_event,
		    &وخت_cmp[موخه_hart - mt->لومړی_hartid]);
}

ساكن بنية sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_ارزښت = mtimer_ارزښت,
	.timer_event_پیل = mtimer_event_پیل,
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
	u64 *mt_وخت_val, *ref_وخت_val;

	إذا (!mt || !ref || !mt->وخت_rd || !mt->وخت_wr || !ref->وخت_rd)
		أرجع;

	mt_وخت_val = (فراغ *)mt->mtime_addr;
	ref_وخت_val = (فراغ *)ref->mtime_addr;
	إذا (!atomic_raw_xchg_ulong(&mt->وخت_delta_computed, 1)) {
		v1 = mt->وخت_rd(mt_وخت_val);
		mv = ref->وخت_rd(ref_وخت_val);
		v2 = mt->وخت_rd(mt_وخت_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->وخت_wr(false, mt->وخت_rd(mt_وخت_val) + delta,
			    mt_وخت_val);
	}
}

فراغ aclint_mtimer_sync(بنية aclint_mtimer_معلومات *mt)
{
	بنية aclint_mtimer_معلومات *reference;

	/* Sync-up non-shared MTIME if reference is available */
	إذا (mt->has_shared_mtime || !mt->وخت_delta_reference)
		أرجع;

	reference = mt->وخت_delta_reference;
	aclint_mtimer_update(mt, reference);
}

فراغ aclint_mtimer_سېټ_reference(بنية aclint_mtimer_معلومات *mt,
				 بنية aclint_mtimer_معلومات *ref)
{
	إذا (!mt || !ref || mt == ref)
		أرجع;

	mt->وخت_delta_reference = ref;
	mt->وخت_delta_computed = 0;
}

ساكن صحيح aclint_mtimer_warm_init(فراغ)
{
	u64 *mt_وخت_cmp;
	u32 موخه_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_معلومات *mt;

	mt = mtimer_get_hart_معلومات_ptr(scratch);
	إذا (!mt)
		أرجع SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_وخت_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->وخت_wr(true, -1ULL,
		    &mt_وخت_cmp[موخه_hart - mt->لومړی_hartid]);

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
	    (mt->hart_شمېر && !mt->mtimecmp_اندازه) ||
	    (mt->mtime_اندازه && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_اندازه && (mt->mtime_اندازه & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_اندازه & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_شمېر > ACLINT_MTIMER_MAX_HARTS))
		أرجع SBI_EINVAL;
	إذا (reference && mt->mtime_freq != reference->mtime_freq)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mtimer_ptr_دننه_واټن) {
		mtimer_ptr_دننه_واټن = sbi_scratch_alloc_type_دننه_واټن(فراغ *);
		إذا (!mtimer_ptr_دننه_واټن)
			أرجع SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_سېټ_reference(mt, reference);
	mt->وخت_rd = mtimer_وخت_rd32;
	mt->وخت_wr = mtimer_وخت_wr32;

	/* Override read/write accessors for 64bit MMIO */
#إذا __riscv_xlen != 32
	إذا (mt->has_64bit_mmio) {
		mt->وخت_rd = mtimer_وخت_rd64;
		mt->وخت_wr = mtimer_وخت_wr64;
	}
#د_که_پای

	/* Update MTIMER pointer in scratch space */
	لكل (i = 0; i < mt->hart_شمېر; i++) {
		scratch = sbi_hartid_منزل_scratch(mt->لومړی_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mtimer_سېټ_hart_معلومات_ptr(scratch, mt);
	}

	إذا (!mt->mtime_اندازه) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_ارزښت = عدم;
	}

	/* Add MTIMER regions to the root domain */
	إذا (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_اندازه)) {
		rc = sbi_domain_ریښه_جمع_کړه_memrange(mt->mtimecmp_addr,
					mt->mtime_اندازه + mt->mtimecmp_اندازه,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا إذا (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_اندازه)) {
		rc = sbi_domain_ریښه_جمع_کړه_memrange(mt->mtime_addr,
					mt->mtime_اندازه + mt->mtimecmp_اندازه,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا {
		rc = sbi_domain_ریښه_جمع_کړه_memrange(mt->mtime_addr,
						mt->mtime_اندازه, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		rc = sbi_domain_ریښه_جمع_کړه_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_اندازه, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_سېټ_device(&mtimer);

	أرجع 0;
}
