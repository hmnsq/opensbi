/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#અન્તર્ભાવ <platform_override.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <spacemit/k1.h>

સ્થિર અચલ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

સ્થિર અચલ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

સ્થિર અંતઃરેખ શૂન્ય spacemit_set_cpu_power(u32 hartid, bool enable)
{
	unsigned પૂર્ણાંક કિંમત;
	unsigned પૂર્ણાંક *cpu_idle_base = (unsigned પૂર્ણાંક *)(unsigned long)cpu_idle_reg[hartid];

	કિંમત = readl(cpu_idle_base);

	જો (enable)
		કિંમત &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	નહીંતર
		કિંમત |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(કિંમત, cpu_idle_base);
}

સ્થિર શૂન્ય spacemit_wakeup_cpu(u32 mpidr)
{
	unsigned પૂર્ણાંક *cpu_reset_base;
	unsigned પૂર્ણાંક cur_hartid = current_hartid();

	cpu_reset_base = (unsigned પૂર્ણાંક *)(unsigned long)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

સ્થિર શૂન્ય spacemit_assert_cpu(શૂન્ય)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

સ્થિર શૂન્ય spacemit_deassert_cpu(unsigned પૂર્ણાંક hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
સ્થિર પૂર્ણાંક spacemit_hart_start(unsigned પૂર્ણાંક hartid, unsigned long saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	પાછા 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
સ્થિર પૂર્ણાંક spacemit_hart_stop(શૂન્ય)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm અસ્થિર ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm અસ્થિર ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm અસ્થિર ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	asm અસ્થિર ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	પાછા SBI_ENOTSUPP;
}

સ્થિર અચલ સંરચના sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

સ્થિર પૂર્ણાંક spacemit_hsm_probe(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

અચલ સંરચના fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
