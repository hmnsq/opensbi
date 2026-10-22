/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#അന്തര്ഭാവ <sbi/riscv_asm.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_domain.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_timer.h>
#അന്തര്ഭാവ <sbi_utils/timer/andes_plmt.h>

ഘടന plmt_data plmt;

സ്ഥിതം u64 plmt_timer_മൂല്യം(ശൂന്യം)
{
#എങ്കിൽ __riscv_xlen == 64
	തിരികെ readq_relaxed(plmt.time_val);
#അല്ലെങ്കിൽ
	u32 lo, hi;

	ചെയ്യുക {
		hi = readl_relaxed((ശൂന്യം *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} ആകുമ്പോൾ (hi != readl_relaxed((ശൂന്യം *)plmt.time_val + 0x04));

	തിരികെ ((u64)hi << 32) | (u64)lo;
#endif
}

സ്ഥിതം ശൂന്യം plmt_timer_event_stop(ശൂന്യം)
{
	u32 target_hart = current_hartid();

	എങ്കിൽ (plmt.hart_എണ്ണം <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#എങ്കിൽ __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#അല്ലെങ്കിൽ
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (ശൂന്യം *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

സ്ഥിതം ശൂന്യം plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	എങ്കിൽ (plmt.hart_എണ്ണം <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#എങ്കിൽ __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#അല്ലെങ്കിൽ
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (ശൂന്യം *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ plmt_warm_timer_init(ശൂന്യം)
{
	എങ്കിൽ (!plmt.time_val)
		തിരികെ SBI_ENODEV;

	plmt_timer_event_stop();

	തിരികെ 0;
}

സ്ഥിതം ഘടന sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_മൂല്യം	   = plmt_timer_മൂല്യം,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

പൂർണ്ണസംഖ്യ plmt_cold_timer_init(ഘടന plmt_data *plmt)
{
	പൂർണ്ണസംഖ്യ rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_വേര്_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	എങ്കിൽ (rc)
		തിരികെ rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	തിരികെ 0;
}
