/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#अन्तर्भाव <sbi/riscv_asm.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_domain.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_timer.h>
#अन्तर्भाव <sbi_utils/timer/andes_plmt.h>

संरचना plmt_data plmt;

स्थिर u64 plmt_timer_मानम्(शून्य)
{
#सचे __riscv_xlen == 64
	पटिनिवत्त readq_relaxed(plmt.time_val);
#अञ्ञथा
	u32 lo, hi;

	करोहि {
		hi = readl_relaxed((शून्य *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} याव (hi != readl_relaxed((शून्य *)plmt.time_val + 0x04));

	पटिनिवत्त ((u64)hi << 32) | (u64)lo;
#endif
}

स्थिर शून्य plmt_timer_event_stop(शून्य)
{
	u32 target_hart = current_hartid();

	सचे (plmt.hart_गणनम् <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#सचे __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#अञ्ञथा
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (शून्य *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

स्थिर शून्य plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	सचे (plmt.hart_गणनम् <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#सचे __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#अञ्ञथा
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (शून्य *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

स्थिर पूर्णाङ्क plmt_warm_timer_init(शून्य)
{
	सचे (!plmt.time_val)
		पटिनिवत्त SBI_ENODEV;

	plmt_timer_event_stop();

	पटिनिवत्त 0;
}

स्थिर संरचना sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_मानम्	   = plmt_timer_मानम्,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

पूर्णाङ्क plmt_cold_timer_init(संरचना plmt_data *plmt)
{
	पूर्णाङ्क rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_मूलम्_add_memrange(
		(unsigned long)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	सचे (rc)
		पटिनिवत्त rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	पटिनिवत्त 0;
}
