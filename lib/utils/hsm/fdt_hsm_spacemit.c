/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#ضمن <platform_override.h>
#ضمن <sbi/riscv_io.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <spacemit/k1.h>

ساكن ثابت u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ساكن ثابت u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ساكن مضمن فراغ spacemit_سيٽ_cpu_power(u32 hartid, bool enable)
{
	بنانشان صحيح قدر;
	بنانشان صحيح *cpu_idle_بنياد = (بنانشان صحيح *)(بنانشان ڊگهو)cpu_idle_reg[hartid];

	قدر = readl(cpu_idle_بنياد);

	إذا (enable)
		قدر &= ~PMU_AP_IDLE_PWRDOWN_نقاب;
	وإلا
		قدر |= PMU_AP_IDLE_PWRDOWN_نقاب;

	writel(قدر, cpu_idle_بنياد);
}

ساكن فراغ spacemit_wakeup_cpu(u32 mpidr)
{
	بنانشان صحيح *cpu_reset_بنياد;
	بنانشان صحيح cur_hartid = current_hartid();

	cpu_reset_بنياد = (بنانشان صحيح *)(بنانشان ڊگهو)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_بنياد);
}

ساكن فراغ spacemit_assert_cpu(فراغ)
{
	spacemit_سيٽ_cpu_power(current_hartid(), false);
}

ساكن فراغ spacemit_deassert_cpu(بنانشان صحيح hartid)
{
	spacemit_سيٽ_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ساكن صحيح spacemit_hart_شروعات(بنانشان صحيح hartid, بنانشان ڊگهو saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	أرجع 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ساكن صحيح spacemit_hart_stop(فراغ)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm متطاير ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm متطاير ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm متطاير ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2_تياري, 1 << (current_hartid() % PLATFORM_MAX_CPUS_في_CLUSTER));
	asm متطاير ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	أرجع SBI_ENOTSUPP;
}

ساكن ثابت بنية sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_شروعات	= spacemit_hart_شروعات,
	.hart_stop	= spacemit_hart_stop,
};

ساكن صحيح spacemit_hsm_probe(ثابت فراغ *fdt, صحيح nodeoff, ثابت بنية fdt_ميلاپ *ميلاپ)
{
	sbi_hsm_سيٽ_device(&spacemit_hsm_ops);

	أرجع 0;
}

ساكن ثابت بنية fdt_ميلاپ spacemit_hsm_ميلاپ[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ثابت بنية fdt_driver fdt_hsm_spacemit = {
	.ميلاپ_جدول = spacemit_hsm_ميلاپ,
	.init = spacemit_hsm_probe,
};
