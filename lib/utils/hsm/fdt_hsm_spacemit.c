/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#অন্তর্ভাৱ <platform_override.h>
#অন্তর্ভাৱ <sbi/riscv_io.h>
#অন্তর্ভাৱ <sbi/sbi_hsm.h>
#অন্তর্ভাৱ <spacemit/k1.h>

স্থির ধ্রুৱ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

স্থির ধ্রুৱ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

স্থির অন্তঃস্থ শূন্য spacemit_set_cpu_power(u32 hartid, bool enable)
{
	unsigned পূর্ণাঙ্ক মান;
	unsigned পূর্ণাঙ্ক *cpu_idle_base = (unsigned পূর্ণাঙ্ক *)(unsigned long)cpu_idle_reg[hartid];

	মান = readl(cpu_idle_base);

	যদি (enable)
		মান &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	অন্যথা
		মান |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(মান, cpu_idle_base);
}

স্থির শূন্য spacemit_wakeup_cpu(u32 mpidr)
{
	unsigned পূর্ণাঙ্ক *cpu_reset_base;
	unsigned পূর্ণাঙ্ক cur_hartid = current_hartid();

	cpu_reset_base = (unsigned পূর্ণাঙ্ক *)(unsigned long)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

স্থির শূন্য spacemit_assert_cpu(শূন্য)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

স্থির শূন্য spacemit_deassert_cpu(unsigned পূর্ণাঙ্ক hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
স্থির পূর্ণাঙ্ক spacemit_hart_start(unsigned পূর্ণাঙ্ক hartid, unsigned long saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	ঘূৰাই 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
স্থির পূর্ণাঙ্ক spacemit_hart_stop(শূন্য)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm চঞ্চল ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm চঞ্চল ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm চঞ্চল ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	asm চঞ্চল ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	ঘূৰাই SBI_ENOTSUPP;
}

স্থির ধ্রুৱ সংরচনা sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

স্থির পূর্ণাঙ্ক spacemit_hsm_probe(ধ্রুৱ শূন্য *fdt, পূর্ণাঙ্ক nodeoff, ধ্রুৱ সংরচনা fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	ঘূৰাই 0;
}

স্থির ধ্রুৱ সংরচনা fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ধ্রুৱ সংরচনা fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
