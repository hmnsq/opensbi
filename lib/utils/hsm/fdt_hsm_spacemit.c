/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#समावेश <platform_override.h>
#समावेश <sbi/riscv_io.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <spacemit/k1.h>

अटल स्थिर u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

अटल स्थिर u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

अटल अंतरभूत व्योम spacemit_set_cpu_power(u32 hartid, bool enable)
{
	अचिन्हित पूर्णांक मूल्य;
	अचिन्हित पूर्णांक *cpu_idle_base = (अचिन्हित पूर्णांक *)(अचिन्हित दीर्घ)cpu_idle_reg[hartid];

	मूल्य = readl(cpu_idle_base);

	अगर (enable)
		मूल्य &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	अन्यथा
		मूल्य |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(मूल्य, cpu_idle_base);
}

अटल व्योम spacemit_wakeup_cpu(u32 mpidr)
{
	अचिन्हित पूर्णांक *cpu_reset_base;
	अचिन्हित पूर्णांक cur_hartid = current_hartid();

	cpu_reset_base = (अचिन्हित पूर्णांक *)(अचिन्हित दीर्घ)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

अटल व्योम spacemit_assert_cpu(व्योम)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

अटल व्योम spacemit_deassert_cpu(अचिन्हित पूर्णांक hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
अटल पूर्णांक spacemit_hart_start(अचिन्हित पूर्णांक hartid, अचिन्हित दीर्घ saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	वापस 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
अटल पूर्णांक spacemit_hart_stop(व्योम)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	यंत्र अस्थिर ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	यंत्र अस्थिर ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	यंत्र अस्थिर ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	यंत्र अस्थिर ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	वापस SBI_ENOTSUPP;
}

अटल स्थिर काष्ठा sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

अटल पूर्णांक spacemit_hsm_probe(स्थिर व्योम *fdt, पूर्णांक nodeoff, स्थिर काष्ठा fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	वापस 0;
}

अटल स्थिर काष्ठा fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

स्थिर काष्ठा fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
