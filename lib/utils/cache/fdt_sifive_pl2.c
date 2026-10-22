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

struct sifive_pl2_quirks {
	bool no_dirty_fill;
};

struct sifive_pl2 {
	struct cache_device dev;
	void *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, struct sifive_pl2, dev)

static цяло sifive_pl2_flush_all(struct cache_device *dev)
{
	struct sifive_pl2 *pl2_dev = to_pl2(dev);
	char *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	ако (pl2_dev->no_dirty_fill) {
		config0 = readl((void *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (void *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#ако __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#иначе
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + sizeof(u32));
#endif
	do {} докато (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	върни 0;
}

static цяло sifive_pl2_warm_init(struct cache_device *dev)
{
	struct sifive_pl2 *pl2_dev = to_pl2(dev);
	char *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	върни 0;
}

static struct cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

static цяло sifive_pl2_cold_init(const void *fdt, цяло nodeoff, const struct fdt_match *match)
{
	const struct sifive_pl2_quirks *quirk = match->data;
	struct sifive_pl2 *pl2_dev;
	struct cache_device *dev;
	u64 reg_addr;
	цяло rc;

	/* find the pl2 control base address */
	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &reg_addr, NULL);
	ако (rc < 0 && reg_addr)
		върни SBI_ENODEV;

	pl2_dev = sbi_zalloc(sizeof(*pl2_dev));
	ако (!pl2_dev)
		върни SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	ако (rc)
		върни rc;

	pl2_dev->addr = (void *)(uintptr_t)reg_addr;
	ако (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	върни 0;
}

static const struct sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

static const struct sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

static const struct fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

struct fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
