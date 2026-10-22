/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SpacemiT
 * Authors:
 *   Xianbin Zhu <xianbin.zhu@linux.spacemit.com>
 *   Troy Mitchell <troy.mitchell@linux.spacemit.com>
 */

#அந்தர்பாவ <platform_override.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_hsm.h>
#அந்தர்பாவ <spacemit/k1.h>

நிலையான மாறிலி u64 cpu_wakeup_reg[] = {
	PMU_AP_CORE0_WAKEUP,
	PMU_AP_CORE1_WAKEUP,
	PMU_AP_CORE2_WAKEUP,
	PMU_AP_CORE3_WAKEUP,
	PMU_AP_CORE4_WAKEUP,
	PMU_AP_CORE5_WAKEUP,
	PMU_AP_CORE6_WAKEUP,
	PMU_AP_CORE7_WAKEUP,
};

நிலையான மாறிலி u64 cpu_idle_reg[] = {
	PMU_AP_CORE0_IDLE_CFG,
	PMU_AP_CORE1_IDLE_CFG,
	PMU_AP_CORE2_IDLE_CFG,
	PMU_AP_CORE3_IDLE_CFG,
	PMU_AP_CORE4_IDLE_CFG,
	PMU_AP_CORE5_IDLE_CFG,
	PMU_AP_CORE6_IDLE_CFG,
	PMU_AP_CORE7_IDLE_CFG,
};

நிலையான உள்வரி வெறுமை spacemit_set_cpu_power(u32 hartid, bool enable)
{
	unsigned முழு மதிப்பு;
	unsigned முழு *cpu_idle_base = (unsigned முழு *)(unsigned long)cpu_idle_reg[hartid];

	மதிப்பு = readl(cpu_idle_base);

	எனில் (enable)
		மதிப்பு &= ~PMU_AP_IDLE_PWRDOWN_MASK;
	இல்லையெனில்
		மதிப்பு |= PMU_AP_IDLE_PWRDOWN_MASK;

	writel(மதிப்பு, cpu_idle_base);
}

நிலையான வெறுமை spacemit_wakeup_cpu(u32 mpidr)
{
	unsigned முழு *cpu_reset_base;
	unsigned முழு cur_hartid = current_hartid();

	cpu_reset_base = (unsigned முழு *)(unsigned long)cpu_wakeup_reg[cur_hartid];

	writel(1 << mpidr, cpu_reset_base);
}

நிலையான வெறுமை spacemit_assert_cpu(வெறுமை)
{
	spacemit_set_cpu_power(current_hartid(), false);
}

நிலையான வெறுமை spacemit_deassert_cpu(unsigned முழு hartid)
{
	spacemit_set_cpu_power(hartid, true);
}

/* Start (or power-up) the given hart */
நிலையான முழு spacemit_hart_start(unsigned முழு hartid, unsigned long saddr)
{
	spacemit_deassert_cpu(hartid);
	spacemit_wakeup_cpu(hartid);

	திரும்பு 0;
}

/*
 * Stop (or power-down) the current hart from running. This call
 * doesn't expect to return if success.
 */
நிலையான முழு spacemit_hart_stop(வெறுமை)
{
	csr_write(CSR_STIMECMP, GENMASK_ULL(63, 0));
	csr_clear(CSR_MIE, MIP_SSIP | MIP_MSIP | MIP_STIP | MIP_MTIP | MIP_SEIP | MIP_MEIP);

	/* disable data preftch */
	csr_clear(CSR_MSETUP, MSETUP_PFE);
	asm நிலையற்ற ("fence iorw, iorw");

	/* flush local dcache */
	csr_write(CSR_MRAOP, MRAOP_ICACHE_INVALID);
	asm நிலையற்ற ("fence iorw, iorw");

	/* disable dcache */
	csr_clear(CSR_MSETUP, MSETUP_DE);
	asm நிலையற்ற ("fence iorw, iorw");

	/*
	 * Core4-7 do not have dedicated bits in ML2SETUP;
	 * instead, they reuse the same bits as core0-3.
	 *
	 * Thereforspacemit_deassert_cpue, use modulo with PLATFORM_MAX_CPUS_PER_CLUSTER
	 * to select the proper bit.
	 */
	csr_clear(CSR_ML2SETUP, 1 << (current_hartid() % PLATFORM_MAX_CPUS_PER_CLUSTER));
	asm நிலையற்ற ("fence iorw, iorw");

	spacemit_assert_cpu();

	wfi();

	திரும்பு SBI_ENOTSUPP;
}

நிலையான மாறிலி கட்டமைப்பு sbi_hsm_device spacemit_hsm_ops = {
	.name		= "spacemit-hsm",
	.hart_start	= spacemit_hart_start,
	.hart_stop	= spacemit_hart_stop,
};

நிலையான முழு spacemit_hsm_probe(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	sbi_hsm_set_device(&spacemit_hsm_ops);

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு fdt_match spacemit_hsm_match[] = {
	{ .compatible = "spacemit,k1" },
	{ },
};

மாறிலி கட்டமைப்பு fdt_driver fdt_hsm_spacemit = {
	.match_table = spacemit_hsm_match,
	.init = spacemit_hsm_probe,
};
