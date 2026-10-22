/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <platform_override.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_hsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <spacemit/k1.h>

ਸਥਿਰ ਅਚਲ u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ਸਥਿਰ ਅਚਲ u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ spacemit_ਸੈੱਟ_cpu_power(u32 hartid, bool enable)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮੁੱਲ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *cpu_idle_ਅਧਾਰ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)cpu_idle_reg[hartid];

	ਮੁੱਲ = readl(cpu_idle_ਅਧਾਰ);

	ਜੇ (enable)
		ਮੁੱਲ &= ~PMU_AP_IDLE_PWRDOWN_ਨਕਾਬ;
	ਨਹੀਂਤਾਂ
		ਮੁੱਲ |= PMU_AP_IDLE_PWRDOWN_ਨਕਾਬ;

	writel(ਮੁੱਲ, cpu_idle_ਅਧਾਰ);
}

ਸਥਿਰ ਖਾਲੀ spacemit_wakeup_cpu(u32 mpidr)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *cpu_reset_ਅਧਾਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_hartid = current_hartid();

	cpu_reset_ਅਧਾਰ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_ਅਧਾਰ);
}

ਸਥਿਰ ਖਾਲੀ spacemit_assert_cpu(ਖਾਲੀ)
{
	spacemit_ਸੈੱਟ_cpu_power(current_hartid(), false);
}

ਸਥਿਰ ਖਾਲੀ spacemit_deassert_cpu(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hartid)
{
	spacemit_ਸੈੱਟ_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ਸਥਿਰ ਪੂਰਨ spacemit_hart_ਸ਼ੁਰੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hartid, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	ਵਾਪਸ 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ਸਥਿਰ ਪੂਰਨ spacemit_hart_stop(ਖਾਲੀ)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm ਅਸਥਿਰ ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm ਅਸਥਿਰ ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm ਅਸਥਿਰ ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2_ਤਿਆਰੀ, 1 << (current_hartid() % PLATFORM_MAX_CPUS_ਪ੍ਰਤੀ_CLUSTER));
	asm ਅਸਥਿਰ ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	ਵਾਪਸ SBI_ENOTSUPP;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_ਸ਼ੁਰੂ	= spacemit_hart_ਸ਼ੁਰੂ,
	.hart_stop	= spacemit_hart_stop,
};

ਸਥਿਰ ਪੂਰਨ spacemit_hsm_probe(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ nodeoff, ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ *ਮੇਲ)
{
	sbi_hsm_ਸੈੱਟ_device(&spacemit_hsm_ops);

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fdt_ਮੇਲ spacemit_hsm_ਮੇਲ[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ਅਚਲ ਬਣਤਰ fdt_driver fdt_hsm_spacemit = {
	.ਮੇਲ_ਸਾਰਣੀ = spacemit_hsm_ਮੇਲ,
	.init = spacemit_hsm_probe,
};
