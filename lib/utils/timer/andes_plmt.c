/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_timer.h>
#समावेश <sbi_utils/timer/andes_plmt.h>

काष्ठा plmt_तथ्य plmt;

अटल u64 plmt_timer_मूल्य(व्योम)
{
#अगर __riscv_xlen == 64
	वापस readq_relaxed(plmt.time_val);
#अन्यथा
	u32 lo, hi;

	करो {
		hi = readl_relaxed((व्योम *)plmt.time_val + 0x04);
		lo = readl_relaxed(plmt.time_val);
	} जबतक (hi != readl_relaxed((व्योम *)plmt.time_val + 0x04));

	वापस ((u64)hi << 32) | (u64)lo;
#पूर्ण_अगर
}

अटल व्योम plmt_timer_event_stop(व्योम)
{
	u32 target_hart = current_hartid();

	अगर (plmt.hart_गिनती <= target_hart)
		ebreak();

	/* Clear PLMT Time Compare */
#अगर __riscv_xlen == 64
	writeq_relaxed(-1ULL, &plmt.time_cmp[target_hart]);
#अन्यथा
	writel_relaxed(-1UL, &plmt.time_cmp[target_hart]);
	writel_relaxed(-1UL, (व्योम *)(&plmt.time_cmp[target_hart]) + 0x04);
#पूर्ण_अगर
}

अटल व्योम plmt_timer_event_start(u64 अग्र_event)
{
	u32 target_hart = current_hartid();

	अगर (plmt.hart_गिनती <= target_hart)
		ebreak();

	/* Program PLMT Time Compare */
#अगर __riscv_xlen == 64
	writeq_relaxed(अग्र_event, &plmt.time_cmp[target_hart]);
#अन्यथा
	u32 mask = -1UL;

	writel_relaxed(अग्र_event & mask, &plmt.time_cmp[target_hart]);
	writel_relaxed(अग्र_event >> 32,
		       (व्योम *)(&plmt.time_cmp[target_hart]) + 0x04);
#पूर्ण_अगर
}

अटल पूर्णांक plmt_warm_timer_init(व्योम)
{
	अगर (!plmt.time_val)
		वापस SBI_ENODEV;

	plmt_timer_event_stop();

	वापस 0;
}

अटल काष्ठा sbi_timer_device plmt_timer = {
	.name		   = "andes_plmt",
	.timer_freq	   = DEFAULT_AE350_PLMT_FREQ,
	.timer_मूल्य	   = plmt_timer_मूल्य,
	.timer_event_start = plmt_timer_event_start,
	.timer_event_stop  = plmt_timer_event_stop,
	.warm_init	   = plmt_warm_timer_init,
};

पूर्णांक plmt_cold_timer_init(काष्ठा plmt_तथ्य *plmt)
{
	पूर्णांक rc;

	/* Add PLMT region to the root domain */
	rc = sbi_domain_मूल_add_memrange(
		(अचिन्हित दीर्घ)plmt->time_val, plmt->आकार,
		PLMT_REGION_ALIGN,
		SBI_DOMAIN_MEMREGION_MMIO |
		SBI_DOMAIN_MEMREGION_M_READABLE |
		SBI_DOMAIN_MEMREGION_M_WRITABLE);
	अगर (rc)
		वापस rc;

	plmt_timer.timer_freq = plmt->timer_freq;

	sbi_timer_set_device(&plmt_timer);

	वापस 0;
}
