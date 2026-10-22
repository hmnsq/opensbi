/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#അന്തര്ഭാവ <libfdt.h>
#അന്തര്ഭാവ <sbi/riscv_io.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_heap.h>
#അന്തര്ഭാവ <sbi_utils/cache/fdt_cache.h>
#അന്തര്ഭാവ <sbi_utils/fdt/fdt_driver.h>

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

ഘടന sifive_pl2_quirks {
	bool no_dirty_fill;
};

ഘടന sifive_pl2 {
	ഘടന cache_device dev;
	ശൂന്യം *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, ഘടന sifive_pl2, dev)

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_pl2_flush_all(ഘടന cache_device *dev)
{
	ഘടന sifive_pl2 *pl2_dev = to_pl2(dev);
	അക്ഷരം *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	എങ്കിൽ (pl2_dev->no_dirty_fill) {
		config0 = readl((ശൂന്യം *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (ശൂന്യം *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#എങ്കിൽ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#അല്ലെങ്കിൽ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + വലുപ്പം(u32));
#endif
	ചെയ്യുക {} ആകുമ്പോൾ (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_pl2_warm_init(ഘടന cache_device *dev)
{
	ഘടന sifive_pl2 *pl2_dev = to_pl2(dev);
	അക്ഷരം *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	തിരികെ 0;
}

സ്ഥിതം ഘടന cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ sifive_pl2_cold_init(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoff, സ്ഥിരം ഘടന fdt_match *match)
{
	സ്ഥിരം ഘടന sifive_pl2_quirks *quirk = match->data;
	ഘടന sifive_pl2 *pl2_dev;
	ഘടന cache_device *dev;
	u64 reg_addr;
	പൂർണ്ണസംഖ്യ rc;

	/* find the pl2 control base address */
	rc = fdt_get_കണ്ണി_addr_size(fdt, nodeoff, 0, &reg_addr, ശൂന്യ);
	എങ്കിൽ (rc < 0 && reg_addr)
		തിരികെ SBI_ENODEV;

	pl2_dev = sbi_zalloc(വലുപ്പം(*pl2_dev));
	എങ്കിൽ (!pl2_dev)
		തിരികെ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	എങ്കിൽ (rc)
		തിരികെ rc;

	pl2_dev->addr = (ശൂന്യം *)(uintptr_t)reg_addr;
	എങ്കിൽ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

സ്ഥിതം സ്ഥിരം ഘടന sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

സ്ഥിതം സ്ഥിരം ഘടന fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

ഘടന fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
