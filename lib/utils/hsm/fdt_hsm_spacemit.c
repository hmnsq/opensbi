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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ spacemit_ಗಣ_cpu_power(u32 hartid, bool enable)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *cpu_idle_ತಳ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)cpu_idle_reg[hartid];

	ಮೌಲ್ಯ = readl(cpu_idle_ತಳ);

	ಆದರೆ (enable)
		ಮೌಲ್ಯ &= ~PMU_AP_IDLE_PWRDOWN_ಮುಖವಾಡ;
	ಇಲ್ಲದಿದ್ದರೆ
		ಮೌಲ್ಯ |= PMU_AP_IDLE_PWRDOWN_ಮುಖವಾಡ;

	writel(ಮೌಲ್ಯ, cpu_idle_ತಳ);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_wakeup_cpu(u32 mpidr)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *cpu_reset_ತಳ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_hartid = current_hartid();

	cpu_reset_ತಳ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *)(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_ತಳ);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_assert_cpu(ಶೂನ್ಯ)
{
	spacemit_ಗಣ_cpu_power(current_hartid(), false);
}

ಸ್ಥಿರ ಶೂನ್ಯ spacemit_deassert_cpu(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hartid)
{
	spacemit_ಗಣ_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_hart_ಆರಂಭ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hartid, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_hart_stop(ಶೂನ್ಯ)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm ಅಸ್ಥಿರ ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm ಅಸ್ಥಿರ ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm ಅಸ್ಥಿರ ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2_ಸಿದ್ಧತೆ, 1 << (current_hartid() % PLATFORM_MAX_CPUS_ಪ್ರತಿ_CLUSTER));
	asm ಅಸ್ಥಿರ ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	ಹಿಂತಿರುಗಿ SBI_ENOTSUPP;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_ಆರಂಭ	= spacemit_hart_ಆರಂಭ,
	.hart_stop	= spacemit_hart_stop,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spacemit_hsm_probe(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoff, ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ *ಹೊಂದಿಕೆ)
{
	sbi_hsm_ಗಣ_device(&spacemit_hsm_ops);

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಹೊಂದಿಕೆ spacemit_hsm_ಹೊಂದಿಕೆ[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_driver fdt_hsm_spacemit = {
	.ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = spacemit_hsm_ಹೊಂದಿಕೆ,
	.init = spacemit_hsm_probe,
};
