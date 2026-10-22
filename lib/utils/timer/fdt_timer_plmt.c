/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Andes Technology Corporation
 *
 * Authors:
 *   Yu Chien Peter Lin <peterlin@andestech.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/fdt/fdt_helper.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/fdt_timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/timer/andes_plmt.h>

ਬਾਹਰੀ ਬਣਤਰ plmt_ਅੰਕੜੇ plmt;

ਸਥਿਰ ਪੂਰਨ fdt_plmt_cold_timer_init(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff,
				    ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	ਪੂਰਨ rc;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ plmt_ਅਧਾਰ;

	rc = fdt_parse_plmt_ਗੰਢ(fdt, nodeoff, &plmt_ਅਧਾਰ, &plmt.ਮਾਪ,
				 &plmt.hart_ਗਿਣਤੀ);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	plmt.ਸਮਾਂ_val = (u64 *)plmt_ਅਧਾਰ;
	plmt.ਸਮਾਂ_cmp = (u64 *)(plmt_ਅਧਾਰ + 0x8);

	rc = fdt_parse_timebase_frequency(fdt, &plmt.timer_freq);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	rc = plmt_cold_timer_init(&plmt);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ timer_plmt_ਮੇਲ[] = {
	{ .compatible = "andestech,plmt0" },
	{},
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_timer_plmt = {
	.ਮੇਲ_ਸਾਰਣੀ = timer_plmt_ਮੇਲ,
	.init = fdt_plmt_cold_timer_init,
};
