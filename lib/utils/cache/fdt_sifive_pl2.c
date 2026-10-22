/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#include <libfdt.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_heap.h>
#include <sbi_utils/cache/fdt_cache.h>
#include <sbi_utils/fdt/fdt_driver.h>

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

sa_nracanaa sifive_pl2_quirks {
	bool no_dirty_fill;
};

sa_nracanaa sifive_pl2 {
	sa_nracanaa cache_device dev;
	void *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, sa_nracanaa sifive_pl2, dev)

static puur_naa_nka sifive_pl2_flush_all(sa_nracanaa cache_device *dev)
{
	sa_nracanaa sifive_pl2 *pl2_dev = to_pl2(dev);
	var_na *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ᯀᯮᯝ (pl2_dev->no_dirty_fill) {
		config0 = readl((void *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (void *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ᯀᯮᯝ __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#ᯀᯮᯝᯂᯪ
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + parimaa_na(u32));
#endif
	do {} ᯀᯬᯝ (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	ᯀᯮᯞ 0;
}

static puur_naa_nka sifive_pl2_warm_init(sa_nracanaa cache_device *dev)
{
	sa_nracanaa sifive_pl2 *pl2_dev = to_pl2(dev);
	var_na *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	ᯀᯮᯞ 0;
}

static sa_nracanaa cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

static puur_naa_nka sifive_pl2_cold_init(const void *fdt, puur_naa_nka nodeoff, const sa_nracanaa fdt_match *match)
{
	const sa_nracanaa sifive_pl2_quirks *quirk = match->data;
	sa_nracanaa sifive_pl2 *pl2_dev;
	sa_nracanaa cache_device *dev;
	u64 reg_addr;
	puur_naa_nka rc;

	/* find the pl2 control base address */
	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, NULL);
	ᯀᯮᯝ (rc < 0 && reg_addr)
		ᯀᯮᯞ SBI_ENODEV;

	pl2_dev = sbi_zalloc(parimaa_na(*pl2_dev));
	ᯀᯮᯝ (!pl2_dev)
		ᯀᯮᯞ SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	ᯀᯮᯝ (rc)
		ᯀᯮᯞ rc;

	pl2_dev->addr = (void *)(uintptr_t)reg_addr;
	ᯀᯮᯝ (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	ᯀᯮᯞ 0;
}

static const sa_nracanaa sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

static const sa_nracanaa sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

static const sa_nracanaa fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

sa_nracanaa fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
