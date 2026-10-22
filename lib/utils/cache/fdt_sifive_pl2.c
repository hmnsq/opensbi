/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#અન્તર્ભાવ <libfdt.h>
#અન્તર્ભાવ <sbi/riscv_io.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi_utils/cache/fdt_cache.h>
#અન્તર્ભાવ <sbi_utils/fdt/fdt_driver.h>

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

સંરચના sifive_pl2_quirks {
	bool no_dirty_fill;
};

સંરચના sifive_pl2 {
	સંરચના cache_device dev;
	શૂન્ય *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, સંરચના sifive_pl2, dev)

સ્થિર પૂર્ણાંક sifive_pl2_flush_all(સંરચના cache_device *dev)
{
	સંરચના sifive_pl2 *pl2_dev = to_pl2(dev);
	અક્ષર *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	જો (pl2_dev->no_dirty_fill) {
		config0 = readl((શૂન્ય *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (શૂન્ય *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#જો __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#નહીંતર
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + કદ(u32));
#endif
	કરો {} જ્યારે (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	પાછા 0;
}

સ્થિર પૂર્ણાંક sifive_pl2_warm_init(સંરચના cache_device *dev)
{
	સંરચના sifive_pl2 *pl2_dev = to_pl2(dev);
	અક્ષર *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	પાછા 0;
}

સ્થિર સંરચના cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

સ્થિર પૂર્ણાંક sifive_pl2_cold_init(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoff, અચલ સંરચના fdt_match *match)
{
	અચલ સંરચના sifive_pl2_quirks *quirk = match->data;
	સંરચના sifive_pl2 *pl2_dev;
	સંરચના cache_device *dev;
	u64 reg_addr;
	પૂર્ણાંક rc;

	/* find the pl2 control base address */
	rc = fdt_get_ગાંઠ_addr_size(fdt, nodeoff, 0, &reg_addr, ખાલી);
	જો (rc < 0 && reg_addr)
		પાછા SBI_ENODEV;

	pl2_dev = sbi_zalloc(કદ(*pl2_dev));
	જો (!pl2_dev)
		પાછા SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	જો (rc)
		પાછા rc;

	pl2_dev->addr = (શૂન્ય *)(uintptr_t)reg_addr;
	જો (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	પાછા 0;
}

સ્થિર અચલ સંરચના sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

સ્થિર અચલ સંરચના sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

સ્થિર અચલ સંરચના fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

સંરચના fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
