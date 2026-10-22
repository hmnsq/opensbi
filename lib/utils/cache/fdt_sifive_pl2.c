/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#अन्तर्भाव <libfdt.h>
#अन्तर्भाव <sbi/riscv_io.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi_utils/cache/fdt_cache.h>
#अन्तर्भाव <sbi_utils/fdt/fdt_driver.h>

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

संरचना sifive_pl2_quirks {
	bool no_dirty_fill;
};

संरचना sifive_pl2 {
	संरचना cache_device dev;
	शून्य *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, संरचना sifive_pl2, dev)

स्थिर पूर्णाङ्क sifive_pl2_flush_all(संरचना cache_device *dev)
{
	संरचना sifive_pl2 *pl2_dev = to_pl2(dev);
	वर्ण *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	सचे (pl2_dev->no_dirty_fill) {
		config0 = readl((शून्य *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (शून्य *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#सचे __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#अञ्ञथा
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + परिमाण(u32));
#endif
	करोहि {} याव (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क sifive_pl2_warm_init(संरचना cache_device *dev)
{
	संरचना sifive_pl2 *pl2_dev = to_pl2(dev);
	वर्ण *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	पटिनिवत्त 0;
}

स्थिर संरचना cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

स्थिर पूर्णाङ्क sifive_pl2_cold_init(ध्रुव शून्य *fdt, पूर्णाङ्क nodeoff, ध्रुव संरचना fdt_match *match)
{
	ध्रुव संरचना sifive_pl2_quirks *quirk = match->data;
	संरचना sifive_pl2 *pl2_dev;
	संरचना cache_device *dev;
	u64 reg_addr;
	पूर्णाङ्क rc;

	/* find the pl2 control base address */
	rc = fdt_get_ग्रन्थिः_addr_size(fdt, nodeoff, 0, &reg_addr, रिक्त);
	सचे (rc < 0 && reg_addr)
		पटिनिवत्त SBI_ENODEV;

	pl2_dev = sbi_zalloc(परिमाण(*pl2_dev));
	सचे (!pl2_dev)
		पटिनिवत्त SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	सचे (rc)
		पटिनिवत्त rc;

	pl2_dev->addr = (शून्य *)(uintptr_t)reg_addr;
	सचे (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	पटिनिवत्त 0;
}

स्थिर ध्रुव संरचना sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

स्थिर ध्रुव संरचना sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

स्थिर ध्रुव संरचना fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

संरचना fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
