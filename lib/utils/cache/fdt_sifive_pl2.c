/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#অন্তর্ভাব <libfdt.h>
#অন্তর্ভাব <sbi/riscv_io.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi_utils/cache/fdt_cache.h>
#অন্তর্ভাব <sbi_utils/fdt/fdt_driver.h>

#define FLUSH64_CMD_TARGET_ALL		(0x2 << 3)
#define FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#define SIFIVE_PL2CACHE_CMD_QLEN	0xff

#define SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#define SIFIVE_PL2CACHE_STATUS_OFF	0x208ULL
#define SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#define SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#define FLUSH64_CMD_POS			56
#define REGIONCLOCKDISABLE_MASK		BIT(3)

#define CONFIG0_ACCEPT_DIRTY_তথ্য_ENABLE	BIT(24)

কাঠামো sifive_pl2_quirks {
	bool no_dirty_fill;
};

কাঠামো sifive_pl2 {
	কাঠামো cache_device dev;
	শূন্য *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, কাঠামো sifive_pl2, dev)

স্থির পূর্ণসংখ্যা sifive_pl2_flush_all(কাঠামো cache_device *dev)
{
	কাঠামো sifive_pl2 *pl2_dev = to_pl2(dev);
	অক্ষর *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	যদি (pl2_dev->no_dirty_fill) {
		config0 = readl((শূন্য *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_তথ্য_ENABLE;
		writel(config0, (শূন্য *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#যদি __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#অন্যথা
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + আকার(u32));
#endif
	করো {} যখন (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা sifive_pl2_warm_init(কাঠামো cache_device *dev)
{
	কাঠামো sifive_pl2 *pl2_dev = to_pl2(dev);
	অক্ষর *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ফেরত 0;
}

স্থির কাঠামো cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

স্থির পূর্ণসংখ্যা sifive_pl2_cold_init(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoff, ধ্রুবক কাঠামো fdt_match *match)
{
	ধ্রুবক কাঠামো sifive_pl2_quirks *quirk = match->তথ্য;
	কাঠামো sifive_pl2 *pl2_dev;
	কাঠামো cache_device *dev;
	u64 reg_addr;
	পূর্ণসংখ্যা rc;

	/* find the pl2 control base address */
	rc = fdt_get_গ্রন্থি_addr_আকার(fdt, nodeoff, 0, &reg_addr, ফাঁকা);
	যদি (rc < 0 && reg_addr)
		ফেরত SBI_ENODEV;

	pl2_dev = sbi_zalloc(আকার(*pl2_dev));
	যদি (!pl2_dev)
		ফেরত SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	যদি (rc)
		ফেরত rc;

	pl2_dev->addr = (শূন্য *)(uintptr_t)reg_addr;
	যদি (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

স্থির ধ্রুবক কাঠামো sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

স্থির ধ্রুবক কাঠামো fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .তথ্য = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .তথ্য = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .তথ্য = &pl2cache0_quirks },
	{},
};

কাঠামো fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
