/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#অন্তর্ভাব <sbi/riscv_asm.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_domain.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_timer.h>
#অন্তর্ভাব <sbi_utils/timer/andes_plmt.h>

কাঠামো plmt_তথ্য plmt;

স্থির u64 plmt_timer_মান(শূন্য)
{
#যদি __riscv_xlen == 64
	ফেরত readq_relaxed(plmt.time_val);
#অন্যথা
	u32 lo, hi;

	করো {
		hi = readl_relaxed((শূন্য *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} যখন (hi != readl_relaxed((শূন্য *)plmt.time_val + 0x04));

	ফেরত ((u64)hi << 32) | (u64)lo;
#endif
}

স্থির শূন্য plmt_timer_event_stop(শূন্য)
{
	u32 target_hart = current_hartid();

	যদি (plmt.hart_গুনতি <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#যদি __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#অন্যথা
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (শূন্য *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

স্থির শূন্য plmt_timer_event_start(u64 পরবর্তী_event)
{
	u32 target_hart = current_hartid();

	যদি (plmt.hart_গুনতি <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#যদি __riscv_xlen == 64
	writeq_relaxed(পরবর্তী_event, &plmt.time_cmp[target_hart]);
#অন্যথা
	u32 mask = -1UL;

	writel_relaxed(পরবর্তী_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(পরবর্তী_event >> 32,
		       (শূন্য *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

স্থির পূর্ণসংখ্যা plmt_warm_timer_init(শূন্য)
{
	যদি (!plmt.time_val)
		ফেরত SBI_ENODEV;

	plmt_timer_event_stop();

	ফেরত 0;
}

স্থির কাঠামো sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_মান	   = plmt_timer_মান,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

পূর্ণসংখ্যা plmt_cold_timer_init(কাঠামো plmt_তথ্য *plmt)
{
	পূর্ণসংখ্যা rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_মূল_add_memrange(
		(unsigned long)plmt->time_val, plmt->আকার_2,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	যদি (rc)
		ফেরত rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	ফেরত 0;
}
