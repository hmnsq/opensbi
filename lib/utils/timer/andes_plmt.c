/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_timer.h>
#شامل <sbi_utils/timer/andes_plmt.h>

ساخت plmt_ڈیٹا plmt;

ساکن u64 plmt_timer_قدر(خالی)
{
#اگر __riscv_xlen == 64
	واپس readq_relaxed(plmt.وقت_val);
#ورنہ
	u32 lo, hi;

	کرو {
		hi = readl_relaxed((خالی *)plmt.وقت_val + 0x04);
		lo = readl_relaxed(plmt.وقت_val);
	} جب (hi != readl_relaxed((خالی *)plmt.وقت_val + 0x04));

	واپس ((u64)hi << 32) | (u64)lo;
#اگر_ختم
}

ساکن خالی plmt_timer_event_stop(خالی)
{
	u32 ہدف_hart = current_hartid();

	اگر (plmt.hart_گنتی <= ہدف_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#اگر __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.وقت_cmp[ہدف_hart]);
#ورنہ
	writel_relaxed(-1UL, &plmt.وقت_cmp[ہدف_hart]);
	writel_relaxed(-1UL, (خالی *)(&plmt.وقت_cmp[ہدف_hart]) + 0x04);
#اگر_ختم
}

ساکن خالی plmt_timer_event_آغاز(u64 اگلا_event)
{
	u32 ہدف_hart = current_hartid();

	اگر (plmt.hart_گنتی <= ہدف_hart)
		ebreak();

	/* Program PLMT Time Compare */
#اگر __riscv_xlen == 64
	writeq_relaxed(اگلا_event, &plmt.وقت_cmp[ہدف_hart]);
#ورنہ
	u32 نقاب = -1UL;

	writel_relaxed(اگلا_event & نقاب, &plmt.وقت_cmp[ہدف_hart]);
	writel_relaxed(اگلا_event >> 32,
		       (خالی *)(&plmt.وقت_cmp[ہدف_hart]) + 0x04);
#اگر_ختم
}

ساکن صحیح plmt_warm_timer_init(خالی)
{
	اگر (!plmt.وقت_val)
		واپس SBI_ENODEV;

	plmt_timer_event_stop();

	واپس 0;
}

ساکن ساخت sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_قدر	   = plmt_timer_قدر,
	.timer_event_آغاز = plmt_timer_event_آغاز,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

صحیح plmt_cold_timer_init(ساخت plmt_ڈیٹا *plmt)
{
	صحیح rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_جڑ_جوڑو_memrange(
		(غیرعلامتی طویل)plmt->وقت_val, plmt->سائز,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	اگر (rc)
		واپس rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_سیٹ_device(&plmt_timer);

	واپس 0;
}
