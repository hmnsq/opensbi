/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#ଅନ୍ତର୍ଭାଵ <libfdt.h>
#ଅନ୍ତର୍ଭାଵ <sbi/riscv_io.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_heap.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/cache/fdt_cache.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/fdt/fdt_driver.h>

#define FLUSH64_CMD_TARGET_ALL		(0x2 << 3)
#define FLUSH64_CMD_TYPE_FLUSH		0x3ULL

#define SIFIVE_PL2CACHE_CMD_QLEN	0xff

#define SIFIVE_PL2CACHE_FLUSH64_OFF	0x200ULL
#define SIFIVE_PL2CACHE_STATUS_OFF	0x208ULL
#define SIFIVE_PL2CACHE_CONFIG1_OFF	0x1000ULL
#define SIFIVE_PL2CACHE_CONFIG0_OFF	0x1008ULL

#define FLUSH64_CMD_POS			56
#define REGIONCLOCKDISABLE_MASK		BIT(3)

#define CONFIG0_ACCEPT_DIRTY_DATA_ENABLE	BIT(24)

ସଂରଚନା sifive_pl2_quirks {
	bool no_dirty_fill;
};

ସଂରଚନା sifive_pl2 {
	ସଂରଚନା cache_device dev;
	ଶୂନ୍ୟ *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, ସଂରଚନା sifive_pl2, dev)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_pl2_flush_all(ସଂରଚନା cache_device *dev)
{
	ସଂରଚନା sifive_pl2 *pl2_dev = to_pl2(dev);
	ଅକ୍ଷର *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ଯଦି (pl2_dev->no_dirty_fill) {
		config0 = readl((ଶୂନ୍ୟ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (ଶୂନ୍ୟ *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ଯଦି __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ଅନ୍ୟଥା
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + ଆକାର(u32));
#endif
	କର {} ଯେତେବେଳେ (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_pl2_warm_init(ସଂରଚନା cache_device *dev)
{
	ସଂରଚନା sifive_pl2 *pl2_dev = to_pl2(dev);
	ଅକ୍ଷର *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ସଂରଚନା cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sifive_pl2_cold_init(ଅଚଳ ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoff, ଅଚଳ ସଂରଚନା fdt_match *match)
{
	ଅଚଳ ସଂରଚନା sifive_pl2_quirks *quirk = match->data;
	ସଂରଚନା sifive_pl2 *pl2_dev;
	ସଂରଚନା cache_device *dev;
	u64 reg_addr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	/* find the pl2 control base address */
	rc = fdt_get_ଗଣ୍ଠି_addr_size(fdt, nodeoff, 0, &reg_addr, ଖାଲି);
	ଯଦି (rc < 0 && reg_addr)
		ଫେରନ୍ତୁ SBI_ENODEV;

	pl2_dev = sbi_zalloc(ଆକାର(*pl2_dev));
	ଯଦି (!pl2_dev)
		ଫେରନ୍ତୁ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	pl2_dev->addr = (ଶୂନ୍ୟ *)(uintptr_t)reg_addr;
	ଯଦି (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

ସଂରଚନା fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
