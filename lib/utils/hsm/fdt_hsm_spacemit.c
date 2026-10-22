/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#شامل <platform_override.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_hsm.h>
#شامل <spacemit/k1.h>

ساکن ثابت u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

ساکن ثابت u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

ساکن اندرونی خالی spacemit_سیٹ_cpu_power(u32 hartid, bool enable)
{
	غیرعلامتی صحیح قدر;
	غیرعلامتی صحیح *cpu_idle_بنیاد = (غیرعلامتی صحیح *)(غیرعلامتی طویل)cpu_idle_reg[hartid];

	قدر = readl(cpu_idle_بنیاد);

	اگر (enable)
		قدر &= ~PMU_AP_IDLE_PWRDOWN_نقاب;
	ورنہ
		قدر |= PMU_AP_IDLE_PWRDOWN_نقاب;

	writel(قدر, cpu_idle_بنیاد);
}

ساکن خالی spacemit_wakeup_cpu(u32 mpidr)
{
	غیرعلامتی صحیح *cpu_reset_بنیاد;
	غیرعلامتی صحیح cur_hartid = current_hartid();

	cpu_reset_بنیاد = (غیرعلامتی صحیح *)(غیرعلامتی طویل)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_بنیاد);
}

ساکن خالی spacemit_assert_cpu(خالی)
{
	spacemit_سیٹ_cpu_power(current_hartid(), false);
}

ساکن خالی spacemit_deassert_cpu(غیرعلامتی صحیح hartid)
{
	spacemit_سیٹ_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
ساکن صحیح spacemit_hart_آغاز(غیرعلامتی صحیح hartid, غیرعلامتی طویل saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	واپس 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
ساکن صحیح spacemit_hart_stop(خالی)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm متلون ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm متلون ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm متلون ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2_تیاری, 1 << (current_hartid() % PLATFORM_MAX_CPUS_فی_CLUSTER));
	asm متلون ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	واپس SBI_ENOTSUPP;
}

ساکن ثابت ساخت sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_آغاز	= spacemit_hart_آغاز,
	.hart_stop	= spacemit_hart_stop,
};

ساکن صحیح spacemit_hsm_probe(ثابت خالی *fdt, صحیح nodeoff, ثابت ساخت fdt_مطابقت *مطابقت)
{
	sbi_hsm_سیٹ_device(&spacemit_hsm_ops);

	واپس 0;
}

ساکن ثابت ساخت fdt_مطابقت spacemit_hsm_مطابقت[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

ثابت ساخت fdt_driver fdt_hsm_spacemit = {
	.مطابقت_جدول = spacemit_hsm_مطابقت,
	.init = spacemit_hsm_probe,
};
