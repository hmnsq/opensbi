/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#ଅନ୍ତର୍ଭାଵ <platform_override.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_hsm.h>
#ଅନ୍ତର୍ଭାଵ <spacemit/k1.h>

ସ୍ଥିର ଅଚଳ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ସ୍ଥିର ଅଚଳ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ spacemit_set_cpu_power(u32 hartid, bool enable)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpu_idle_base = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(unsigned long)cpu_idle_reg[hartid];

	ମୂଲ୍ୟ = readl(cpu_idle_base);

	ଯଦି (enable)
		ମୂଲ୍ୟ &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	ଅନ୍ୟଥା
		ମୂଲ୍ୟ |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(ମୂଲ୍ୟ, cpu_idle_base);
}

ସ୍ଥିର ଶୂନ୍ୟ spacemit_wakeup_cpu(u32 mpidr)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cpu_reset_base;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_hartid = current_hartid();

	cpu_reset_base = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)(unsigned long)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

ସ୍ଥିର ଶୂନ୍ୟ spacemit_assert_cpu(ଶୂନ୍ୟ)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

ସ୍ଥିର ଶୂନ୍ୟ spacemit_deassert_cpu(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_hart_start(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hartid, unsigned long saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	ଫେରନ୍ତୁ 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_hart_stop(ଶୂନ୍ୟ)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm ଅସ୍ଥିର ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm ଅସ୍ଥିର ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm ଅସ୍ଥିର ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	asm ଅସ୍ଥିର ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	ଫେରନ୍ତୁ SBI_ENOTSUPP;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା spacemit_hsm_probe(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ଅଚଳ ସଂରଚନା fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
