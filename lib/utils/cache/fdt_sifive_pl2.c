/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#అన్తర్భావ <libfdt.h>
#అన్తర్భావ <sbi/riscv_io.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_heap.h>
#అన్తర్భావ <sbi_utils/cache/fdt_cache.h>
#అన్తర్భావ <sbi_utils/fdt/fdt_driver.h>

#నిర్వచించు FLUSH64_CMD_లక్ష్యం_ALL		(0x2 << 3)
#నిర్వచించు FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#నిర్వచించు SIFIVE_PL2CACHE_CMD_QLEN	0xff

#నిర్వచించు SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#నిర్వచించు SIFIVE_PL2_CACHE_స్థితి_OFF	0x208ULL
#నిర్వచించు SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#నిర్వచించు SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#నిర్వచించు FLUSH64_CMD_POS			56
#నిర్వచించు REGIONCLOCKDISABLE_ముసుగు		బిట్టు(3)

#నిర్వచించు CONFIG0_ACCEPT_DIRTY_దత్తాంశం_ENABLE	బిట్టు(24)

నిర్మాణం sifive_pl2_quirks {
	bool no_dirty_fill;
};

నిర్మాణం sifive_pl2 {
	నిర్మాణం cache_device dev;
	శూన్య *addr;
	bool no_dirty_fill;
};

#నిర్వచించు గమ్యం_pl2(_dev) container_యొక్క(_dev, నిర్మాణం sifive_pl2, dev)

స్థిర పూర్ణాంకం sifive_pl2_flush_all(నిర్మాణం cache_device *dev)
{
	నిర్మాణం sifive_pl2 *pl2_dev = గమ్యం_pl2(dev);
	అక్షరం *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_లక్ష్యం_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ఒకవేళ (pl2_dev->no_dirty_fill) {
		config0 = readl((శూన్య *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_దత్తాంశం_ENABLE;
		writel(config0, (శూన్య *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ఒకవేళ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#లేకపోతే
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + పరిమాణం(u32));
#అయితే_ముగింపు
	చేయి {} అయితే (readl(addr + SIFIVE_PL2_CACHE_స్థితి_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	తిరిగి 0;
}

స్థిర పూర్ణాంకం sifive_pl2_warm_init(నిర్మాణం cache_device *dev)
{
	నిర్మాణం sifive_pl2 *pl2_dev = గమ్యం_pl2(dev);
	అక్షరం *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_ముసుగు);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	తిరిగి 0;
}

స్థిర నిర్మాణం cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

స్థిర పూర్ణాంకం sifive_pl2_cold_init(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoff, స్థిరరాశి నిర్మాణం fdt_సరిపోలిక *సరిపోలిక)
{
	స్థిరరాశి నిర్మాణం sifive_pl2_quirks *quirk = సరిపోలిక->దత్తాంశం;
	నిర్మాణం sifive_pl2 *pl2_dev;
	నిర్మాణం cache_device *dev;
	u64 reg_addr;
	పూర్ణాంకం rc;

	/* find the pl2 control base address */
	rc = fdt_get_కణుపు_addr_కొలత(fdt, nodeoff, 0, &reg_addr, ఖాళీ);
	ఒకవేళ (rc < 0 && reg_addr)
		తిరిగి SBI_ENODEV;

	pl2_dev = sbi_zalloc(పరిమాణం(*pl2_dev));
	ఒకవేళ (!pl2_dev)
		తిరిగి SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_వ్యక్తిగత = true;

	rc = fdt_cache_కూడు(fdt, nodeoff, dev);
	ఒకవేళ (rc)
		తిరిగి rc;

	pl2_dev->addr = (శూన్య *)(uintptr_t)reg_addr;
	ఒకవేళ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	తిరిగి 0;
}

స్థిర స్థిరరాశి నిర్మాణం sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

స్థిర స్థిరరాశి నిర్మాణం sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

స్థిర స్థిరరాశి నిర్మాణం fdt_సరిపోలిక sifive_pl2_సరిపోలిక[] = {
	{ .compatible = "sifive,pl2cache2", .దత్తాంశం = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .దత్తాంశం = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .దత్తాంశం = &pl2cache0_quirks },
	{},
};

నిర్మాణం fdt_driver fdt_sifive_pl2 = {
	.సరిపోలిక_పట్టిక = sifive_pl2_సరిపోలిక,
	.init = sifive_pl2_cold_init,
};
