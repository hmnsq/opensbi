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

स्थिर u64 plmt_timer_मूल्य(शून्य)
{
#जर __riscv_xlen == 64
	परत readq_relaxed(plmt.time_val);
#अन्यथा
	u32 lo, hi;

	करा {
		hi = readl_relaxed((शून्य *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} पर्यंत (hi != readl_relaxed((शून्य *)plmt.time_val + 0x04));

	परत ((u64)hi << 32) | (u64)lo;
#endif
}

स्थिर शून्य plmt_timer_event_stop(शून्य)
{
	u32 target_hart = current_hartid();

	जर (plmt.hart_मोजणी <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#जर __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#अन्यथा
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (शून्य *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

स्थिर शून्य plmt_timer_event_start(u64 next_event)
{
	u32 target_hart = current_hartid();

	जर (plmt.hart_मोजणी <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#जर __riscv_xlen == 64
	writeq_relaxed(next_event, &plmt.time_cmp[target_hart]);
#अन्यथा
	u32 mask = -1UL;

	writel_relaxed(next_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(next_event >> 32,
		       (शून्य *)(&plmt.time_cmp[target_hart]) + 0x04);
#endif
}

स्थिर पूर्णांक plmt_warm_timer_init(शून्य)
{
	जर (!plmt.time_val)
		परत SBI_ENODEV;

	plmt_timer_event_stop();

	परत 0;
}

स्थिर संरचना sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_मूल्य	   = plmt_timer_मूल्य,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

पूर्णांक plmt_cold_timer_init(संरचना plmt_data *plmt)
{
	पूर्णांक rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_मूळ_add_memrange(
		(अचिन्हित दीर्घ)plmt->time_val, plmt->size,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	जर (rc)
		परत rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	परत 0;
}
