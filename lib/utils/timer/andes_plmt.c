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

بنية plmt_انکڑے plmt;

ساكن u64 plmt_timer_قدر(فراغ)
{
#إذا __riscv_xlen == 64
	أرجع readq_relaxed(plmt.ویلا_val);
#وإلا
	u32 lo, hi;

	افعل {
		hi = readl_relaxed((فراغ *)plmt.ویلا_val + 0x04);
		lo = readl_relaxed(plmt.ویلا_val);
	} طالما (hi != readl_relaxed((فراغ *)plmt.ویلا_val + 0x04));

	أرجع ((u64)hi << 32) | (u64)lo;
#جے_انت
}

ساكن فراغ plmt_timer_event_stop(فراغ)
{
	u32 نشانہ_hart = current_hartid();

	إذا (plmt.hart_گنتی <= نشانہ_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#إذا __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.ویلا_cmp[نشانہ_hart]);
#وإلا
	writel_relaxed(-1UL, &plmt.ویلا_cmp[نشانہ_hart]);
	writel_relaxed(-1UL, (فراغ *)(&plmt.ویلا_cmp[نشانہ_hart]) + 0x04);
#جے_انت
}

ساكن فراغ plmt_timer_event_شروع(u64 اگلا_event)
{
	u32 نشانہ_hart = current_hartid();

	إذا (plmt.hart_گنتی <= نشانہ_hart)
		ebreak();

	/* Program PLMT Time Compare */
#إذا __riscv_xlen == 64
	writeq_relaxed(اگلا_event, &plmt.ویلا_cmp[نشانہ_hart]);
#وإلا
	u32 نقاب = -1UL;

	writel_relaxed(اگلا_event & نقاب, &plmt.ویلا_cmp[نشانہ_hart]);
	writel_relaxed(اگلا_event >> 32,
		       (فراغ *)(&plmt.ویلا_cmp[نشانہ_hart]) + 0x04);
#جے_انت
}

ساكن صحيح plmt_warm_timer_init(فراغ)
{
	إذا (!plmt.ویلا_val)
		أرجع SBI_ENODEV;

	plmt_timer_event_stop();

	أرجع 0;
}

ساكن بنية sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_قدر	   = plmt_timer_قدر,
	.timer_event_شروع = plmt_timer_event_شروع,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

صحيح plmt_cold_timer_init(بنية plmt_انکڑے *plmt)
{
	صحيح rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_جڑ_جوڑو_memrange(
		(بنانشانی لما)plmt->ویلا_val, plmt->ناپ,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	إذا (rc)
		أرجع rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_سیٹ_device(&plmt_timer);

	أرجع 0;
}
