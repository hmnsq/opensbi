/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#అన్తర్భావ <platform_override.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_hsm.h>
#అన్తర్భావ <spacemit/k1.h>

స్థిర స్థిరరాశి u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

స్థిర స్థిరరాశి u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

స్థిర అంతఃవరుస శూన్య spacemit_సమితి_cpu_power(u32 hartid, bool enable)
{
	గుర్తులేని పూర్ణాంకం విలువ;
	గుర్తులేని పూర్ణాంకం *cpu_idle_పునాది = (గుర్తులేని పూర్ణాంకం *)(గుర్తులేని దీర్ఘ)cpu_idle_reg[hartid];

	విలువ = readl(cpu_idle_పునాది);

	ఒకవేళ (enable)
		విలువ &= ~PMU_AP_IDLE_PWRDOWN_ముసుగు;
	లేకపోతే
		విలువ |= PMU_AP_IDLE_PWRDOWN_ముసుగు;

	writel(విలువ, cpu_idle_పునాది);
}

స్థిర శూన్య spacemit_wakeup_cpu(u32 mpidr)
{
	గుర్తులేని పూర్ణాంకం *cpu_reset_పునాది;
	గుర్తులేని పూర్ణాంకం cur_hartid = current_hartid();

	cpu_reset_పునాది = (గుర్తులేని పూర్ణాంకం *)(గుర్తులేని దీర్ఘ)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_పునాది);
}

స్థిర శూన్య spacemit_assert_cpu(శూన్య)
{
	spacemit_సమితి_cpu_power(current_hartid(), false);
}

స్థిర శూన్య spacemit_deassert_cpu(గుర్తులేని పూర్ణాంకం hartid)
{
	spacemit_సమితి_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
స్థిర పూర్ణాంకం spacemit_hart_ప్రారంభం(గుర్తులేని పూర్ణాంకం hartid, గుర్తులేని దీర్ఘ saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	తిరిగి 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
స్థిర పూర్ణాంకం spacemit_hart_stop(శూన్య)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm అస్థిర ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm అస్థిర ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm అస్థిర ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2_అమరిక, 1 << (current_hartid() % PLATFORM_MAX_CPUS_చొప్పున_CLUSTER));
	asm అస్థిర ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	తిరిగి SBI_ENOTSUPP;
}

స్థిర స్థిరరాశి నిర్మాణం sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_ప్రారంభం	= spacemit_hart_ప్రారంభం,
	.hart_stop	= spacemit_hart_stop,
};

స్థిర పూర్ణాంకం spacemit_hsm_probe(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	sbi_hsm_సమితి_device(&spacemit_hsm_ops);

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక spacemit_hsm_సరిపోలిక[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

స్థిరరాశి నిర్మాణం fdt_driver fdt_hsm_spacemit = {
	.సరిపోలిక_పట్టిక = spacemit_hsm_సరిపోలిక,
	.init = spacemit_hsm_probe,
};
