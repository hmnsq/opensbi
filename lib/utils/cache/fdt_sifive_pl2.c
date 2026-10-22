/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 SiFive Inc.
 */

#அந்தர்பாவ <libfdt.h>
#அந்தர்பாவ <sbi/riscv_io.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_heap.h>
#அந்தர்பாவ <sbi_utils/cache/fdt_cache.h>
#அந்தர்பாவ <sbi_utils/fdt/fdt_driver.h>

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

கட்டமைப்பு sifive_pl2_quirks {
	bool no_dirty_fill;
};

கட்டமைப்பு sifive_pl2 {
	கட்டமைப்பு cache_device dev;
	வெறுமை *addr;
	bool no_dirty_fill;
};

#define to_pl2(_dev) container_of(_dev, கட்டமைப்பு sifive_pl2, dev)

நிலையான முழு sifive_pl2_flush_all(கட்டமைப்பு cache_device *dev)
{
	கட்டமைப்பு sifive_pl2 *pl2_dev = to_pl2(dev);
	எழுத்து *addr = pl2_dev->addr;
	u64 cmd = (FLUSH64_CMD_TARGET_ALL | FLUSH64_CMD_TYPE_FLUSH) << FLUSH64_CMD_POS;
	u32 config0;

	/*
	 * While flushing pl2 cache, a speculative load might causes a dirty line pull
	 * into PL2. It will cause the SiFive SMC0 refuse to enter the power gating.
	 * Disable the ACCEPT_DIRTY_DATA_ENABLE to avoid the issue.
	 */
	எனில் (pl2_dev->no_dirty_fill) {
		config0 = readl((வெறுமை *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
		config0 &= ~CONFIG0_ACCEPT_DIRTY_DATA_ENABLE;
		writel(config0, (வெறுமை *)addr + SIFIVE_PL2CACHE_CONFIG0_OFF);
	}

#எனில் __riscv_xlen != 32
	writeq(cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
#இல்லையெனில்
	writel((u32)cmd, addr + SIFIVE_PL2CACHE_FLUSH64_OFF);
	writel((u32)(cmd >> 32), addr + SIFIVE_PL2CACHE_FLUSH64_OFF + அளவு(u32));
#endif
	செய் {} போது (readl(addr + SIFIVE_PL2CACHE_STATUS_OFF) & SIFIVE_PL2CACHE_CMD_QLEN);

	திரும்பு 0;
}

நிலையான முழு sifive_pl2_warm_init(கட்டமைப்பு cache_device *dev)
{
	கட்டமைப்பு sifive_pl2 *pl2_dev = to_pl2(dev);
	எழுத்து *addr = pl2_dev->addr;
	u32 val;

	/* Enabling the clock gating */
	val = readl(addr + SIFIVE_PL2CACHE_CONFIG1_OFF);
	val &= (~REGIONCLOCKDISABLE_MASK);
	writel(val, addr + SIFIVE_PL2CACHE_CONFIG1_OFF);

	திரும்பு 0;
}

நிலையான கட்டமைப்பு cache_ops sifive_pl2_ops = {
	.warm_init = sifive_pl2_warm_init,
	.cache_flush_all = sifive_pl2_flush_all,
};

நிலையான முழு sifive_pl2_cold_init(மாறிலி வெறுமை *fdt, முழு nodeoff, மாறிலி கட்டமைப்பு fdt_match *match)
{
	மாறிலி கட்டமைப்பு sifive_pl2_quirks *quirk = match->data;
	கட்டமைப்பு sifive_pl2 *pl2_dev;
	கட்டமைப்பு cache_device *dev;
	u64 reg_addr;
	முழு rc;

	/* find the pl2 control base address */
	rc = fdt_get_முனை_addr_size(fdt, nodeoff, 0, &reg_addr, வெற்று);
	எனில் (rc < 0 && reg_addr)
		திரும்பு SBI_ENODEV;

	pl2_dev = sbi_zalloc(அளவு(*pl2_dev));
	எனில் (!pl2_dev)
		திரும்பு SBI_ENOMEM;

	dev = &pl2_dev->dev;
	dev->ops = &sifive_pl2_ops;
	dev->cpu_private = true;

	rc = fdt_cache_add(fdt, nodeoff, dev);
	எனில் (rc)
		திரும்பு rc;

	pl2_dev->addr = (வெறுமை *)(uintptr_t)reg_addr;
	எனில் (quirk)
		pl2_dev->no_dirty_fill = quirk->no_dirty_fill;

	திரும்பு 0;
}

நிலையான மாறிலி கட்டமைப்பு sifive_pl2_quirks pl2cache2_quirks = {
	.no_dirty_fill = true,
};

நிலையான மாறிலி கட்டமைப்பு sifive_pl2_quirks pl2cache0_quirks = {
	.no_dirty_fill = false,
};

நிலையான மாறிலி கட்டமைப்பு fdt_match sifive_pl2_match[] = {
	{ .compatible = "sifive,pl2cache2", .data = &pl2cache2_quirks },
	{ .compatible = "sifive,pl2cache1", .data = &pl2cache0_quirks },
	{ .compatible = "sifive,pl2cache0", .data = &pl2cache0_quirks },
	{},
};

கட்டமைப்பு fdt_driver fdt_sifive_pl2 = {
	.match_table = sifive_pl2_match,
	.init = sifive_pl2_cold_init,
};
