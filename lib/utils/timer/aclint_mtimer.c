/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_scratch.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/timer/aclint_mtimer.h>

ساکن غیرعلامتی طویل mtimer_ptr_انحراف;

#تعریف_کرو mtimer_get_hart_ڈیٹا_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), خالی *, mtimer_ptr_انحراف)

#تعریف_کرو mtimer_سیٹ_hart_ڈیٹا_ptr(__scratch, __mtimer)			\
	sbi_scratch_write_type((__scratch), خالی *, mtimer_ptr_انحراف, (__mtimer))

#اگر __riscv_xlen != 32
ساکن u64 mtimer_وقت_rd64(متلون u64 *addr)
{
	واپس readq_relaxed(addr);
}

ساکن خالی mtimer_وقت_wr64(bool timecmp, u64 قدر, متلون u64 *addr)
{
	writeq_relaxed(قدر, addr);
}
#اگر_ختم

ساکن u64 mtimer_وقت_rd32(متلون u64 *addr)
{
	u32 lo, hi;

	کرو {
		hi = readl_relaxed((u32 *)addr + 1);
		lo = readl_relaxed((u32 *)addr);
	} جب (hi != readl_relaxed((u32 *)addr + 1));

	واپس ((u64)hi << 32) | (u64)lo;
}

ساکن خالی mtimer_وقت_wr32(bool timecmp, u64 قدر, متلون u64 *addr)
{
	writel_relaxed((timecmp) ? -1U : 0U, (خالی *)(addr));
	writel_relaxed((u32)(قدر >> 32), (حرف *)(addr) + 0x04);
	writel_relaxed((u32)قدر, (خالی *)(addr));
}

ساکن u64 mtimer_قدر(خالی)
{
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت aclint_mtimer_ڈیٹا *mt;

	mt = mtimer_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mt)
		واپس 0;

	/* Read MTIMER Time Value */
	واپس mt->وقت_rd((خالی *)mt->mtime_addr);
}

ساکن خالی mtimer_event_stop(خالی)
{
	u32 ہدف_hart = current_hartid();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت aclint_mtimer_ڈیٹا *mt;
	u64 *وقت_cmp;

	mt = mtimer_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mt)
		واپس;

	/* Clear MTIMER Time Compare */
	وقت_cmp = (خالی *)mt->mtimecmp_addr;
	mt->وقت_wr(true, -1ULL, &وقت_cmp[ہدف_hart - mt->پہلا_hartid]);
}

ساکن خالی mtimer_event_آغاز(u64 اگلا_event)
{
	u32 ہدف_hart = current_hartid();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت aclint_mtimer_ڈیٹا *mt;
	u64 *وقت_cmp;

	mt = mtimer_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mt)
		واپس;

	/* Program MTIMER Time Compare */
	وقت_cmp = (خالی *)mt->mtimecmp_addr;
	mt->وقت_wr(true, اگلا_event,
		    &وقت_cmp[ہدف_hart - mt->پہلا_hartid]);
}

ساکن ساخت sbi_timer_device mtimer = {
	.name = "aclint-mtimer",
	.timer_قدر = mtimer_قدر,
	.timer_event_آغاز = mtimer_event_آغاز,
	.timer_event_stop = mtimer_event_stop
};

ساخت aclint_mtimer_ڈیٹا *aclint_get_mtimer_ڈیٹا(خالی)
{
	واپس mtimer_get_hart_ڈیٹا_ptr(sbi_scratch_thishart_ptr());
}

خالی aclint_mtimer_update(ساخت aclint_mtimer_ڈیٹا *mt,
			  ساخت aclint_mtimer_ڈیٹا *ref)
{
	u64 v1, v2, mv, delta;
	u64 *mt_وقت_val, *ref_وقت_val;

	اگر (!mt || !ref || !mt->وقت_rd || !mt->وقت_wr || !ref->وقت_rd)
		واپس;

	mt_وقت_val = (خالی *)mt->mtime_addr;
	ref_وقت_val = (خالی *)ref->mtime_addr;
	اگر (!atomic_raw_xchg_ulong(&mt->وقت_delta_computed, 1)) {
		v1 = mt->وقت_rd(mt_وقت_val);
		mv = ref->وقت_rd(ref_وقت_val);
		v2 = mt->وقت_rd(mt_وقت_val);
		delta = mv - ((v1 / 2) + (v2 / 2));
		mt->وقت_wr(false, mt->وقت_rd(mt_وقت_val) + delta,
			    mt_وقت_val);
	}
}

خالی aclint_mtimer_sync(ساخت aclint_mtimer_ڈیٹا *mt)
{
	ساخت aclint_mtimer_ڈیٹا *reference;

	/* Sync-up non-shared MTIME if reference is available */
	اگر (mt->has_shared_mtime || !mt->وقت_delta_reference)
		واپس;

	reference = mt->وقت_delta_reference;
	aclint_mtimer_update(mt, reference);
}

خالی aclint_mtimer_سیٹ_reference(ساخت aclint_mtimer_ڈیٹا *mt,
				 ساخت aclint_mtimer_ڈیٹا *ref)
{
	اگر (!mt || !ref || mt == ref)
		واپس;

	mt->وقت_delta_reference = ref;
	mt->وقت_delta_computed = 0;
}

ساکن صحیح aclint_mtimer_warm_init(خالی)
{
	u64 *mt_وقت_cmp;
	u32 ہدف_hart = current_hartid();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت aclint_mtimer_ڈیٹا *mt;

	mt = mtimer_get_hart_ڈیٹا_ptr(scratch);
	اگر (!mt)
		واپس SBI_ENODEV;

	/* Sync-up MTIME register */
	aclint_mtimer_sync(mt);

	/* Clear Time Compare */
	mt_وقت_cmp = (خالی *)mt->mtimecmp_addr;
	mt->وقت_wr(true, -1ULL,
		    &mt_وقت_cmp[ہدف_hart - mt->پہلا_hartid]);

	واپس 0;
}

صحیح aclint_mtimer_cold_init(ساخت aclint_mtimer_ڈیٹا *mt,
			    ساخت aclint_mtimer_ڈیٹا *reference)
{
	u32 i;
	صحیح rc;
	ساخت sbi_scratch *scratch;

	/* Sanity checks */
	اگر (!mt ||
	    (mt->hart_گنتی && !mt->mtimecmp_سائز) ||
	    (mt->mtime_سائز && (mt->mtime_addr & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtime_سائز && (mt->mtime_سائز & (ACLINT_MTIMER_ALIGN - 1))) ||
	    (mt->mtimecmp_addr & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->mtimecmp_سائز & (ACLINT_MTIMER_ALIGN - 1)) ||
	    (mt->hart_گنتی > ACLINT_MTIMER_MAX_HARTS))
		واپس SBI_EINVAL;
	اگر (reference && mt->mtime_freq != reference->mtime_freq)
		واپس SBI_EINVAL;

	/* Allocate scratch space pointer */
	اگر (!mtimer_ptr_انحراف) {
		mtimer_ptr_انحراف = sbi_scratch_alloc_type_انحراف(خالی *);
		اگر (!mtimer_ptr_انحراف)
			واپس SBI_ENOMEM;
	}

	/* Initialize private data */
	aclint_mtimer_سیٹ_reference(mt, reference);
	mt->وقت_rd = mtimer_وقت_rd32;
	mt->وقت_wr = mtimer_وقت_wr32;

	/* Override read/write accessors for 64bit MMIO */
#اگر __riscv_xlen != 32
	اگر (mt->has_64bit_mmio) {
		mt->وقت_rd = mtimer_وقت_rd64;
		mt->وقت_wr = mtimer_وقت_wr64;
	}
#اگر_ختم

	/* Update MTIMER pointer in scratch space */
	برائے (i = 0; i < mt->hart_گنتی; i++) {
		scratch = sbi_hartid_مقصد_scratch(mt->پہلا_hartid + i);
		/*
		 * We don't need to fail if scratch pointer is not available
		 * because we might be dealing with hartid of a HART disabled
		 * in the device tree.
		 */
		اگر (!scratch)
			جاری;
		mtimer_سیٹ_hart_ڈیٹا_ptr(scratch, mt);
	}

	اگر (!mt->mtime_سائز) {
		/* Disable reading mtime when mtime is not available */
		mtimer.timer_قدر = تہی;
	}

	/* Add MTIMER regions to the root domain */
	اگر (mt->mtime_addr == (mt->mtimecmp_addr + mt->mtimecmp_سائز)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(mt->mtimecmp_addr,
					mt->mtime_سائز + mt->mtimecmp_سائز,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;
	} ورنہ اگر (mt->mtimecmp_addr == (mt->mtime_addr + mt->mtime_سائز)) {
		rc = sbi_domain_جڑ_جوڑو_memrange(mt->mtime_addr,
					mt->mtime_سائز + mt->mtimecmp_سائز,
					MTIMER_REGION_ALIGN,
					(SBI_DOMAIN_MEMREGION_MMIO |
					 SBI_DOMAIN_MEMREGION_M_READABLE |
					 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;
	} ورنہ {
		rc = sbi_domain_جڑ_جوڑو_memrange(mt->mtime_addr,
						mt->mtime_سائز, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;

		rc = sbi_domain_جڑ_جوڑو_memrange(mt->mtimecmp_addr,
						mt->mtimecmp_سائز, MTIMER_REGION_ALIGN,
						(SBI_DOMAIN_MEMREGION_MMIO |
						 SBI_DOMAIN_MEMREGION_M_READABLE |
						 SBI_DOMAIN_MEMREGION_M_WRITABLE));
		اگر (rc)
			واپس rc;
	}

	mtimer.timer_freq = mt->mtime_freq;
	mtimer.warm_init = aclint_mtimer_warm_init;
	sbi_timer_سیٹ_device(&mtimer);

	واپس 0;
}
