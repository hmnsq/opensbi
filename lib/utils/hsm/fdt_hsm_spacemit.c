/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#ಅನ್ತರ್ಭಾವ <platform_override.h>
#ಅನ್ತರ್ಭಾವ <sbi/riscv_io.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_hsm.h>
#ಅನ್ತರ್ಭಾವ <spacemit/k1.h>

ಸ್ಥಿರ ಧ್ರುವ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ಸ್ಥಿರ ಧ್ರುವ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ spacemit_set_cpu_power(u32 hartid, bool enable)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;
	unsigned ಪೂರ್ಣಾಙ್ಕ *cpu_idle_base = (unsigned ಪೂರ್ಣಾಙ್ಕ *)(unsigned long)cpu_idle_reg[hartid];

	ಮೌಲ್ಯ = readl(cpu_idle_base);

	ಯದಿ (enable)
		ಮೌಲ್ಯ &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	ಅನ್ಯಥಾ
		ಮೌಲ್ಯ |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(ಮೌಲ್ಯ, cpu_idle_base);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_wakeup_cpu(u32 mpidr)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ *cpu_reset_base;
	unsigned ಪೂರ್ಣಾಙ್ಕ cur_hartid = current_hartid();

	cpu_reset_base = (unsigned ಪೂರ್ಣಾಙ್ಕ *)(unsigned long)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_assert_cpu(ಶೂನ್ಯ)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_deassert_cpu(unsigned ಪೂರ್ಣಾಙ್ಕ hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_hart_start(unsigned ಪೂರ್ಣಾಙ್ಕ hartid, unsigned long saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_hart_stop(ಶೂನ್ಯ)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm ಚಞ್ಚಲ ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm ಚಞ್ಚಲ ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm ಚಞ್ಚಲ ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	asm ಚಞ್ಚಲ ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	ಪ್ರತ್ಯಾಗಮ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ spacemit_hsm_probe(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoff, ಧ್ರುವ ಸಂರಚನಾ fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ಧ್ರುವ ಸಂರಚನಾ fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
