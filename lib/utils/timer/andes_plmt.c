/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_timer.h>
#ضمن <sbi_utils/timer/andes_plmt.h>

بنية plmt_معلومات plmt;

ساكن u64 plmt_timer_قدر(فراغ)
{
#إذا __riscv_xlen == 64
	أرجع readq_relaxed(plmt.وقت_val);
#وإلا
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((فراغ *)plmt.وقت_val + 0x04);
		lo = readl_relaxed(plmt.وقت_val);
	} طالما (hi != readl_relaxed((فراغ *)plmt.وقت_val + 0x04));

	أرجع ((u64)hi << 32) | (u64)lo;
#جيڪڏهن_پورو
}

ساكن فراغ plmt_timer_event_stop(فراغ)
{
	u32 هدف_hart = current_hartid();

	إذا (plmt.hart_ڳڻپ <= هدف_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#إذا __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.وقت_cmp[هدف_hart]);
#وإلا
	writel_relaxed(-1UL, &plmt.وقت_cmp[هدف_hart]);
	writel_relaxed(-1UL, (فراغ *)(&plmt.وقت_cmp[هدف_hart]) + 0x04);
#جيڪڏهن_پورو
}

ساكن فراغ plmt_timer_event_شروعات(u64 ايندڙ_event)
{
	u32 هدف_hart = current_hartid();

	إذا (plmt.hart_ڳڻپ <= هدف_hart)
		ebreak();

	/* Program PLMT Time Compare */
#إذا __riscv_xlen == 64
	writeq_relaxed(ايندڙ_event, &plmt.وقت_cmp[هدف_hart]);
#وإلا
	u32 نقاب = -1UL;

	writel_relaxed(ايندڙ_event & نقاب, &plmt.وقت_cmp[هدف_hart]);
	writel_relaxed(ايندڙ_event >> 32,
		       (فراغ *)(&plmt.وقت_cmp[هدف_hart]) + 0x04);
#جيڪڏهن_پورو
}

ساكن صحيح plmt_warm_timer_init(فراغ)
{
	إذا (!plmt.وقت_val)
		أرجع SBI_ENODEV;

	plmt_timer_event_stop();

	أرجع 0;
}

ساكن بنية sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_قدر	   = plmt_timer_قدر,
	.timer_event_شروعات = plmt_timer_event_شروعات,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

صحيح plmt_cold_timer_init(بنية plmt_معلومات *plmt)
{
	صحيح rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_پاڙ_جوڙيو_memrange(
		(بنانشان ڊگهو)plmt->وقت_val, plmt->سائيز,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_سيٽ_device(&plmt_timer);

	أرجع 0;
}
