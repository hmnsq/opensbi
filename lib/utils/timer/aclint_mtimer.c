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

ساكن بیعلامت دراز mtimer_ptr_جابه_جایی;

#تعریف_کن mtimer_get_hart_داده_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), فراغ *, mtimer_ptr_جابه_جایی)

#تعریف_کن mtimer_مجموعه_hart_داده_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), فراغ *, mtimer_ptr_جابه_جایی, (__mtimer))

#إذا __riscv_xlen != 32
ساكن u64 mtimer_زمان_rd64(متطاير u64 *addr)
{
	أرجع readq_relaxed(addr);
}

ساكن فراغ mtimer_زمان_wr64(bool timecmp, u64 مقدار, متطاير u64 *addr)
{
	writeq_relaxed(مقدار, addr);
}
#پایان_اگر

ساكن u64 mtimer_زمان_rd32(متطاير u64 *addr)
{
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} طالما (hi != readl_relaxed((u32 *)addr + 1));

	أرجع ((u64)hi << 32) | (u64)lo;
}

ساكن فراغ mtimer_زمان_wr32(bool timecmp, u64 مقدار, متطاير u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (فراغ *)(addr));
	writel_relaxed((u32)(مقدار >> 32), (حرف *)(addr) + 0x04);
	writel_relaxed((u32)مقدار, (فراغ *)(addr));
}

ساكن u64 mtimer_مقدار(فراغ)
{
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_داده *mt;

	mt = mtimer_get_hart_داده_ptr(scratch);
	إذا (!mt)
		أرجع 0;

	/* Read MTIMER Time Value */
	أرجع mt->زمان_rd((فراغ *)mt->mtime_addr);
}

ساكن فراغ mtimer_event_stop(فراغ)
{
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_داده *mt;
	u64 *زمان_cmp;

	mt = mtimer_get_hart_داده_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Clear MTIMER Time Compare */
	زمان_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->زمان_wr(true, -1ULL, &زمان_cmp[هدف_hart - mt->اولی_hartid]);
}

ساكن فراغ mtimer_event_شروع(u64 بعدی_event)
{
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_داده *mt;
	u64 *زمان_cmp;

	mt = mtimer_get_hart_داده_ptr(scratch);
	إذا (!mt)
		أرجع;

	/* Program MTIMER Time Compare */
	زمان_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->زمان_wr(true, بعدی_event,
		    &زمان_cmp[هدف_hart - mt->اولی_hartid]);
}

ساكن بنية sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_مقدار = mtimer_مقدار,
	.timer_event_شروع = mtimer_event_شروع,
	.timer_event_stop = mtimer_event_stop
};

بنية aclint_mtimer_داده *aclint_get_mtimer_داده(فراغ)
{
	أرجع mtimer_get_hart_داده_ptr(sbi_scratch_thishart_ptr());
}

فراغ aclint_mtimer_update(بنية aclint_mtimer_داده *mt,
			  بنية aclint_mtimer_داده *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_زمان_val, *ref_زمان_val;

	إذا (!mt || !ref || !mt->زمان_rd || !mt->زمان_wr || !ref->زمان_rd)
		أرجع;

	mt_زمان_val = (فراغ *)mt->mtime_addr;
	ref_زمان_val = (فراغ *)ref->mtime_addr;
	إذا (!atomic_raw_xchg_ulong(&mt->زمان_delta_computed, 1)) {
		v1 = mt->زمان_rd(mt_زمان_val);
		mv = ref->زمان_rd(ref_زمان_val);
		v2 = mt->زمان_rd(mt_زمان_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->زمان_wr(false, mt->زمان_rd(mt_زمان_val) + delta,
			    mt_زمان_val);
	}
}

فراغ aclint_mtimer_sync(بنية aclint_mtimer_داده *mt)
{
	بنية aclint_mtimer_داده *reference;

	/* Sync-up non-shared MTIME if reference is available */
	إذا (mt->has_shared_mtime || !mt->زمان_delta_reference)
		أرجع;

	reference = mt->زمان_delta_reference;
	aclint_mtimer_update(mt, reference);
}

فراغ aclint_mtimer_مجموعه_reference(بنية aclint_mtimer_داده *mt,
				 بنية aclint_mtimer_داده *ref)
{
	إذا (!mt || !ref || mt == ref)
		أرجع;

	mt->زمان_delta_reference = ref;
	mt->زمان_delta_computed = 0;
}

ساكن صحيح aclint_mtimer_warm_init(فراغ)
{
	u64 *mt_زمان_cmp;
	u32 هدف_hart = current_hartid();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية aclint_mtimer_داده *mt;

	mt = mtimer_get_hart_داده_ptr(scratch);
	إذا (!mt)
		أرجع SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_زمان_cmp = (فراغ *)mt->mtimecmp_addr;
	mt->زمان_wr(true, -1ULL,
		    &mt_زمان_cmp[هدف_hart - mt->اولی_hartid]);

	أرجع 0;
}

صحيح aclint_mtimer_cold_init(بنية aclint_mtimer_داده *mt,
			    بنية aclint_mtimer_داده *reference)
{
	u32 i;
	صحيح rc;
	بنية sbi_scratch *scratch;

	/* Sanity checks */
	إذا (!mt ||
	    (mt->hart_شمار && !mt->mtimecmp_اندازه) ||
	    (mt->mtime_اندازه && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_اندازه && (mt->mtime_اندازه & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_اندازه & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_شمار > ACLINT_MTIMER_MAX_HARTS))
		أرجع SBI_EINVAL;
	إذا (reference && mt->mtime_freq != reference->mtime_freq)
		أرجع SBI_EINVAL;

	/* Allocate scratch space pointer */
	إذا (!mtimer_ptr_جابه_جایی) {
		mtimer_ptr_جابه_جایی = sbi_scratch_alloc_type_جابه_جایی(فراغ *);
		إذا (!mtimer_ptr_جابه_جایی)
			أرجع SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_مجموعه_reference(mt, reference);
	mt->زمان_rd = mtimer_زمان_rd32;
	mt->زمان_wr = mtimer_زمان_wr32;

	/* Override read/write accessors for 64bit MMIO */
#إذا __riscv_xlen != 32
	إذا (mt->has_64bit_mmio) {
		mt->زمان_rd = mtimer_زمان_rd64;
		mt->زمان_wr = mtimer_زمان_wr64;
	}
#پایان_اگر

	/* Update MTIMER pointer in scratch space */
	لكل (i = 0; i < mt->hart_شمار; i++) {
		scratch = sbi_hartid_مقصد_scratch(mt->اولی_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		إذا (!scratch)
			استمر;
		mtimer_مجموعه_hart_داده_ptr(scratch, mt);
	}

	إذا (!mt->mtime_اندازه) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_مقدار = عدم;
	}

	/* Add MTIMER regions to the root domain */
	إذا (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_اندازه)) {
		rc = sbi_domain_ریشه_جمع_کن_memrange(mt->mtimecmp_addr,
					mt->mtime_اندازه + mt->mtimecmp_اندازه,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا إذا (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_اندازه)) {
		rc = sbi_domain_ریشه_جمع_کن_memrange(mt->mtime_addr,
					mt->mtime_اندازه + mt->mtimecmp_اندازه,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	} وإلا {
		rc = sbi_domain_ریشه_جمع_کن_memrange(mt->mtime_addr,
						mt->mtime_اندازه, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;

		rc = sbi_domain_ریشه_جمع_کن_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_اندازه, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		إذا (rc)
			أرجع rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_مجموعه_device(&mtimer);

	أرجع 0;
}
