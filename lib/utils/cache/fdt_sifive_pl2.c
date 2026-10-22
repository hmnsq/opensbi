/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#شامل <libfdt.h>
#شامل <sbi/riscv_io.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi_utils/cache/fdt_cache.h>
#شامل <sbi_utils/fdt/fdt_driver.h>

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

ساختار sifive_pl2_quirks {
	bool no_dirty_fill;
};

ساختار sifive_pl2 {
	ساختار cache_device dev;
	تهی *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, ساختار sifive_pl2, dev)

ایستا صحیح sifive_pl2_flush_all(ساختار cache_device *dev)
{
	ساختار sifive_pl2 *pl2_dev = to_pl2(dev);
	نویسه *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	اگر (pl2_dev->no_dirty_fill) {
		config0 = readl((تهی *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (تهی *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#اگر __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#وگرنه
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + اندازه(u32));
#endif
	انجامبده {} وقتی (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	بازگشت 0;
}

ایستا صحیح sifive_pl2_warm_init(ساختار cache_device *dev)
{
	ساختار sifive_pl2 *pl2_dev = to_pl2(dev);
	نویسه *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	بازگشت 0;
}

ایستا ساختار cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

ایستا صحیح sifive_pl2_cold_init(ثابت تهی *fdt, صحیح nodeoff, ثابت ساختار fdt_match *match)
{
	ثابت ساختار sifive_pl2_quirks *quirk = match->data;
	ساختار sifive_pl2 *pl2_dev;
	ساختار cache_device *dev;
	u64 reg_addr;
	صحیح rc;

	/* find the pl2 control base address */
	rc = fdt_get_گره_addr_size(fdt, nodeoff, 0, &reg_addr, پوچ);
	اگر (rc < 0 && reg_addr)
		بازگشت SBI_ENODEV;

	pl2_dev = sbi_zalloc(اندازه(*pl2_dev));
	اگر (!pl2_dev)
		بازگشت SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	اگر (rc)
		بازگشت rc;

	pl2_dev->addr = (تهی *)(uintptr_t)reg_addr;
	اگر (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	بازگشت 0;
}

ایستا ثابت ساختار sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

ایستا ثابت ساختار sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

ایستا ثابت ساختار fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

ساختار fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
